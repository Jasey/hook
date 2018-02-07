#include <stdio.h>
#include <Windows.h>


int ApiHook(char *DllName,PROC OldFunAddr,PROC NewFunAddr);
DWORD* lpAddr;
PROC OldProc;
char LogMessage[255];

int WINAPI  __stdcall  MyMessageBox(HWND hwnd,LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	printf("�㿴�������������� \n");
	return 0;
}

int main(){

	OldProc = (PROC)MessageBox;
	printf("begin hook MessageBox as MyMessageBox\n");
	ApiHook("USER32.dll", OldProc, (PROC) MyMessageBox);
	printf("after hook api, call MessageBox \n");
	MessageBox(NULL,"��Ҫ��ʾ����","API HOOK",0);

	getchar();
	return 0;
}


int  ApiHook(char *DllName,//DLL�ļ���
			  PROC OldFunAddr,//ҪHOOK�ĺ�����ַ
			  PROC NewFunAddr//���ǹ���ĺ�����ַ
			  )
{
	//�õ���������ģ�����ַ
	HMODULE lpBase = GetModuleHandle(NULL);
	IMAGE_DOS_HEADER *dosHeader;
	IMAGE_NT_HEADERS *ntHeader;
	//��λ��DOSͷ
	dosHeader=(IMAGE_DOS_HEADER*)lpBase;
	//��λ��PEͷ
	ntHeader=(IMAGE_NT_HEADERS32*)((BYTE*)lpBase+dosHeader->e_lfanew);
	//��λ�������
	IMAGE_IMPORT_DESCRIPTOR *pImportDesc=(IMAGE_IMPORT_DESCRIPTOR*)((BYTE*)lpBase+ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	//ѭ������IMAGE_IMPORT_DESCRIPTOR��������

	wsprintf(LogMessage, "MessageBox function' DLL Name : %s\n", DllName);
	OutputDebugString (LogMessage);

	wsprintf(LogMessage, "MessageBox address : 0x%02X\n", (DWORD)OldFunAddr);
	OutputDebugString (LogMessage);

	wsprintf(LogMessage, "MyMessageBox address : 0x%02X\n", (DWORD)NewFunAddr);
	OutputDebugString (LogMessage);

	while(pImportDesc->FirstThunk)
	{
		//�õ�DLL�ļ���
		char* pszDllName = (char*)((BYTE*)lpBase + pImportDesc->Name);
		//�Ƚϵõ���DLL�ļ����Ƿ��ҪHOOK�������ڵ�DLL��ͬ
		if(lstrcmpiA(pszDllName, DllName) == 0)
		{
			wsprintf(LogMessage, "Find The %s's Descriptor\n", DllName);
			OutputDebugString (LogMessage);
			break;
		}
		pImportDesc++;
	}
	//��λ��FirstThunk����ָ���IMAGE_THUNK_DATA����ʱ����ṹ�Ѿ��Ǻ�����ڵ��ַ��
	IMAGE_THUNK_DATA* pThunk = (IMAGE_THUNK_DATA*)
		((BYTE*)lpBase + pImportDesc->FirstThunk);
	//�����ⲿ��IAT��
	while(pThunk->u1.Function)
	{
		lpAddr = (DWORD*)&(pThunk->u1.Function);
		//�ȽϺ�����ַ�Ƿ���ͬ
		if(*lpAddr == (DWORD)OldFunAddr)
		{	
			wsprintf(LogMessage, "Find the MessageBox function enter address : 0x%02X, it's content : 0x%02X\n", lpAddr, *lpAddr);
			OutputDebugString (LogMessage);
			DWORD dwOldProtect;
			//�޸��ڴ��������
			VirtualProtect(lpAddr, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect);
			//API��������ڵ��ַ�ĳ����ǹ���ĺ����ĵ�ַ
			WriteProcessMemory(GetCurrentProcess(),lpAddr, &NewFunAddr, sizeof(DWORD), NULL);
			wsprintf(LogMessage, "after hook function enter address : 0x%02X, it's content : 0x%02X\n", lpAddr, *lpAddr);
			OutputDebugString (LogMessage);
		}
		pThunk++;
	}
	return 0;
}