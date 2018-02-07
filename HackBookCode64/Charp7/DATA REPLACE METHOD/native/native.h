// native.h : main header file for the NATIVE application
//

#if !defined(AFX_NATIVE_H__3C91C003_CD5B_4D0E_9BAF_3164D3558EB8__INCLUDED_)
#define AFX_NATIVE_H__3C91C003_CD5B_4D0E_9BAF_3164D3558EB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CNativeApp:
// See native.cpp for the implementation of this class
//

class CNativeApp : public CWinApp
{
public:
	CNativeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNativeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CNativeApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NATIVE_H__3C91C003_CD5B_4D0E_9BAF_3164D3558EB8__INCLUDED_)
