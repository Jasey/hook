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


//定义函数原型
typedef int (WINAPI *FRECV)(SOCKET s, char *buf, int len, int flags);
typedef int (WINAPI *FSEND)(SOCKET s, const char *buf, int len, int flags);
typedef int (WINAPI *FWSASEND)(SOCKET s, 
							   LPWSABUF lpBuffers,
							   DWORD dwBufferCount, 
							   LPDWORD lpNumberOfBytesSent,
                               DWORD dwFlags,
                               LPWSAOVERLAPPED lpOverlapped,
                               LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

typedef int (WINAPI *FWSARECV)(SOCKET s,
                               LPWSABUF lpBuffers,
                               DWORD dwBufferCount,
                               LPDWORD lpNumberOfBytesRecvd,
                               LPDWORD lpFlags,
							   LPWSAOVERLAPPED lpOverlapped,
							   LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

//保存旧地址
FRECV       pOldRecv = NULL;
FSEND       pOldSend = NULL;
FWSASEND    pOldWSASend = NULL;   
FWSARECV    pOldWSARecv = NULL;    

CTDS_MSSQL m_tds;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHOOK::CHOOK()
{

}

CHOOK::~CHOOK()
{

}

//写自己的函数 简单的输出操作而已

void DebugPrintf(const char* szPre,const char* buf,int len)
{
  char* szBuf = new char[len*5+20];
  strcpy(szBuf,szPre);
  
  for (int i = 0; i < len;  ++i)
  {
    /* 格式化为16进制 */
    sprintf(szBuf,"%s%02x ",szBuf,*(buf+i));
  }
  OutputDebugString(szBuf);
  delete szBuf;
}

int WINAPI myrecv(SOCKET s,char *buf,int len,int flags)
{
    int ret = pOldRecv(s,buf,len,flags); 
	DebugPrintf("[recv]",buf,len);
	return ret;
}

int WINAPI mysend(SOCKET s,const char *buf,int len,int flags)
{
	u_char tempBuff[65536];
	memset(tempBuff, 0, sizeof(tempBuff));
	memcpy(tempBuff,buf,len);	
	m_tds.TDSDecode(tempBuff,len,1);
	//DebugPrintf("[send]",(const char *)psz,len);
	/*char szBuffer[100];
	sprintf(szBuffer, "123%d",hWnd);
	OutputDebugString((char *)szBuffer);*/
	return pOldSend(s,buf,len,flags);
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

int WINAPI myWSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent,
					DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
  ParseBuf("[WSASend]",lpBuffers,dwBufferCount);
  return pOldWSASend(s,lpBuffers,dwBufferCount,lpNumberOfBytesSent,dwFlags,lpOverlapped,lpCompletionRoutine);
}

int WINAPI myWSARecv(SOCKET s,LPWSABUF lpBuffers,DWORD dwBufferCount,LPDWORD lpNumberOfBytesRecvd,LPDWORD lpFlags,
					LPWSAOVERLAPPED lpOverlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	int ret = pOldWSARecv(s,
	lpBuffers,
	dwBufferCount,
	lpNumberOfBytesRecvd,
	lpFlags,
	lpOverlapped,
	lpCompletionRoutine);
	ParseBuf("[WSARecv]",lpBuffers,dwBufferCount);
	return ret;
}

void Start()
{
	//获取原始函数地址
	HMODULE hModule = GetModuleHandle("Ws2_32.dll");
	//pOldWSARecv = (FWSARECV)GetProcAddress(hModule,"WSARecv");
	//pOldWSASend = (FWSASEND)GetProcAddress(hModule,"WSASend");
	pOldSend = (FSEND)GetProcAddress(hModule,"send");
	//pOldRecv = (FRECV)GetProcAddress(hModule,"recv");
	//assert(pOldWSARecv&&pOldWSASend&&pOldSend&&pOldRecv);

	//开始HOOK
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	//DetourAttach(&(PVOID&)pOldWSARecv, myWSARecv);
	//DetourAttach(&(PVOID&)pOldWSASend, myWSASend);
	DetourAttach(&(PVOID&)pOldSend, mysend);
	//DetourAttach(&(PVOID&)pOldRecv, myrecv);
	DetourTransactionCommit();
}
