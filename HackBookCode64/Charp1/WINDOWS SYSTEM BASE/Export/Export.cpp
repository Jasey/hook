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
		//DLL�����ص��ڴ�ʱ
	case DLL_PROCESS_ATTACH:
		{
			break;
		}
		//���������̴߳���ʱ
	case DLL_THREAD_ATTACH:
		{
			//........�������ܴ���
			break;
		}
		//�߳������˳�ʱ
	case DLL_THREAD_DETACH:
		{
			//........�������ܴ���
			break;
		}
		//��̬���ӿⱻж��ʱ
	case DLL_PROCESS_DETACH:
		{
			//........�������ܴ���
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
