// nativeDlg.h : header file
//

#if !defined(AFX_NATIVEDLG_H__1CDBF463_2285_4FAB_8F67_D2E46FDF41AF__INCLUDED_)
#define AFX_NATIVEDLG_H__1CDBF463_2285_4FAB_8F67_D2E46FDF41AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CNativeDlg dialog

class CNativeDlg : public CDialog
{
// Construction
public:
	CNativeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CNativeDlg)
	enum { IDD = IDD_NATIVE_DIALOG };
	CString	m_Ip;
	CString	m_Port;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNativeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CNativeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnShengCheng();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NATIVEDLG_H__1CDBF463_2285_4FAB_8F67_D2E46FDF41AF__INCLUDED_)
