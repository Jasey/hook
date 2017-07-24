// hookexe.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

DWORD dwOffect,dwArgu;

BOOL CreateRemoteDll(const char *DllFullPath, const DWORD dwRemoteProcessId)
{
	HANDLE hToken;
	if ( OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken) )
	{
		TOKEN_PRIVILEGES tkp;

		LookupPrivilegeValue( NULL,SE_DEBUG_NAME,&tkp.Privileges[0].Luid );//修改进程权限
		tkp.PrivilegeCount=1;
		tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges( hToken,FALSE,&tkp,sizeof tkp,NULL,NULL );//通知系统修改进程权限
		CloseHandle(hToken);
	}

	HANDLE hRemoteProcess;

	//打开远程线程
	if( (hRemoteProcess = OpenProcess( PROCESS_CREATE_THREAD |    //允许远程创建线程
		PROCESS_VM_OPERATION |                //允许远程VM操作
		PROCESS_VM_WRITE|                    //允许远程VM写
		PROCESS_ALL_ACCESS,
		FALSE, dwRemoteProcessId ) )== NULL )
	{
		printf("OpenProcess Error!");
		return FALSE;
	}

	char *pszLibFileRemote;
	//在远程进程的内存地址空间分配DLL文件名缓冲区
	pszLibFileRemote = (char *) VirtualAllocEx( hRemoteProcess, NULL, lstrlen(DllFullPath)+1, 
		MEM_COMMIT, PAGE_READWRITE);
	if(pszLibFileRemote == NULL)
	{
		printf("VirtualAllocEx error! ");
		CloseHandle(hRemoteProcess);
		return FALSE;
	}

	//将DLL的路径名复制到远程进程的内存空间
	if( WriteProcessMemory(hRemoteProcess,
		pszLibFileRemote, (void *) DllFullPath, lstrlen(DllFullPath)+1, NULL) == 0)
	{
		printf("WriteProcessMemory Error");
		CloseHandle(hRemoteProcess);
		return FALSE;
	}

	//计算LoadLibraryA的入口地址
	PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)
		GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryA");

	if(pfnStartAddr == NULL)
	{
		printf("GetProcAddress Error");
		return FALSE;
	}

	HANDLE hRemoteThread;

	//远程调用loadLibraryA调用dll
	hRemoteThread = CreateRemoteThread( hRemoteProcess, NULL, 0, 
		pfnStartAddr, pszLibFileRemote, 0, NULL);
	WaitForSingleObject(hRemoteThread,INFINITE);
	if( hRemoteThread == NULL)
	{
		printf("CreateRemoteThread Error");
		CloseHandle(hRemoteProcess);
		return FALSE;
	}
	CloseHandle(hRemoteProcess);
	CloseHandle(hRemoteThread);
	return TRUE;
}



void Hook(int dwPid, char* curpath)
{ 

	if(CreateRemoteDll(curpath,dwPid))
		printf("启动成功\n");
	else
		printf("启动失败\n");

}

int main(int argc, char* argv[])
{
	int pid;
	printf("输入进程pid\n");
	scanf("%d",&pid);
	char curpath[260];
	printf("输入dll路径:\n");
	scanf("%s",&curpath);
	Hook(pid, curpath);
	getchar();
	return 0;
}

