// improt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
//定义函数原型
typedef int(*MyMessageBox)(char *lpText,char *lpCaption);
int main(int argc, char* argv[])
{
	//装载DLL文件
	HMODULE hModule=LoadLibrary("Export.dll");
	if(hModule==NULL)
	{
		printf("LoadLibrary error \n");
		return 0;
	}
	//得到MyMessageBox地址
	MyMessageBox NewMessageBox=(MyMessageBox)GetProcAddress(hModule,"MyMessageBox");
	if(NewMessageBox==NULL)
	{
		printf("GetProcAddress error \n");
		return 0;
	}
	//调用导出函数
	NewMessageBox("by  认真的雪","http://www.nohack.cn");
	//释放DLL类库
	FreeLibrary(hModule);

	return 0;
}
