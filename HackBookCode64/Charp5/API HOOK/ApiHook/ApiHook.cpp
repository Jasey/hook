// ApiHook.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <tlhelp32.h>
#include <stdio.h>
DWORD* lpAddr;
PROC OldProc = (PROC)OpenProcess;
HINSTANCE g_hInstance = NULL; 
HHOOK  g_hHook = NULL; 

typedef HANDLE (__stdcall *OPENPROCESS)(DWORD,BOOL,DWORD); 

DWORD GetProcessId(char *ProcessName)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize=sizeof(pe32);
	//获得系统内所有进程快照
	HANDLE hProcessSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hProcessSnap==INVALID_HANDLE_VALUE)
	{	
		return FALSE;
	}
	//枚举列表中的第一个进程
	BOOL bProcess=Process32First(hProcessSnap,&pe32);
	while(bProcess)
	{
		//比较得到的进程名和要保护的进程名是否相同
		if(stricmp(pe32.szExeFile,ProcessName)==0)
		{
			//相同则返回此进程ID
			return pe32.th32ProcessID;
		}
		bProcess=Process32Next(hProcessSnap,&pe32);
	}
	CloseHandle(hProcessSnap);
	//否则返回0
	return 0;
}

HANDLE __stdcall  MyOpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId)
{
	//寻找要保护进程的ID
	if(GetProcessId("SetHook.exe")==dwProcessId)
		return 0;
	//假如不是要保护的进程，则执行OpenProcess函数，并返回其返回值
	return ((OPENPROCESS)OldProc)(dwDesiredAccess, bInheritHandle, dwProcessId);
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
	ntHeader=(IMAGE_NT_HEADERS32*)((BYTE*)lpBase+dosHeader->e_lfanew);
	//定位到导入表
	IMAGE_IMPORT_DESCRIPTOR *pImportDesc=(IMAGE_IMPORT_DESCRIPTOR*)((BYTE*)lpBase+ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	//循环遍历IMAGE_IMPORT_DESCRIPTOR机构数组
	while(pImportDesc->FirstThunk)
	{
		//得到DLL文件名
		char* pszDllName = (char*)((BYTE*)lpBase + pImportDesc->Name);
		//比较得到的DLL文件名是否和要HOOK函数所在的DLL相同
		if(lstrcmpiA(pszDllName, DllName) == 0)
		{
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
		lpAddr = (DWORD*)&(pThunk->u1.Function);
		//比较函数地址是否相同
		if(*lpAddr == (DWORD)OldFunAddr)
		{	
			DWORD dwOldProtect;
			//修改内存包含属性
			VirtualProtect(lpAddr, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect);
			//API函数的入口点地址改成我们构造的函数的地址
			WriteProcessMemory(GetCurrentProcess(),lpAddr, &NewFunAddr, sizeof(DWORD), NULL);
		}
		pThunk++;
	}
	return 0;
}

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    //把消息传递给下一个钩子
	return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}
BOOL StartHook()
{
	if (g_hHook!=NULL) 
		return FALSE;
	//安装钩子
	g_hHook = SetWindowsHookEx(WH_GETMESSAGE, HookProc, g_hInstance, NULL);
	return TRUE; 
}
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	g_hInstance = (HINSTANCE)hModule;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//当dll加载时，挂钩OpenProcess函数
		ApiHook("kernel32.dll",OldProc,(PROC)MyOpenProcess);
		break;
	case DLL_PROCESS_DETACH:
		//当dll卸载时，还原OpenProcess函数
		WriteProcessMemory(GetCurrentProcess(),lpAddr, &OldProc, sizeof(DWORD), NULL);
		if (g_hHook != NULL) UnhookWindowsHookEx(g_hHook);
		break;
	}
    return TRUE;
}

