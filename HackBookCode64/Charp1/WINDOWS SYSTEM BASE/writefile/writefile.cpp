// writefile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	//调用CreateFile函数以只写方式打开一个文件
	HANDLE hFile=CreateFile(argv[1],GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
	{
		printf("CreateFile error\n");
		return 0;
	}
	//调用SetFilePointer函数调整文件指针位置，移动到文件末尾
	if(SetFilePointer(hFile,0,NULL,FILE_END)==-1)
	{
		printf("SetFilePointer error \n");	
	    return 0;	
	}
	char buff[256]="配置信息";
	DWORD dwWrite;
	//把buff中的内容写入到文件末尾
	if(!WriteFile(hFile,&buff,strlen(buff),&dwWrite,NULL))
	{
		printf("WriteFile error \n");	
	    return 0;	
	}
	printf("往%s中写入数据成功\n",argv[1]);
	CloseHandle(hFile);
	return 0;
}
