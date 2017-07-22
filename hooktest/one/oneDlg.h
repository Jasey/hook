// oneDlg.h : 头文件
//

#pragma once


// ConeDlg 对话框
class ConeDlg : public CDialog
{
// 构造
public:
	ConeDlg(CWnd* pParent = NULL);	// 标准构造函数

public:
 HINSTANCE hAddDll;
 HINSTANCE hinst;
 typedef int (WINAPI*AddProc)(int a,int b);
 AddProc add;

// 对话框数据
	enum { IDD = IDD_ONE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnOK();

	afx_msg void OnCancel();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void doHook();
	afx_msg void OnBnClickedButton2();
};
