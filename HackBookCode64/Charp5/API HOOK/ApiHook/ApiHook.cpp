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
	//���ϵͳ�����н��̿���
	HANDLE hProcessSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hProcessSnap==INVALID_HANDLE_VALUE)
	{	
		return FALSE;
	}
	//ö���б��еĵ�һ������
	BOOL bProcess=Process32First(hProcessSnap,&pe32);
	while(bProcess)
	{
		//�Ƚϵõ��Ľ�������Ҫ�����Ľ������Ƿ���ͬ
		if(stricmp(pe32.szExeFile,ProcessName)==0)
		{
			//��ͬ�򷵻ش˽���ID
			return pe32.th32ProcessID;
		}
		bProcess=Process32Next(hProcessSnap,&pe32);
	}
	CloseHandle(hProcessSnap);
	//���򷵻�0
	return 0;
}

HANDLE __stdcall  MyOpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId)
{
	//Ѱ��Ҫ�������̵�ID
	if(GetProcessId("SetHook.exe")==dwProcessId)
		return 0;
	//���粻��Ҫ�����Ľ��̣���ִ��OpenProcess�������������䷵��ֵ
	return ((OPENPROCESS)OldProc)(dwDesiredAccess, bInheritHandle, dwProcessId);
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
	ntHeader=(IMAGE_NT_HEADERS32*)((BYTE*)lpBase+dosHeader->e_lfanew);
	//��λ�������
	IMAGE_IMPORT_DESCRIPTOR *pImportDesc=(IMAGE_IMPORT_DESCRIPTOR*)((BYTE*)lpBase+ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	//ѭ������IMAGE_IMPORT_DESCRIPTOR��������
	while(pImportDesc->FirstThunk)
	{
		//�õ�DLL�ļ���
		char* pszDllName = (char*)((BYTE*)lpBase + pImportDesc->Name);
		//�Ƚϵõ���DLL�ļ����Ƿ��ҪHOOK�������ڵ�DLL��ͬ
		if(lstrcmpiA(pszDllName, DllName) == 0)
		{
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
		lpAddr = (DWORD*)&(pThunk->u1.Function);
		//�ȽϺ�����ַ�Ƿ���ͬ
		if(*lpAddr == (DWORD)OldFunAddr)
		{	
			DWORD dwOldProtect;
			//�޸��ڴ��������
			VirtualProtect(lpAddr, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect);
			//API��������ڵ��ַ�ĳ����ǹ���ĺ����ĵ�ַ
			WriteProcessMemory(GetCurrentProcess(),lpAddr, &NewFunAddr, sizeof(DWORD), NULL);
		}
		pThunk++;
	}
	return 0;
}

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    //����Ϣ���ݸ���һ������
	return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}
BOOL StartHook()
{
	if (g_hHook!=NULL) 
		return FALSE;
	//��װ����
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
		//��dll����ʱ���ҹ�OpenProcess����
		ApiHook("kernel32.dll",OldProc,(PROC)MyOpenProcess);
		break;
	case DLL_PROCESS_DETACH:
		//��dllж��ʱ����ԭOpenProcess����
		WriteProcessMemory(GetCurrentProcess(),lpAddr, &OldProc, sizeof(DWORD), NULL);
		if (g_hHook != NULL) UnhookWindowsHookEx(g_hHook);
		break;
	}
    return TRUE;
}

