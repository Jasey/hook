
// JikexueyuanInjecter.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CJikexueyuanInjecterApp: 
// �йش����ʵ�֣������ JikexueyuanInjecter.cpp
//

class CJikexueyuanInjecterApp : public CWinApp
{
public:
	CJikexueyuanInjecterApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CJikexueyuanInjecterApp theApp;