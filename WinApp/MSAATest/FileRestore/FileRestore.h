
// FileRestore.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFileRestoreApp: 
// �йش����ʵ�֣������ FileRestore.cpp
//

class CFileRestoreApp : public CWinApp
{
public:
	CFileRestoreApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFileRestoreApp theApp;