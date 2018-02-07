// httpdown.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")

//判断文件是否存在
BOOL FileExists(LPCTSTR lpszFileName)
{
	//得到文件属性
	DWORD dwAttributes=GetFileAttributes(lpszFileName);
	//函数调用成功则文件存在
	if(dwAttributes==0xffffffff)
	{
		return false;
	}
	//否则文件不存在
	else
	{
		return true;
	}
}

//http下载文件
void download(char *Url,char *FilePath)
{
	//删除已有文件
	if(!DeleteFile(FilePath))
	{
		printf("文件已存在，并且无法删除\n");
	}
	//下载文件
	URLDownloadToFile(0,Url,FilePath,0,0);
	//判断文件存不存在，以确定下载成有与否
	if(FileExists(FilePath))
	{
		printf("文件下载成功\n");
	}
	else
	{
		printf("文件下载失败\n");
	}	
}

int main(int argc, char* argv[])
{
	download(argv[1],argv[2]);
	return 0;
}
