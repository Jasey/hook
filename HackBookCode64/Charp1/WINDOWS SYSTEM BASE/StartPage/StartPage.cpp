// StartPage.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <stdio.h>
//�����޸��ַ������ͼ�ֵ
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
	//Ҫ�޸ĳɵ���ַ
	char StartPage[255]="http://www.baidu.cn/";
	//�����޸��ַ������ͼ�ֵ�ĺ���
 	CreateStringReg(HKEY_CURRENT_USER,"Software\\Microsoft\\Internet Explorer\\Main","Start Page",StartPage);
	return 0;
}



