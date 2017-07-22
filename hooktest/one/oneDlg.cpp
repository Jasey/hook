// oneDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "one.h"
#include "oneDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// ConeDlg �Ի���



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


// ConeDlg ��Ϣ�������

BOOL ConeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void ConeDlg::OnPaint()
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
