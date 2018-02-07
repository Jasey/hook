#include <stdio.h>
#include <Windows.h>


int ApiHook(char *DllName,PROC OldFunAddr,PROC NewFunAddr);
DWORD* lpAddr;
PROC OldProc;
char LogMessage[255];

int WINAPI  __stdcall  MyMessageBox(HWND hwnd,LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	printf("你看，我在命令行了 \n");
	return 0;
}

int main(){

	OldProc = (PROC)MessageBox;
	printf("begin hook MessageBox as MyMessageBox\n");
	ApiHook("USER32.dll", OldProc, (PROC) MyMessageBox);
	printf("after hook api, call MessageBox \n");
	MessageBox(NULL,"我要显示窗口","API HOOK",0);

	getchar();
	return 0;
}


int  ApiHook(char *DllName,//DLL文件名
			  PROC OldFunAddr,//要HOOK的函数地址
			  PROC NewFunAddr//我们够造的函数地址
			  )
{
	//得到函数进程模块基地址
	HMODULE lpBase = GetModuleHandle(NULL);
	IMAGE_DOS_HEADER *dosHeader;
	IMAGE_NT_HEADERS *ntHeader;
	//定位到DOS头
	dosHeader=(IMAGE_DOS_HEADER*)lpBase;
	//定位到PE头
	ntHeader=(IMAGE_NT_HEADERS32*)((BYTE*)lpBase+dosHeader->e_lfanew);
	//定位到导入表
	IMAGE_IMPORT_DESCRIPTOR *pImportDesc=(IMAGE_IMPORT_DESCRIPTOR*)((BYTE*)lpBase+ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	//循环遍历IMAGE_IMPORT_DESCRIPTOR机构数组

	wsprintf(LogMessage, "MessageBox function' DLL Name : %s\n", DllName);
	OutputDebugString (LogMessage);

	wsprintf(LogMessage, "MessageBox address : 0x%02X\n", (DWORD)OldFunAddr);
	OutputDebugString (LogMessage);

	wsprintf(LogMessage, "MyMessageBox address : 0x%02X\n", (DWORD)NewFunAddr);
	OutputDebugString (LogMessage);

	while(pImportDesc->FirstThunk)
	{
		//得到DLL文件名
		char* pszDllName = (char*)((BYTE*)lpBase + pImportDesc->Name);
		//比较得到的DLL文件名是否和要HOOK函数所在的DLL相同
		if(lstrcmpiA(pszDllName, DllName) == 0)
		{
			wsprintf(LogMessage, "Find The %s's Descriptor\n", DllName);
			OutputDebugString (LogMessage);
			break;
		}
		pImportDesc++;
	}
	//定位到FirstThunk参数指向的IMAGE_THUNK_DATA，此时这个结构已经是函数入口点地址了
	IMAGE_THUNK_DATA* pThunk = (IMAGE_THUNK_DATA*)
		((BYTE*)lpBase + pImportDesc->FirstThunk);
	//遍历这部分IAT表
	while(pThunk->u1.Function)
	{
		lpAddr = (DWORD*)&(pThunk->u1.Function);
		//比较函数地址是否相同
		if(*lpAddr == (DWORD)OldFunAddr)
		{	
			wsprintf(LogMessage, "Find the MessageBox function enter address : 0x%02X, it's content : 0x%02X\n", lpAddr, *lpAddr);
			OutputDebugString (LogMessage);
			DWORD dwOldProtect;
			//修改内存包含属性
			VirtualProtect(lpAddr, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect);
			//API函数的入口点地址改成我们构造的函数的地址
			WriteProcessMemory(GetCurrentProcess(),lpAddr, &NewFunAddr, sizeof(DWORD), NULL);
			wsprintf(LogMessage, "after hook function enter address : 0x%02X, it's content : 0x%02X\n", lpAddr, *lpAddr);
			OutputDebugString (LogMessage);
		}
		pThunk++;
	}
	return 0;
}