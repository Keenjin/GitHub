
// xcopyfile.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CxcopyfileApp: 
// �йش����ʵ�֣������ xcopyfile.cpp
//

class CxcopyfileApp : public CWinApp
{
public:
	CxcopyfileApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CxcopyfileApp theApp;