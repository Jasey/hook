// Process.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <tlhelp32.h>
int KillProcess(DWORD Pid)
{
	//�򿪽��̵õ����̾��
	HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,Pid);
	if(hProcess==NULL)
	{ 		
		printf("OpenProcess error\n");
		return 0;
	}
	//��������
	if (TerminateProcess(hProcess,0))
	{
		printf("�������̳ɹ�\n");
		return 0;
	}
	else
	{
		printf("��������ʧ��\n");
		return 0;
	}	
}

int GetProcess()
{
	char buff[1024]={0};
	PROCESSENTRY32 pe32;
	pe32.dwSize=sizeof(pe32);
	//���ϵͳ�����н��̿���
	HANDLE hProcessSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hProcessSnap==INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot error");
		return 0;
	}
	//ö���б��еĵ�һ������
	BOOL bProcess=Process32First(hProcessSnap,&pe32);
	while(bProcess)
	{
		//��ʽ���������ͽ���ID
		wsprintf(buff,"%s---------------%d\r\n",pe32.szExeFile,pe32.th32ProcessID);
		//����������ͽ���ID
		printf(buff);
		memset(buff,0x00,1024);	
		//����ö�ٽ���
		bProcess=Process32Next(hProcessSnap,&pe32);
	}
	CloseHandle(hProcessSnap);
	return 0;
}


int main(int argc, char* argv[])
{
	if(argc==2&&strcmp(argv[1],"list")==0)
	{
		GetProcess();
	}
	if(argc==3&&strcmp(argv[1],"kill")==0)
	{
		KillProcess(atoi(argv[2]));

	}
	return 0;
}
