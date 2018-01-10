// PageBasic.cpp : implementation file
//

#include "stdafx.h"
#include "FileRestore.h"
#include "PageBasic.h"
#include "afxdialogex.h"


// CPageBasic dialog

IMPLEMENT_DYNAMIC(CPageBasic, CDialogEx)

CPageBasic::CPageBasic(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PAGE_BASIC, pParent)
{

}

CPageBasic::~CPageBasic()
{
}

void CPageBasic::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageBasic, CDialogEx)
END_MESSAGE_MAP()


// CPageBasic message handlers
BOOL CPageBasic::Create(CWnd* pParent)
{
	BOOL bRet = FALSE;

	do
	{
		if (!pParent)
		{
			break;
		}

		if (!CDialogEx::Create(IDD_PAGE_BASIC, pParent))
		{
			break;
		}

		SetBackgroundColor(RGB(255, 255, 255));

		// 移动到合适位置
		CRect rcWnd;
		pParent->GetClientRect(&rcWnd);
		rcWnd.left += 2;
		rcWnd.top += 21;
		rcWnd.right -= 3;
		rcWnd.bottom -= 2;
		MoveWindow(&rcWnd);

		bRet = TRUE;

	} while (FALSE);

	return bRet;
}

BOOL CPageBasic::Show(BOOL bShow)
{
	int nCmd = bShow ? SW_SHOW : SW_HIDE;
	return CDialogEx::ShowWindow(nCmd);
}