// HOOK.cpp: implementation of the CHOOK class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NirvanaSys.h"
#include "HOOK.h"

#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <assert.h>
#include "detours.h"


#pragma comment(lib,"detours.lib")
#pragma comment(lib,"Ws2_32.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



typedef HMODULE (WINAPI *FLoadLibraryA)(LPCTSTR lpFileName);

typedef HRESULT (WINAPI *FSysFreeString)(BSTR bstr); 

typedef HRESULT (WINAPI *Fput_ConnectionString)(BSTR pbstr);


FLoadLibraryA pOldLoadLibraryA = NULL;

FSysFreeString pOldSysFreeString = NULL;

Fput_ConnectionString pOldput_ConnectionString = NULL;


TradePram_t tradePram;
bool unFlag,setUp=true;
char iniPath[260]={0};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHOOK::CHOOK()
{

}

CHOOK::~CHOOK()
{

}


void DebugPrintf(const char* szPre,const char* buf,int len)
{
  char* szBuf = new char[len*5+20];
  strcpy(szBuf,szPre);
  
  for (int i = 0; i < len;  ++i)
  {
    /* 格式化为16进制 */
    sprintf(szBuf,"%s%02x ",szBuf,*(buf+i));
  }
  //OutputDebugString(szBuf);
  delete szBuf;
}





//处理WSA系列函数
void ParseBuf(const char* szPre,LPWSABUF  lpBuffers,DWORD dwBufferCount)
{
	for (int i = 0; i < dwBufferCount; ++i)
	{
		/* code */
		DebugPrintf(szPre,lpBuffers[i].buf,lpBuffers[i].len);
	}
}

HMODULE WINAPI myLoadLibraryA(LPCTSTR lpFileName)
{
	/*if (strstr(lpFileName,"ws2_32"))
	{
		HMODULE hModule = GetModuleHandle("Ws2_32.dll");
		pOldWSARecv = (FWSARECV)GetProcAddress(hModule,"WSARecv");
		pOldWSASend = (FWSASEND)GetProcAddress(hModule,"WSASend");
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourTransactionCommit();
	}*/
	HMODULE ret=pOldLoadLibraryA(lpFileName);
	return ret;
}

CString GetDbData(CString Source, char * Index, char * End)
{
	CString sSubStr;
	int first,last;
	first= Source.Find(Index);
	if (first != -1){
		last= Source.Find(End,first);
	}
	if (first!= -1 && last!= -1){
		int nCount = last-first-strlen(Index);
		sSubStr = Source.Mid(first+strlen(Index),nCount);
	}
	return sSubStr;
}

HRESULT WINAPI mySysFreeString(BSTR bstr)
{	
	CString ConStr(bstr);
	CString str_dbuser,str_dbpass,str_dbserver,str_dbname;
	HKEY hKey; 
    LPCTSTR lpRun = "Software\\PosBirdge";
    DWORD state,dwtype,sizeBuff;
	long lRet;
	ConStr+=";";
	if(strstr(ConStr,"Provider=SQLOLEDB")){
		str_dbpass=GetDbData(ConStr,"Password=",";");
		str_dbuser=GetDbData(ConStr,"User ID=",";");
		str_dbserver=GetDbData(ConStr,"Data Source=",";");
		str_dbname=GetDbData(ConStr,"Initial Catalog=",";");
		//创建键
		lRet = RegCreateKeyEx(HKEY_CURRENT_USER,lpRun,0,NULL,0,0,NULL,&hKey,&state);
		if(lRet == ERROR_SUCCESS){
			if(state == REG_CREATED_NEW_KEY){
			}
			//关闭键
			RegCloseKey(hKey);
		}
		lRet= RegOpenKeyEx(HKEY_CURRENT_USER, lpRun, 0, KEY_WRITE, &hKey);
		if(lRet == ERROR_SUCCESS) {     
			//创建或者修改键值
			RegSetValueEx(hKey, "user",0,REG_SZ,(const unsigned char *)(LPCTSTR)str_dbuser,str_dbuser.GetLength());
			RegSetValueEx(hKey, "pw",0,REG_SZ,(const unsigned char *)(LPCTSTR)str_dbpass,str_dbpass.GetLength());
			RegSetValueEx(hKey, "dbname",0,REG_SZ,(const unsigned char *)(LPCTSTR)str_dbname,str_dbname.GetLength());
			RegSetValueEx(hKey, "server",0,REG_SZ,(const unsigned char *)(LPCTSTR)str_dbserver,str_dbserver.GetLength());
			//关闭键    
			RegCloseKey(hKey);
			setUp=false;
		}
	}
	HRESULT ret=pOldSysFreeString(bstr);
	return ret;
}

HRESULT WINAPI myput_ConnectionString(BSTR bstr)
{	
	CString ConStr(bstr);
	CString str_dbuser,str_dbpass,str_dbserver,str_dbname;
	HKEY hKey; 
    LPCTSTR lpRun = "Software\\PosBridge\\put_connect";
    DWORD state,dwtype,sizeBuff;
	long lRet;
	ConStr+=";";
	if(strstr(ConStr,"Provider=SQLOLEDB")){
		str_dbpass=GetDbData(ConStr,"Password=",";");
		str_dbuser=GetDbData(ConStr,"User ID=",";");
		str_dbserver=GetDbData(ConStr,"Data Source=",";");
		str_dbname=GetDbData(ConStr,"Initial Catalog=",";");
		//创建键
		lRet = RegCreateKeyEx(HKEY_CURRENT_USER,lpRun,0,NULL,0,0,NULL,&hKey,&state);
		if(lRet == ERROR_SUCCESS){
			if(state == REG_CREATED_NEW_KEY){
			}
			//关闭键
			RegCloseKey(hKey);
		}
		lRet= RegOpenKeyEx(HKEY_CURRENT_USER, lpRun, 0, KEY_WRITE, &hKey);
		if(lRet == ERROR_SUCCESS) {     
			//创建或者修改键值
			RegSetValueEx(hKey, "user",0,REG_SZ,(const unsigned char *)(LPCTSTR)str_dbuser,str_dbuser.GetLength());
			RegSetValueEx(hKey, "pw",0,REG_SZ,(const unsigned char *)(LPCTSTR)str_dbpass,str_dbpass.GetLength());
			RegSetValueEx(hKey, "dbname",0,REG_SZ,(const unsigned char *)(LPCTSTR)str_dbname,str_dbname.GetLength());
			RegSetValueEx(hKey, "server",0,REG_SZ,(const unsigned char *)(LPCTSTR)str_dbserver,str_dbserver.GetLength());
			//关闭键    
			RegCloseKey(hKey);
			setUp=false;
		}
	}
	HRESULT ret=pOldput_ConnectionString(bstr);
	return ret;
}

void Start()
{
	HMODULE hModuleKernel = GetModuleHandle("Kernel32.dll");
	pOldLoadLibraryA=(FLoadLibraryA)GetProcAddress(hModuleKernel,"LoadLibraryA");
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)pOldLoadLibraryA, myLoadLibraryA);
	DetourTransactionCommit();

	HMODULE hModuleCom = GetModuleHandle("OleAut32.dll");
	pOldSysFreeString=(FSysFreeString)GetProcAddress(hModuleCom,"SysFreeString");
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)pOldSysFreeString, mySysFreeString);
	DetourTransactionCommit();
}
