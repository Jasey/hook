// httpdown.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")

//�ж��ļ��Ƿ����
BOOL FileExists(LPCTSTR lpszFileName)
{
	//�õ��ļ�����
	DWORD dwAttributes=GetFileAttributes(lpszFileName);
	//�������óɹ����ļ�����
	if(dwAttributes==0xffffffff)
	{
		return false;
	}
	//�����ļ�������
	else
	{
		return true;
	}
}

//http�����ļ�
void download(char *Url,char *FilePath)
{
	//ɾ�������ļ�
	if(!DeleteFile(FilePath))
	{
		printf("�ļ��Ѵ��ڣ������޷�ɾ��\n");
	}
	//�����ļ�
	URLDownloadToFile(0,Url,FilePath,0,0);
	//�ж��ļ��治���ڣ���ȷ�����س������
	if(FileExists(FilePath))
	{
		printf("�ļ����سɹ�\n");
	}
	else
	{
		printf("�ļ�����ʧ��\n");
	}	
}

int main(int argc, char* argv[])
{
	download(argv[1],argv[2]);
	return 0;
}
