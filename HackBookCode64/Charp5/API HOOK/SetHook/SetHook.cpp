// SetHook.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

typedef BOOL (*StartHook)();
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{	
		//装载DLL文件
	HMODULE hModule=LoadLibrary("ApiHook.dll");
	if(hModule==NULL)
	{
		return 0;
	}
	//得到StartHook地址
	StartHook NewStartHook=(StartHook)GetProcAddress(hModule,"StartHook");
	if(NewStartHook==NULL)
	{
		return 0;
	}
	BOOL bRet=NewStartHook();
	if (bRet)
	{
		MessageBox(NULL,"钩子安装成功","SetHook",0);
	}
	else
	{
		MessageBox(NULL,"钩子安装失败","SetHook",0);
		return 0;
	}
	for(;;)
	{
		Sleep(10000);
	}

	
	return 0;
}





