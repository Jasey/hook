// ezj.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <string>
int main(int argc, char* argv[])
{
	char Path[255];
	char FileName[255];
	char Data[512]="------------------------by 认真的雪------------------------\r\n-------------------http://www.nohack.cn-------------------";
	for(int i=0;i<10000;i++) //
	{
		//得到windows目录
		GetWindowsDirectory(Path,sizeof(Path));
		//用i的值加.txt来给文件命名
		wsprintf(FileName,"\\%d.txt",i);
		//给path赋以完整路径
		strcat(Path,FileName);
		HANDLE hFile;
		//创建文件
		hFile=CreateFile(Path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hFile==INVALID_HANDLE_VALUE)
		{
			continue;
		}
		DWORD dwWrite;
		//把Data中的数据写入文件
		WriteFile(hFile,&Data,strlen(Data),&dwWrite,NULL);
		//关闭文件句柄
		CloseHandle(hFile);
		memset(Path,0x00,255);
		memset(FileName,0x00,255);
	}
	return 0;
}
