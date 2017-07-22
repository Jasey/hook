// Hook.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Hook.h"

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
	// Place all significant initinalization in InitInstance
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
typedef int (WINAPI*AddProc)(int a,int b);//add.dll�е�add��������
AddProc add; //add.dll�е�add����
HANDLE hProcess=NULL; //�������̵ľ��
FARPROC pfadd;  //ָ��add������Զָ��
DWORD dwPid;  //��������ID
//end of ��������

//��������
void HookOn(); //��������
void HookOff(); //�رչ���
LRESULT CALLBACK MouseProc(int nCode,WPARAM wParam,LPARAM lParam); //��깳�Ӻ���
void Inject(); //�������ע�䣬�滻��ڵĺ���
int WINAPI Myadd(int a,int b); //���Ƕ�����µ�add()����
BOOL InstallHook(); //��װ���Ӻ���
void UninstallHook(); //ж�ع��Ӻ���
//end of ��������


BOOL CHookApp::InitInstance() 
{
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
	if (hhk!=NULL)
		::UnhookWindowsHookEx(hhk);
	AfxMessageBox(_TEXT("Unhooked!"));
}

void Inject()
{

	if (m_bInjected==false)
	{
		m_bInjected=true;


		//��ȡadd.dll�����ҵ�add()����
		HMODULE hmod=::LoadLibrary(_TEXT("add.dll"));
		add=(AddProc)::GetProcAddress(hmod,"add");
		pfadd=(FARPROC)add;

		if (pfadd==NULL)
		{
			AfxMessageBox(_TEXT("cannot locate add()"));
		}

		// ��add()����ڴ��뱣�浽OldCode��
		_asm 
		{ 
			lea edi,OldCode 
				mov esi,pfadd 
				cld 
				movsd 
				movsb 
		}

		NewCode[0]=0xe9;//��һ���ֽ�0xe9�൱��jmpָ��
		//��ȡMyadd()����Ե�ַ
		_asm 
		{ 
			lea eax,Myadd
				mov ebx,pfadd 
				sub eax,ebx 
				sub eax,5 
				mov dword ptr [NewCode+1],eax 
		} 
		//�����ϣ�����NewCode[]������൱��ָ�� jmp Myadd
		HookOn();
	}
}

int WINAPI Myadd(int a,int b)
{
	//�ػ��˶�add()�ĵ��ã����Ǹ�a,b����1
	a=a+1;
	b=b+1;


	HookOff();//�ص�Myadd()���ӷ�ֹ��ѭ��

	int ret;
	ret=add(a,b);

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
	ASSERT(hProcess!=NULL);

	DWORD dwTemp=0;
	DWORD dwOldProtect;

	//���ڴ汣��ģʽ��Ϊ��д,��ģʽ������dwOldProtect
	VirtualProtectEx(hProcess,pfadd,5,PAGE_READWRITE,&dwOldProtect); 
	//������������add��ǰ5���ֽڸ�ΪJmp Myadd 
	WriteProcessMemory(hProcess,pfadd,NewCode,5,0);
	//���ڴ汣��ģʽ�Ļ�ΪdwOldProtect
	VirtualProtectEx(hProcess,pfadd,5,dwOldProtect,&dwTemp);

	bHook=true; 
}

void HookOff()//������������add()����ڴ���ָ�
{ 
	ASSERT(hProcess!=NULL);

	DWORD dwTemp=0;
	DWORD dwOldProtect;

	VirtualProtectEx(hProcess,pfadd,5,PAGE_READWRITE,&dwOldProtect); 
	WriteProcessMemory(hProcess,pfadd,OldCode,5,0); 
	VirtualProtectEx(hProcess,pfadd,5,dwOldProtect,&dwTemp); 
	bHook=false; 
}