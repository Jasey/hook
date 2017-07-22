
// JikexueyuanInjecterDlg.cpp : 实现文件
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

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CJikexueyuanInjecterDlg 对话框



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


// CJikexueyuanInjecterDlg 消息处理程序

BOOL CJikexueyuanInjecterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标


	//可以使用如下方式调用
	/*if (!EnablePrivilege())
	{
	MessageBox(_T("Enable Debug Privilege Failed!"));
	}*/
	

	// TODO:  在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CJikexueyuanInjecterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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

//由于视频大小原因，有些问题没能详细解释
//如果大家在注入的时候，出现了标号为5的错误
//很可能是你注入了系统的关键进程
//或者是由于杀软阻挡，系统安全等级过高导致的
//这个时候，大家可以尝试一下下面的提权函数
//在窗口初始化中（OnInitDialog）或者注入过程前引用都可以

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
	//Xp 中没有这个函数
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
