// oneDlg.h : ͷ�ļ�
//

#pragma once


// ConeDlg �Ի���
class ConeDlg : public CDialog
{
// ����
public:
	ConeDlg(CWnd* pParent = NULL);	// ��׼���캯��

public:
 HINSTANCE hAddDll;
 typedef int (WINAPI*AddProc)(int a,int b);
 AddProc add;
 HINSTANCE hinst;

// �Ի�������
	enum { IDD = IDD_ONE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDoHook();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedUninstall();
};
