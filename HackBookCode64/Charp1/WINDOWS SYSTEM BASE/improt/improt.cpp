// improt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
//���庯��ԭ��
typedef int(*MyMessageBox)(char *lpText,char *lpCaption);
int main(int argc, char* argv[])
{
	//װ��DLL�ļ�
	HMODULE hModule=LoadLibrary("Export.dll");
	if(hModule==NULL)
	{
		printf("LoadLibrary error \n");
		return 0;
	}
	//�õ�MyMessageBox��ַ
	MyMessageBox NewMessageBox=(MyMessageBox)GetProcAddress(hModule,"MyMessageBox");
	if(NewMessageBox==NULL)
	{
		printf("GetProcAddress error \n");
		return 0;
	}
	//���õ�������
	NewMessageBox("by  �����ѩ","http://www.nohack.cn");
	//�ͷ�DLL���
	FreeLibrary(hModule);

	return 0;
}
