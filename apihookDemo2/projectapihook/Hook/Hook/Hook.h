// Hook.h : Hook DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CHookApp
// �йش���ʵ�ֵ���Ϣ������� Hook.cpp
//

class CHookApp : public CWinApp
{
public:
	CHookApp();

// ��д
public:
	virtual BOOL InitInstance();
	int ExitInstance();

	DECLARE_MESSAGE_MAP()
};
