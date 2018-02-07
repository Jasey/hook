// Export.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	switch(ul_reason_for_call)
	{
		//DLL被加载到内存时
	case DLL_PROCESS_ATTACH:
		{
			break;
		}
		//进程中有线程创建时
	case DLL_THREAD_ATTACH:
		{
			//........其他功能代码
			break;
		}
		//线程正常退出时
	case DLL_THREAD_DETACH:
		{
			//........其他功能代码
			break;
		}
		//动态链接库被卸载时
	case DLL_PROCESS_DETACH:
		{
			//........其他功能代码
			break;
		}
	default:break;
	}
	return TRUE;
}

int MyMessageBox(char *lpText,char *lpCaption)
{
	MessageBox(NULL,lpText,lpCaption,0);
	return 0;
}
