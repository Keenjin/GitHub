
// FileRestoreDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FileRestore.h"
#include "FileRestoreDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

														// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};


CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CFileRestoreDlg dialog



CFileRestoreDlg::CFileRestoreDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_FILERESTORE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileRestoreDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_tabMain);
}

BEGIN_MESSAGE_MAP(CFileRestoreDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CFileRestoreDlg::OnTcnSelchangeTab)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CFileRestoreDlg message handlers

BOOL CFileRestoreDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_tabMain.InsertItem(TAB_INDEX_GENERAL, _T("首页"));
	m_tabMain.InsertItem(TAB_INDEX_ABOUT, _T("关于"));

	m_pageBasic.Create(GetDlgItem(IDC_TAB));
	m_pageAbout.Create(GetDlgItem(IDC_TAB));

	ShowPage(TAB_INDEX_GENERAL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFileRestoreDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFileRestoreDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFileRestoreDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFileRestoreDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	ETabIndex eIndex = (ETabIndex)m_tabMain.GetCurSel();
	*pResult = ShowPage(eIndex);
}

BOOL CFileRestoreDlg::ShowPage(ETabIndex eIndex)
{
	BOOL bRet = TRUE;

	switch (eIndex)
	{
	case TAB_INDEX_GENERAL:
		bRet &= m_pageBasic.Show(TRUE);
		bRet &= m_pageAbout.Show(FALSE);
		break;
	case TAB_INDEX_ABOUT:
		bRet &= m_pageBasic.Show(FALSE);
		bRet &= m_pageAbout.Show(TRUE);
		break;
	default:
		break;
	}

	return bRet;
}

void CFileRestoreDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	BOOL bCancelClose = FALSE;
	m_pageBasic.OnWndClose(bCancelClose);

	if (!bCancelClose)
	{
		m_pageAbout.EndDialog(0);
		CDialog::OnClose();
	}
}
