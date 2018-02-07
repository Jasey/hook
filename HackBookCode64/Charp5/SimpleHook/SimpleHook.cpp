// SimpleHook.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
INT64* lpAddr;
PROC OldProc;
char LogMessage[255];

BOOL  WINAPI  MyTerminateProcess(HANDLE hProcess,UINT uExitCode)
{
	MessageBox(NULL,"没法结束进程了吧","API HOOK",0);
	return 1;
}

int  ApiHook(char *DllName,//DLL文件名
			  PROC OldFunAddr,//要HOOK的函数地址
			  PROC NewFunAddr//我们够造的函数地址
			  )
{
	//得到函数进程模块基地址
	HMODULE lpBase = GetModuleHandle(NULL);
	IMAGE_DOS_HEADER *dosHeader;
	IMAGE_NT_HEADERS *ntHeader;
	IMAGE_IMPORT_BY_NAME *ImportName;
	//定位到DOS头
	dosHeader=(IMAGE_DOS_HEADER*)lpBase;
	//定位到PE头
	ntHeader=(IMAGE_NT_HEADERS64*)((BYTE*)lpBase+dosHeader->e_lfanew);
	//定位到导入表
	IMAGE_IMPORT_DESCRIPTOR *pImportDesc=(IMAGE_IMPORT_DESCRIPTOR*)((BYTE*)lpBase+ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	//循环遍历IMAGE_IMPORT_DESCRIPTOR机构数组

	wsprintf(LogMessage, "TerminateProcess function' DLL Name : %s\n", DllName);
	OutputDebugString (LogMessage);

	wsprintf(LogMessage, "TerminateProcess address : 0x%I64X\n", (INT64)OldFunAddr);
	OutputDebugString (LogMessage);

	wsprintf(LogMessage, "MyTerminateProcess address : 0x%I64X\n", (INT64)NewFunAddr);
	OutputDebugString (LogMessage);

	while(pImportDesc->FirstThunk)
	{
		//得到DLL文件名
		char* pszDllName = (char*)((BYTE*)lpBase + pImportDesc->Name);
		//比较得到的DLL文件名是否和要HOOK函数所在的DLL相同
		if(lstrcmpiA(pszDllName, DllName) == 0)
		{
			wsprintf(LogMessage, "Find The %s's Descriptor\n", DllName);
			OutputDebugString (LogMessage);
			break;
		}
		pImportDesc++;
	}
	//定位到FirstThunk参数指向的IMAGE_THUNK_DATA，此时这个结构已经是函数入口点地址了
	IMAGE_THUNK_DATA* pThunk = (IMAGE_THUNK_DATA*)
		((BYTE*)lpBase + pImportDesc->FirstThunk);
	//遍历这部分IAT表
	while(pThunk->u1.Function)
	{
		lpAddr = (INT64*)&(pThunk->u1.Function);
		//比较函数地址是否相同
		if(*lpAddr == (INT64)OldFunAddr)
		{	
			wsprintf(LogMessage, "Find the TerminateProcess function enter address : 0x%X, it's content : 0x%X\n", lpAddr, *lpAddr);
			OutputDebugString (LogMessage);
			DWORD dwOldProtect;
			//修改内存包含属性
			VirtualProtect(lpAddr, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect);
			//API函数的入口点地址改成我们构造的函数的地址
			WriteProcessMemory(GetCurrentProcess(),lpAddr, &NewFunAddr, sizeof(INT64), NULL);

			wsprintf(LogMessage, "after hook function enter address : 0x%I64X, it's content : 0x%I64X\n", lpAddr, *lpAddr);
			OutputDebugString (LogMessage);
		}
		pThunk++;
	}
	return 0;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		MessageBox(NULL,"DLL 注入成功","API HOOK",0);
		//得到TerminateProcess函数地址
		OldProc = (PROC)TerminateProcess;
		//定位，修改IAT表
		ApiHook("kernel32.dll",OldProc,(PROC)MyTerminateProcess);
		break;
	case DLL_PROCESS_DETACH:
		MessageBox(NULL,"DLL 卸载成功","API HOOK",0);
		//恢复IAT表中API函数的入口点地址
		WriteProcessMemory(GetCurrentProcess(),lpAddr, &OldProc, sizeof(INT64), NULL);
		break;	
	}
    return TRUE;
}

