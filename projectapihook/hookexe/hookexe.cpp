// hookexe.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows>
#include <tlhelp32.h>
#include <tchar.h>
using namespace std;

DWORD dwOffect,dwArgu;

BOOL CreateRemoteDll(const char *DllFullPath, const DWORD dwRemoteProcessId)
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
	CloseHandle(hRemoteProcess);
	CloseHandle(hRemoteThread);
	return TRUE;
}



void Hook(int dwPid, char* curpath)
{ 

	if(CreateRemoteDll(curpath,dwPid))
		printf("�����ɹ�\n");
	else
		printf("����ʧ��\n");

}

int ProcessFind(char * Exename)
{
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (!hProcess)
	{
		return FALSE;
	}
	PROCESSENTRY32 info;
	info.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hProcess,&info))
	{
		return FALSE;
	}
	while (true)
	{
		if (_tcscmp(info.szExeFile,Exename)==0)
		{
			return info.th32ProcessID;
		}
		if (!Process32Next(hProcess,&info))
		{
			return FALSE;
		}
	}
	return FALSE;

}


int main(int argc, char* argv[])
{
	char processName[260];
	printf("���������:\n");
	scanf("%s",&processName);
	char curpath[260];
	printf("����dll·��:\n");
	scanf("%s",&curpath);

	int pid = ProcessFind(processName);
	while(!pid){
		Sleep(50);
		pid = ProcessFind(processName);
	}

	printf("%s process id : %d\n", processName, pid);
	Hook(pid, curpath);
	getchar();
	return 0;
}
