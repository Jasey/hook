// SetHook.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

typedef BOOL (*StartHook)();
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{	
		//װ��DLL�ļ�
	HMODULE hModule=LoadLibrary("ApiHook.dll");
	if(hModule==NULL)
	{
		return 0;
	}
	//�õ�StartHook��ַ
	StartHook NewStartHook=(StartHook)GetProcAddress(hModule,"StartHook");
	if(NewStartHook==NULL)
	{
		return 0;
	}
	BOOL bRet=NewStartHook();
	if (bRet)
	{
		MessageBox(NULL,"���Ӱ�װ�ɹ�","SetHook",0);
	}
	else
	{
		MessageBox(NULL,"���Ӱ�װʧ��","SetHook",0);
		return 0;
	}
	for(;;)
	{
		Sleep(10000);
	}

	
	return 0;
}





