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
		//�޸Ľ���Ȩ��
		LookupPrivilegeValue( NULL,SE_DEBUG_NAME, &tkp.Privileges[0].Luid );
		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		//֪ͨϵͳ�޸Ľ���Ȩ��
		AdjustTokenPrivileges( h_token, FALSE, &tkp, sizeof( tkp ), NULL, NULL );
	}

	//��Զ���߳�
	if( ( h_remote_process = OpenProcess( PROCESS_CREATE_THREAD | //����Զ�̴����߳�
		PROCESS_VM_OPERATION | //����Զ��VM����
		PROCESS_VM_WRITE, //����#����ʾ����Զ��VMд
		FALSE, remote_pro_id ) )== NULL )
	{
		USES_CONVERSION;
		_tcscpy(msg, A2T("OpenProcess Error!"));
		MessageBox(NULL,msg,msg,MB_OK);
		return FALSE;
	}
	char *lib_func_buf;
	//��Զ�̽��̵��ڴ��ַ�ռ����DLL�ļ���������
	lib_func_buf = (char *) VirtualAllocEx( h_remote_process, NULL, lstrlen(dll_path) + 1,
		MEM_COMMIT, PAGE_READWRITE);
	if( lib_func_buf == NULL )
	{
		USES_CONVERSION;
		_tcscpy(msg, A2T("VirtualAllocEx Error!"));
		MessageBox(NULL,msg,msg,MB_OK);
		return FALSE;
	}
	//��DLL��·�������Ƶ�Զ�̽��̵��ڴ�ռ�
	if( WriteProcessMemory( h_remote_process,
		lib_func_buf, ( void * )dll_path, lstrlen( dll_path ) + 1, NULL ) == 0 )
	{
		USES_CONVERSION;
		_tcscpy(msg, A2T("WriteProcessMemory Error!"));
		MessageBox(NULL,msg,msg,MB_OK);
		return FALSE;
	}
	//����LoadLibraryA����ڵ�ַ
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