// writefile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	//����CreateFile������ֻд��ʽ��һ���ļ�
	HANDLE hFile=CreateFile(argv[1],GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
	{
		printf("CreateFile error\n");
		return 0;
	}
	//����SetFilePointer���������ļ�ָ��λ�ã��ƶ����ļ�ĩβ
	if(SetFilePointer(hFile,0,NULL,FILE_END)==-1)
	{
		printf("SetFilePointer error \n");	
	    return 0;	
	}
	char buff[256]="������Ϣ";
	DWORD dwWrite;
	//��buff�е�����д�뵽�ļ�ĩβ
	if(!WriteFile(hFile,&buff,strlen(buff),&dwWrite,NULL))
	{
		printf("WriteFile error \n");	
	    return 0;	
	}
	printf("��%s��д�����ݳɹ�\n",argv[1]);
	CloseHandle(hFile);
	return 0;
}
