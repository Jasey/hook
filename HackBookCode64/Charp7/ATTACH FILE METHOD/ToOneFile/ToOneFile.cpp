// ToOneFile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
int main(int argc, char* argv[])
{
	//只读方式打开DLL文件
	HANDLE nhFile=CreateFile(argv[1],GENERIC_READ,FILE_SHARE_READ,NULL,
		                 OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(nhFile==INVALID_HANDLE_VALUE)
	{
		printf("CreateFile error\n");
		return 0;
	}
	//只写方式打开EXE文件
	HANDLE shFile=CreateFile(argv[2],GENERIC_WRITE,FILE_SHARE_READ,NULL,
		                  OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(shFile==INVALID_HANDLE_VALUE)
	{
		printf("CreateFile error\n");
		return 0;
	}
	//把EXE文件的指针移动到文件末尾
    if(SetFilePointer(shFile,0,NULL,FILE_END)==-1)
	{
		printf("SetFilePointer error\n");
		return 0;
	}
	char buff[2800]={0};
	DWORD dwRead;
	DWORD dwWrite;
	while(1)
	{
		//每次读取2800字节
		if(!ReadFile(nhFile,buff,2800,&dwRead,NULL))
		{
			printf("ReadFile error\n");
			return 0;	
		}
		//读到的实际值小于2800字节
		if(dwRead<2800)
		{
			//写入实际读到的字节数
			if(!WriteFile(shFile,buff,dwRead,&dwWrite,NULL))
			{
				printf("WriteFile error\n");
				return 0;	
			}
			//跳出循环
			break;
		}
		//写入读到的2800字节
		if(!WriteFile(shFile,buff,2800,&dwWrite,NULL))
		{
			printf("WriteFile error\n");
			return 0;
		}
	}
	printf("合并文件成功\n");
	//关闭文件句柄
	CloseHandle(shFile);
	CloseHandle(nhFile);
	return 0;
}
