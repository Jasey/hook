
// JikexueyuanInjecterDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "JikexueyuanInjecter.h"
#include "JikexueyuanInjecterDlg.h"
#include "afxdialogex.h"
#include "tlhelp32.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//BOOL EnablePrivilege();

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CJikexueyuanInjecterDlg �Ի���



CJikexueyuanInjecterDlg::CJikexueyuanInjecterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJikexueyuanInjecterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJikexueyuanInjecterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CJikexueyuanInjecterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	
	ON_BN_CLICKED(IDC_OPEN, &CJikexueyuanInjecterDlg::OnBnClickedOpen)
	ON_BN_CLICKED(IDC_INJECT, &CJikexueyuanInjecterDlg::OnBnClickedInject)
END_MESSAGE_MAP()


// CJikexueyuanInjecterDlg ��Ϣ�������

BOOL CJikexueyuanInjecterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��


	//����ʹ�����·�ʽ����
	/*if (!EnablePrivilege())
	{
	MessageBox(_T("Enable Debug Privilege Failed!"));
	}*/
	

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CJikexueyuanInjecterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CJikexueyuanInjecterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CJikexueyuanInjecterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CJikexueyuanInjecterDlg::OnBnClickedOpen()
{
	CFileDialog filedialog(TRUE, 0, 0, 6UL, _T("DLL Files|*.dll|"));
	if (filedialog.DoModal()==IDOK)
	{
		CString Dllpath;
		Dllpath = filedialog.GetPathName();
		SetDlgItemText(IDC_DLLPATH, Dllpath);
	}
}

DWORD ProcessFind(LPCTSTR Exename)
{
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (!hProcess)
	{
		return FALSE;
	}
	PROCESSENTRY32 info;
	info.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hProcess,&info))
	{
		return FALSE;
	}
	while (true)
	{
		if (_tcscmp(info.szExeFile,Exename)==0)
		{
			return info.th32ProcessID;
		}
		if (!Process32Next(hProcess,&info))
		{
			return FALSE;
		}
	}
	return FALSE;

}

//������Ƶ��Сԭ����Щ����û����ϸ����
//��������ע���ʱ�򣬳����˱��Ϊ5�Ĵ���
//�ܿ�������ע����ϵͳ�Ĺؼ�����
//����������ɱ���赲��ϵͳ��ȫ�ȼ����ߵ��µ�
//���ʱ�򣬴�ҿ��Գ���һ���������Ȩ����
//�ڴ��ڳ�ʼ���У�OnInitDialog������ע�����ǰ���ö�����

//BOOL EnablePrivilege()
//{
//	HANDLE hToken = NULL;
//	TOKEN_PRIVILEGES tp;
//	LUID luid;
//
//	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY | TOKEN_READ, &hToken))
//		return FALSE;
//	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
//		return TRUE;
//
//	tp.PrivilegeCount = 1;
//	tp.Privileges[0].Luid = luid;
//	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
//
//	AdjustTokenPrivileges(hToken, FALSE, &tp, NULL, NULL, NULL);
//	CloseHandle(hToken);
//	return (GetLastError() == ERROR_SUCCESS);
//}


BOOL Inject(LPCTSTR DLLPath, DWORD ProcessID)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, ProcessID);
	if (!hProcess)
	{
		return FALSE;
	}
	SIZE_T PathSize = (_tcslen(DLLPath) + 1)*sizeof(TCHAR);
	LPVOID StartAddress = VirtualAllocEx(hProcess, NULL, PathSize, MEM_COMMIT, PAGE_READWRITE);
	if (!StartAddress)
	{
		return FALSE;
	}
	if (!WriteProcessMemory(hProcess,StartAddress,DLLPath,PathSize,NULL))
	{
		return FALSE;
	}
	PTHREAD_START_ROUTINE pfnStartAddress = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "LoadLibraryW");
	if (!pfnStartAddress)
	{
		return FALSE;
	}
	HANDLE hThread = CreateRemoteThreadEx(hProcess, NULL, NULL, pfnStartAddress, StartAddress, NULL, NULL, NULL);
	//Xp ��û���������
	//CreateRemoteThread
	if (!hThread)
	{
		return FALSE;
	}
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hProcess);
	return TRUE;
}


void CJikexueyuanInjecterDlg::OnBnClickedInject()
{
	CString Dllpath;
	CString Exename;
	GetDlgItemText(IDC_EXENAME, Exename);
	GetDlgItemText(IDC_DLLPATH, Dllpath);
	if (Exename.GetLength()==0)
	{
		MessageBox(_T("Please input exe name"));
		return;
	}
	DWORD ProcessID = ProcessFind(Exename);
	if (!ProcessID)
	{
		MessageBox(_T("Can't find the process!"));
		return;
	}
	BOOL IsInjected=Inject(Dllpath, ProcessID);
	if (IsInjected)
	{
		MessageBox(_T("Inject Success!"));
	}
	else
	{
		MessageBox(_T("Inject Failed!"));
	}
}
