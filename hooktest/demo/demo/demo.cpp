#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <atlbase.h>
#include <conio.h>
#include <stdlib.h>

HANDLE h_token;
HANDLE h_remote_process;
TCHAR msg[MAX_PATH];
BOOL inject_dll( LPCTSTR dll_path, const DWORD remote_pro_id )
{

	if ( OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &h_token ) )
	{
		TOKEN_PRIVILEGES tkp;
		//修改进程权限
		LookupPrivilegeValue( NULL,SE_DEBUG_NAME, &tkp.Privileges[0].Luid );
		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		//通知系统修改进程权限
		AdjustTokenPrivileges( h_token, FALSE, &tkp, sizeof( tkp ), NULL, NULL );
	}

	//打开远程线程
	if( ( h_remote_process = OpenProcess( PROCESS_CREATE_THREAD | //允许远程创建线程
		PROCESS_VM_OPERATION | //允许远程VM操作
		PROCESS_VM_WRITE, //考试#大提示允许远程VM写
		FALSE, remote_pro_id ) )== NULL )
	{
		USES_CONVERSION;
		_tcscpy(msg, A2T("OpenProcess Error!"));
		MessageBox(NULL,msg,msg,MB_OK);
		return FALSE;
	}
	char *lib_func_buf;
	//在远程进程的内存地址空间分配DLL文件名缓冲区
	lib_func_buf = (char *) VirtualAllocEx( h_remote_process, NULL, lstrlen(dll_path) + 1,
		MEM_COMMIT, PAGE_READWRITE);
	if( lib_func_buf == NULL )
	{
		USES_CONVERSION;
		_tcscpy(msg, A2T("VirtualAllocEx Error!"));
		MessageBox(NULL,msg,msg,MB_OK);
		return FALSE;
	}
	//将DLL的路径名复制到远程进程的内存空间
	if( WriteProcessMemory( h_remote_process,
		lib_func_buf, ( void * )dll_path, lstrlen( dll_path ) + 1, NULL ) == 0 )
	{
		USES_CONVERSION;
		_tcscpy(msg, A2T("WriteProcessMemory Error!"));
		MessageBox(NULL,msg,msg,MB_OK);
		return FALSE;
	}
	//计算LoadLibraryA的入口地址
	PTHREAD_START_ROUTINE load_start_addr = ( PTHREAD_START_ROUTINE )
		GetProcAddress( GetModuleHandle( TEXT("Kernel32") ), "LoadLibraryA");
	if( load_start_addr == NULL )
	{
		USES_CONVERSION;
		_tcscpy(msg, A2T("GetProcAddress Error!"));
		MessageBox(NULL,msg,msg,MB_OK);
		return FALSE;
	}
	HANDLE h_remote_thread;
	if( (h_remote_thread = CreateRemoteThread( h_remote_process, NULL, 0,
		load_start_addr, lib_func_buf, 0, NULL ) ) == NULL)
	{
		USES_CONVERSION;
		_tcscpy(msg, A2T("CreateRemoteThread Error!"));
		MessageBox(NULL,msg,msg,MB_OK);
		return FALSE;
	}
	return TRUE; 
} 

int main(int argc, char **argv)
{

	TCHAR dll[MAX_PATH];
	int id = 1100;
	USES_CONVERSION;
	_tcscpy(dll, A2T("D:\\myfile\\git-hub\\APIHOOK\\demo\\Debug\\mydll.dll"));
	if(inject_dll(dll,id)){
		USES_CONVERSION;
		_tcscpy(msg, A2T("SecussFull!"));
		MessageBox(NULL,msg,msg,MB_OK);
	}else{
		USES_CONVERSION;
		_tcscpy(msg, A2T("Error!"));
		MessageBox(NULL,msg,msg,MB_OK);
	}
	getch();
	return 0;
}