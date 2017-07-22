// oneDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "one.h"
#include "oneDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// ConeDlg 对话框



ConeDlg::ConeDlg(CWnd* pParent): CDialog(ConeDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	hAddDll=::LoadLibrary(_TEXT("add.dll"));

}

void ConeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(ConeDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &ConeDlg::doHook)
	ON_BN_CLICKED(IDC_BUTTON2, &ConeDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// ConeDlg 消息处理程序

BOOL ConeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void ConeDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void ConeDlg::OnPaint()
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
HCURSOR ConeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void ConeDlg::OnOK(){

	// TODO: Add extra validation here
	if (hAddDll == NULL){
		hAddDll=::LoadLibrary(_TEXT("add.dll"));
	}

	add=(AddProc)::GetProcAddress(hAddDll,"add");

	int a=1;
	int b=2;


	int c=add(a,b);
	CString temp;
	temp.Format(_TEXT("%d+%d=%d"),a,b,c);
	AfxMessageBox(temp);
}

void ConeDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	typedef BOOL (CALLBACK *UnhookProc)(); 
	UnhookProc UninstallHook;

	UninstallHook=::GetProcAddress(hinst,"UninstallHook");
	if(UninstallHook!=NULL) UninstallHook();
	if (hinst!=NULL)
	{
		::FreeLibrary(hinst);
	}
	if (hAddDll!=NULL)
	{
		::FreeLibrary(hAddDll);
	}
	CDialog::OnCancel();
}


void ConeDlg::doHook()
{
	hinst=LoadLibrary(_TEXT("Hook.dll"));
	if(hinst==NULL)
	{
		AfxMessageBox(_TEXT("no hook.dll!"));
		return;
	}
	typedef BOOL (CALLBACK *inshook)(); 
	inshook insthook;

	insthook=::GetProcAddress(hinst,"InstallHook");
	if(insthook==NULL)
	{
		AfxMessageBox(_TEXT("func not found!"));
		return;
	}

	DWORD pid=::GetCurrentProcessId();
	BOOL ret=insthook();
}

void ConeDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
		typedef BOOL (CALLBACK *UnhookProc)(); 
	UnhookProc UninstallHook;

	UninstallHook=::GetProcAddress(hinst,"UninstallHook");
	if(UninstallHook!=NULL) UninstallHook();
	if (hinst!=NULL)
	{
		::FreeLibrary(hinst);
	}
	if (hAddDll!=NULL)
	{
		::FreeLibrary(hAddDll);
	}
}
