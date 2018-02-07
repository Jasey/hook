// ToOneFile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
int main(int argc, char* argv[])
{
	//ֻ����ʽ��DLL�ļ�
	HANDLE nhFile=CreateFile(argv[1],GENERIC_READ,FILE_SHARE_READ,NULL,
		                 OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(nhFile==INVALID_HANDLE_VALUE)
	{
		printf("CreateFile error\n");
		return 0;
	}
	//ֻд��ʽ��EXE�ļ�
	HANDLE shFile=CreateFile(argv[2],GENERIC_WRITE,FILE_SHARE_READ,NULL,
		                  OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(shFile==INVALID_HANDLE_VALUE)
	{
		printf("CreateFile error\n");
		return 0;
	}
	//��EXE�ļ���ָ���ƶ����ļ�ĩβ
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
		//ÿ�ζ�ȡ2800�ֽ�
		if(!ReadFile(nhFile,buff,2800,&dwRead,NULL))
		{
			printf("ReadFile error\n");
			return 0;	
		}
		//������ʵ��ֵС��2800�ֽ�
		if(dwRead<2800)
		{
			//д��ʵ�ʶ������ֽ���
			if(!WriteFile(shFile,buff,dwRead,&dwWrite,NULL))
			{
				printf("WriteFile error\n");
				return 0;	
			}
			//����ѭ��
			break;
		}
		//д�������2800�ֽ�
		if(!WriteFile(shFile,buff,2800,&dwWrite,NULL))
		{
			printf("WriteFile error\n");
			return 0;
		}
	}
	printf("�ϲ��ļ��ɹ�\n");
	//�ر��ļ����
	CloseHandle(shFile);
	CloseHandle(nhFile);
	return 0;
}
