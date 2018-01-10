
// FileRestoreDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "PageBasic.h"

enum ETabIndex
{
	TAB_INDEX_GENERAL = 0,

	TAB_INDEX_ABOUT
};



// CFileRestoreDlg dialog
class CFileRestoreDlg : public CDialog
{
// Construction
public:
	CFileRestoreDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILERESTORE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_tabMain;
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);

	CPageBasic	m_pageBasic;
};
