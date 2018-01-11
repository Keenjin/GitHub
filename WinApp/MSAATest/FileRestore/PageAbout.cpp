// PageAbout.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FileRestore.h"
#include "PageAbout.h"
#include "afxdialogex.h"


// CPageAbout �Ի���

IMPLEMENT_DYNAMIC(CPageAbout, CDialogEx)

CPageAbout::CPageAbout(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPageAbout::IDD, pParent)
{

}

CPageAbout::~CPageAbout()
{
}

void CPageAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageAbout, CDialogEx)
END_MESSAGE_MAP()


// CPageAbout ��Ϣ�������
// CPageBasic message handlers
BOOL CPageAbout::Create(CWnd* pParent)
{
	BOOL bRet = FALSE;

	do
	{
		if (!pParent)
		{
			break;
		}

		if (!CDialogEx::Create(IDD_PAGE_ABOUT, pParent))
		{
			break;
		}

		SetBackgroundColor(RGB(255, 255, 255));

		// �ƶ�������λ��
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

BOOL CPageAbout::Show(BOOL bShow)
{
	int nCmd = bShow ? SW_SHOW : SW_HIDE;
	return CDialogEx::ShowWindow(nCmd);
}