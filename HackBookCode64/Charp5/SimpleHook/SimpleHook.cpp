// SimpleHook.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
INT64* lpAddr;
PROC OldProc;
char LogMessage[255];

BOOL  WINAPI  MyTerminateProcess(HANDLE hProcess,UINT uExitCode)
{
	MessageBox(NULL,"û�����������˰�","API HOOK",0);
	return 1;
}

int  ApiHook(char *DllName,//DLL�ļ���
			  PROC OldFunAddr,//ҪHOOK�ĺ�����ַ
			  PROC NewFunAddr//���ǹ���ĺ�����ַ
			  )
{
	//�õ���������ģ�����ַ
	HMODULE lpBase = GetModuleHandle(NULL);
	IMAGE_DOS_HEADER *dosHeader;
	IMAGE_NT_HEADERS *ntHeader;
	IMAGE_IMPORT_BY_NAME *ImportName;
	//��λ��DOSͷ
	dosHeader=(IMAGE_DOS_HEADER*)lpBase;
	//��λ��PEͷ
	ntHeader=(IMAGE_NT_HEADERS64*)((BYTE*)lpBase+dosHeader->e_lfanew);
	//��λ�������
	IMAGE_IMPORT_DESCRIPTOR *pImportDesc=(IMAGE_IMPORT_DESCRIPTOR*)((BYTE*)lpBase+ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	//ѭ������IMAGE_IMPORT_DESCRIPTOR��������

	wsprintf(LogMessage, "TerminateProcess function' DLL Name : %s\n", DllName);
	OutputDebugString (LogMessage);

	wsprintf(LogMessage, "TerminateProcess address : 0x%I64X\n", (INT64)OldFunAddr);
	OutputDebugString (LogMessage);

	wsprintf(LogMessage, "MyTerminateProcess address : 0x%I64X\n", (INT64)NewFunAddr);
	OutputDebugString (LogMessage);

	while(pImportDesc->FirstThunk)
	{
		//�õ�DLL�ļ���
		char* pszDllName = (char*)((BYTE*)lpBase + pImportDesc->Name);
		//�Ƚϵõ���DLL�ļ����Ƿ��ҪHOOK�������ڵ�DLL��ͬ
		if(lstrcmpiA(pszDllName, DllName) == 0)
		{
			wsprintf(LogMessage, "Find The %s's Descriptor\n", DllName);
			OutputDebugString (LogMessage);
			break;
		}
		pImportDesc++;
	}
	//��λ��FirstThunk����ָ���IMAGE_THUNK_DATA����ʱ����ṹ�Ѿ��Ǻ�����ڵ��ַ��
	IMAGE_THUNK_DATA* pThunk = (IMAGE_THUNK_DATA*)
		((BYTE*)lpBase + pImportDesc->FirstThunk);
	//�����ⲿ��IAT��
	while(pThunk->u1.Function)
	{
		lpAddr = (INT64*)&(pThunk->u1.Function);
		//�ȽϺ�����ַ�Ƿ���ͬ
		if(*lpAddr == (INT64)OldFunAddr)
		{	
			wsprintf(LogMessage, "Find the TerminateProcess function enter address : 0x%X, it's content : 0x%X\n", lpAddr, *lpAddr);
			OutputDebugString (LogMessage);
			DWORD dwOldProtect;
			//�޸��ڴ��������
			VirtualProtect(lpAddr, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect);
			//API��������ڵ��ַ�ĳ����ǹ���ĺ����ĵ�ַ
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
		MessageBox(NULL,"DLL ע��ɹ�","API HOOK",0);
		//�õ�TerminateProcess������ַ
		OldProc = (PROC)TerminateProcess;
		//��λ���޸�IAT��
		ApiHook("kernel32.dll",OldProc,(PROC)MyTerminateProcess);
		break;
	case DLL_PROCESS_DETACH:
		MessageBox(NULL,"DLL ж�سɹ�","API HOOK",0);
		//�ָ�IAT����API��������ڵ��ַ
		WriteProcessMemory(GetCurrentProcess(),lpAddr, &OldProc, sizeof(INT64), NULL);
		break;	
	}
    return TRUE;
}

