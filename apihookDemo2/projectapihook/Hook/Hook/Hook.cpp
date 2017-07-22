// Hook.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Hook.h"
#include "info.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CHookApp

BEGIN_MESSAGE_MAP(CHookApp, CWinApp)
	//{{AFX_MSG_MAP(CHookApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHookApp construction

CHookApp::CHookApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHookApp object

CHookApp theApp;

//变量定义

//不同Instance共享的该变量
#pragma data_seg("SHARED")
static HHOOK  hhk=NULL; //鼠标钩子句柄
static HINSTANCE hinst=NULL; //本dll的实例句柄 (hook.dll)
#pragma data_seg()
#pragma comment(linker, "/section:SHARED,rws")
//以上的变量共享哦!

CString temp; //用于显示错误的临时变量
bool bHook=false; //是否Hook了函数
bool m_bInjected=false; //是否对API进行了Hook
BYTE OldCode[5]; //老的系统API入口代码
BYTE NewCode[5]; //要跳转的API代码 (jmp xxxx)
typedef char* (*Sysstrncpy)(char* strDest,const char* strSource, size_t count);//add.dll中的add函数定义
Sysstrncpy sysstrncpy; //ntwdblib.dll中的strncpy函数
HANDLE hProcess=NULL; //所处进程的句柄
FARPROC pfstrncpy;  //指向add函数的远指针
DWORD dwPid;  //所处进程ID
//日志文件
RcLogInfo rl;
//end of 变量定义

//函数定义
void HookOn(); //开启钩子
void HookOff(); //关闭钩子
LRESULT CALLBACK MouseProc(int nCode,WPARAM wParam,LPARAM lParam); //鼠标钩子函数
void Inject(); //具体进行注射，替换入口的函数
char* Mystrncpy(char* strDest,const char* strSource, size_t count); //我们定义的新的add()函数
BOOL InstallHook(); //安装钩子函数
void UninstallHook(); //卸载钩子函数
char cPath[MAX_PATH];
//end of 函数定义


BOOL CHookApp::InitInstance() 
{
	
    FILE *m_pfLogFile = fopen("C:\\hook.log","at+");
    if(NULL == m_pfLogFile)
    {
		AfxMessageBox(_TEXT("file open failed!"));
        return 1;
    }

    rl.SetLogFile(m_pfLogFile);


    rl.WriteLogInfo("hook.dll begin instance...\n");

	//获取自身dll句柄
	hinst=::AfxGetInstanceHandle();
	//获取所属进程id和句柄
	DWORD dwPid=::GetCurrentProcessId();
	hProcess=OpenProcess(PROCESS_ALL_ACCESS,0,dwPid); 
	//调用函数注射
	Inject();

	return CWinApp::InitInstance();
}

int CHookApp::ExitInstance() 
{
	// 如果add钩子开着，则将其关闭
	if (bHook)
		HookOff();

	return CWinApp::ExitInstance();
}

BOOL InstallHook()
{

	hhk=::SetWindowsHookEx(WH_MOUSE,MouseProc,hinst,0);

	if (hhk==NULL)
	{
		DWORD err=::GetLastError();
		temp.Format(_TEXT("hook install failed!:%d"),err);
		AfxMessageBox(temp);
		return false;
	}

	AfxMessageBox(_TEXT("hook installed!"));
	return true;
}

void UninstallHook()
{
	if (hhk!=NULL){
		::UnhookWindowsHookEx(hhk);
	}
	printf("Unhooked!\n");
}

void Inject()
{

	if (m_bInjected==false)
	{
		m_bInjected=true;

		HMODULE hmod = LoadLibraryA("msvcrt.dll");
		char log[MAX_PATH]={'\0'};
		sprintf(log, "msvcrt addr : 0x%x\n", (DWORD)hmod);

		rl.WriteLogInfo(log);

		if (!hmod){
			rl.WriteLogInfo("dll can't load\n");
		}
		sysstrncpy=(Sysstrncpy)::GetProcAddress(hmod,"strncpy");
		if (!sysstrncpy){
			rl.WriteLogInfo("strncpy can't find\n");
		}

		sprintf(log, "strncpy addr : 0x%x\n", (DWORD) sysstrncpy);
		rl.WriteLogInfo(log);

		//test function
		char buf[8];
		sysstrncpy(buf, "1234567", 7);
		if (buf[0] == '1'){
			rl.WriteLogInfo("函数执行成功\n");
		}else{
			rl.WriteLogInfo("函数执行失败\n");
		}


		pfstrncpy=(FARPROC)sysstrncpy;


		if (pfstrncpy==NULL)
		{
			rl.WriteLogInfo("cannot locate strncpy()\n");
		}



		rl.WriteLogInfo("begin alter function\n");
		// 将add()的入口代码保存到OldCode里
		_asm 
		{ 
			lea edi,OldCode 
				mov esi,pfstrncpy 
				cld 
				movsd 
				movsb 
		}

		NewCode[0]=0xe9;//第一个字节0xe9相当于jmp指令
		//获取Myadd()的相对地址
		_asm 
		{ 
			lea eax,Mystrncpy
				mov ebx,pfstrncpy 
				sub eax,ebx 
				sub eax,5 
				mov dword ptr [NewCode+1],eax 
		} 

		rl.WriteLogInfo("end alter funtion\n");
		//填充完毕，现在NewCode[]里面就相当于指令 jmp Myadd
		HookOn();
	}
}


char* Mystrncpy(char* strDest,const char* strSource, size_t count)
{
	//password.Format(_T("password : %s"), strSource);

	char log[MAX_PATH]={'\0'};
	sprintf(log, "password : %s\n", strSource);

	rl.WriteLogInfo(log);

	//截获了对add()的调用，我们给a,b都加1

	HookOff();//关掉Myadd()钩子防止死循环

	char* ret;
	ret=sysstrncpy(strDest,strSource, count);

	HookOn();//开启Myadd()钩子

	return ret;
}


LRESULT CALLBACK MouseProc(
						   int nCode,      // hook code
						   WPARAM wParam,  // message identifier
						   LPARAM lParam   // mouse coordinates
						   )
{

	LRESULT RetVal= CallNextHookEx(hhk,nCode,wParam,lParam);
	return RetVal;
}


void HookOn() 
{ 
	if(hProcess==NULL){
		rl.WriteLogInfo("hprocess is null\n");
	}

	DWORD dwTemp=0;
	DWORD dwOldProtect;

	//将内存保护模式改为可写,老模式保存入dwOldProtect
	if (!VirtualProtectEx(hProcess,pfstrncpy,5,PAGE_READWRITE,&dwOldProtect)){
		rl.WriteLogInfo("保存老模式失败\n");
	}
	//将所属进程中add的前5个字节改为Jmp Myadd 
	if (!WriteProcessMemory(hProcess,pfstrncpy,NewCode,5,0)){
		rl.WriteLogInfo("跳转 failed\n");
	}
	//将内存保护模式改回为dwOldProtect
	if (!VirtualProtectEx(hProcess,pfstrncpy,5,dwOldProtect,&dwTemp)){
		rl.WriteLogInfo("改回来 failed\n");
	}


	bHook=true; 
}

void HookOff()//将所属进程中add()的入口代码恢复
{ 
	ASSERT(hProcess!=NULL);

	DWORD dwTemp=0;
	DWORD dwOldProtect;

	VirtualProtectEx(hProcess,pfstrncpy,5,PAGE_READWRITE,&dwOldProtect); 
	WriteProcessMemory(hProcess,pfstrncpy,OldCode,5,0); 
	VirtualProtectEx(hProcess,pfstrncpy,5,dwOldProtect,&dwTemp); 
	bHook=false; 
}