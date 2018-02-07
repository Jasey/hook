// Thread.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
HANDLE hFile;
//定义临界区对象
CRITICAL_SECTION cs;
//写文件线程函数
DWORD WINAPI Thread(LPVOID lpParam)
{
	//得到是哪个线程
	int n=(int)lpParam;
	DWORD dwWrite;
	for (int i=0;i<10000;i++)
	{	//进入临界区
		EnterCriticalSection(&cs);
		char Data[512]="------------------------by 认真的雪------------------------\r\n-------------------http://www.nohack.cn-------------------";
		//写入文件
		WriteFile(hFile,&Data,strlen(Data),&dwWrite,NULL);
		//出临界区
		LeaveCriticalSection(&cs);
	}
	//输出哪个线程运行结束
	printf("第%d号线程结束运行\n",n);
	return 0;
}
int main(int argc, char* argv[])
{
	//创建文件
	hFile=CreateFile("c:\\hack.txt",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
	{
		printf("CreateFile Error\n");
		return 0;
	}
	DWORD ThreadId;
	HANDLE hThread[5];
	//初始化临界区对象
	InitializeCriticalSection(&cs);
	//创建5个线程
	for(int i=0;i<5;i++)
	{
		hThread[i]=CreateThread(NULL,NULL,Thread,LPVOID(i+1),0,&ThreadId);
		printf("第%d号线程创建成功\n",i+1);		
	}
	//等待五个线程运行结束
	WaitForMultipleObjects(5,hThread,true,INFINITE);
	//删除临界区对象
	DeleteCriticalSection(&cs);
	//关闭文件句柄
	CloseHandle(hFile);
	return 0;
}
