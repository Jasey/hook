// hookexe.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

DWORD dwOffect,dwArgu;

BOOL CreateRemoteDll(const char *DllFullPath, const DWORD dwRemoteProcessId /**,DWORD dwOffect,DWORD dwArgu**/)
{
	HANDLE hToken;
	if ( OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken) )
	{
		TOKEN_PRIVILEGES tkp;

		LookupPrivilegeValue( NULL,SE_DEBUG_NAME,&tkp.Privileges[0].Luid );//�޸Ľ���Ȩ��
		tkp.PrivilegeCount=1;
		tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges( hToken,FALSE,&tkp,sizeof tkp,NULL,NULL );//֪ͨϵͳ�޸Ľ���Ȩ��
		CloseHandle(hToken);
	}

	HANDLE hRemoteProcess;

	//��Զ���߳�
	if( (hRemoteProcess = OpenProcess( PROCESS_CREATE_THREAD |    //����Զ�̴����߳�
		PROCESS_VM_OPERATION |                //����Զ��VM����
		PROCESS_VM_WRITE|                    //����Զ��VMд
		PROCESS_ALL_ACCESS,
		FALSE, dwRemoteProcessId ) )== NULL )
	{
		printf("OpenProcess Error!");
		return FALSE;
	}

	char *pszLibFileRemote;
	//��Զ�̽��̵��ڴ��ַ�ռ����DLL�ļ���������
	pszLibFileRemote = (char *) VirtualAllocEx( hRemoteProcess, NULL, lstrlen(DllFullPath)+1, 
		MEM_COMMIT, PAGE_READWRITE);
	if(pszLibFileRemote == NULL)
	{
		printf("VirtualAllocEx error! ");
		CloseHandle(hRemoteProcess);
		return FALSE;
	}

	//��DLL��·�������Ƶ�Զ�̽��̵��ڴ�ռ�
	if( WriteProcessMemory(hRemoteProcess,
		pszLibFileRemote, (void *) DllFullPath, lstrlen(DllFullPath)+1, NULL) == 0)
	{
		printf("WriteProcessMemory Error");
		CloseHandle(hRemoteProcess);
		return FALSE;
	}

	//����LoadLibraryA����ڵ�ַ
	PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)
		GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryA");

	if(pfnStartAddr == NULL)
	{
		printf("GetProcAddress Error");
		return FALSE;
	}

	HANDLE hRemoteThread;

	//Զ�̵���loadLibraryA����dll
	hRemoteThread = CreateRemoteThread( hRemoteProcess, NULL, 0, 
		pfnStartAddr, pszLibFileRemote, 0, NULL);
	WaitForSingleObject(hRemoteThread,INFINITE);
	if( hRemoteThread == NULL)
	{
		printf("CreateRemoteThread Error");
		CloseHandle(hRemoteProcess);
		return FALSE;
	}
	/**
	DWORD dwDllAddr;
	//���ý���֮��ִ��GetExitCodeThread���ɻ��dll�׵�ַ
	GetExitCodeThread(hRemoteThread,&dwDllAddr);
// 	DWORD dwNum = 0x3480;
	if(dwDllAddr!=0)
	{	
		dwDllAddr += dwOffect;
		HANDLE hHookFunc;
		hHookFunc = CreateRemoteThread( hRemoteProcess, NULL, 0, 
			(PTHREAD_START_ROUTINE)dwDllAddr, (LPVOID)dwArgu, 0, NULL);
		WaitForSingleObject(hHookFunc,INFINITE);
		if( hHookFunc == NULL)
		{
			CloseHandle(hRemoteThread);
			CloseHandle(hRemoteProcess);
			return FALSE;
		}
		CloseHandle(hHookFunc);

	}
	else
	{
		CloseHandle(hRemoteProcess);
		CloseHandle(hRemoteThread);
		return FALSE;

	}**/
	CloseHandle(hRemoteProcess);
	CloseHandle(hRemoteThread);
	return TRUE;
}



void Hook(int dwPid)
{ 
	char curpath[260];
	GetModuleFileName(NULL,curpath,260);
	*strrchr(curpath,'\\') = '\0';
	strcat(curpath,"\\Hook.dll");

	printf("dll path : %s", curpath);

	if(CreateRemoteDll(curpath,dwPid/**,dwOffect,dwArgu**/))
		printf("�����ɹ�\n");
	else
		printf("����ʧ��\n");

}

int main(int argc, char* argv[])
{
	int pid;
	printf("�������pid\n");
	scanf("%d",&pid);
	Hook(pid);
	getchar();
	return 0;
}

