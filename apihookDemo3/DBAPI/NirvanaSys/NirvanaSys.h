// NirvanaSys.h : main header file for the NIRVANASYS DLL
//

#if !defined(AFX_NIRVANASYS_H__04CB1D18_98F6_43CF_B1F8_5EEC9126EDB4__INCLUDED_)
#define AFX_NIRVANASYS_H__04CB1D18_98F6_43CF_B1F8_5EEC9126EDB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CNirvanaSysApp
// See NirvanaSys.cpp for the implementation of this class
//


#define HOOK_DLL_API __declspec(dllexport)

typedef struct dbinfo{
    CString user;
    CString pass;
    CString server;
	CString dbname;
}dbinfo_t;

static dbinfo_t dataInfo;

typedef struct GetPram{
	bool unicodeFlag;	//unicode标识
	char SysFlag[100];		//收银系统品牌
	char FilePath[260];
	HWND hWnd;			//窗口句柄,用于发送成功提示
}GetPram_t;

typedef struct TradePram{
	CString PrintFlag;		//交易SQL
	CString SysFlag;		//收银系统品牌
	CString FilePath;
	HWND hWnd;			//窗口句柄,用于发送成功提示
	bool CutPaper;
	char *AddPrintData;
	int AddLength;
}TradePram_t;

class CNirvanaSysApp : public CWinApp
{
public:
	CNirvanaSysApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNirvanaSysApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CNirvanaSysApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NIRVANASYS_H__04CB1D18_98F6_43CF_B1F8_5EEC9126EDB4__INCLUDED_)
