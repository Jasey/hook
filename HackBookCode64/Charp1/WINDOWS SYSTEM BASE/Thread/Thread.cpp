// Thread.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
HANDLE hFile;
//�����ٽ�������
CRITICAL_SECTION cs;
//д�ļ��̺߳���
DWORD WINAPI Thread(LPVOID lpParam)
{
	//�õ����ĸ��߳�
	int n=(int)lpParam;
	DWORD dwWrite;
	for (int i=0;i<10000;i++)
	{	//�����ٽ���
		EnterCriticalSection(&cs);
		char Data[512]="------------------------by �����ѩ------------------------\r\n-------------------http://www.nohack.cn-------------------";
		//д���ļ�
		WriteFile(hFile,&Data,strlen(Data),&dwWrite,NULL);
		//���ٽ���
		LeaveCriticalSection(&cs);
	}
	//����ĸ��߳����н���
	printf("��%d���߳̽�������\n",n);
	return 0;
}
int main(int argc, char* argv[])
{
	//�����ļ�
	hFile=CreateFile("c:\\hack.txt",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
	{
		printf("CreateFile Error\n");
		return 0;
	}
	DWORD ThreadId;
	HANDLE hThread[5];
	//��ʼ���ٽ�������
	InitializeCriticalSection(&cs);
	//����5���߳�
	for(int i=0;i<5;i++)
	{
		hThread[i]=CreateThread(NULL,NULL,Thread,LPVOID(i+1),0,&ThreadId);
		printf("��%d���̴߳����ɹ�\n",i+1);		
	}
	//�ȴ�����߳����н���
	WaitForMultipleObjects(5,hThread,true,INFINITE);
	//ɾ���ٽ�������
	DeleteCriticalSection(&cs);
	//�ر��ļ����
	CloseHandle(hFile);
	return 0;
}
