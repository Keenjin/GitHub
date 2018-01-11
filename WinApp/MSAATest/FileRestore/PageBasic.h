#pragma once
#include "afxwin.h"
#include "FileClusterTag.h"
#include "Thread.h"
#include "afxcmn.h"

#define CostTimerID	1

#define WM_UPDATE_BASIC	WM_USER + 111

enum EUpdateMsgID
{
	UPDATE_MSG_ID_UNKNOWN = 0,
	UPDATE_MSG_ID_END_THREAD,
	UPDATE_MSG_ID_START_PROGRESS,
	UPDATE_MSG_ID_INCREMENT_PROGRESS,
	UPDATE_MSG_ID_END_PROGRESS,
};

enum ERestoreMethod
{
	RESTORE_METHOD_UNKNOWN = 0,
	RESTORE_METHOD_FILE,
	RESTORE_METHOD_DISK,
};
// CPageBasic dialog

class CPageBasic 
	: public CDialogEx
	, public IThreadProcCallback
	, public IFileClusterCallback
{
	DECLARE_DYNAMIC(CPageBasic)

public:
	CPageBasic(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPageBasic();

	BOOL Create(CWnd* pParent);
	BOOL Show(BOOL bShow);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PAGE_BASIC };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	void ShowDiskSize();
	BOOL OpenExplorer(const CAtlString& strDir);
	void EnableAllCtrls(BOOL bEnable);

	void ShowProgress(BOOL bShow);

public:
	CThread m_thread;
	virtual void ThreadProc();

	void OnWndClose(BOOL& bCancelClose);
	BOOL IsRestoring();

	void StartCostTimer();
	void StopCostTimer();

	void NotifyUpdate(EUpdateMsgID eMsg, LPARAM lParam = 0);

	virtual LRESULT OnProgressStart(ULONGLONG ullTotal);
	virtual LRESULT OnProgressIncrement(ULONGLONG ullCurrent);
	virtual LRESULT OnProgressEnd();

public:
	CComboBox m_comboDisk;

	std::vector<DISKINFO>	m_vecDiskInfo;
	afx_msg void OnCbnSelchangeComboDisk();
	CEdit m_editSavePath;
	afx_msg void OnEnChangeEditSavepath();
	afx_msg void OnBnClickedBtnBrowser();
	afx_msg void OnBnClickedBtnOpen();
	CButton m_btnRestoreFile;
	CButton m_btnRestoreDisk;
	afx_msg void OnBnClickedBtnFilerestore();
	afx_msg void OnBnClickedBtnDiskrestore();
	CButton m_btnBrowser;
	CButton m_btnOpenDir;

	CAtlString m_strFileForRestore;
	ERestoreMethod m_eMethod;

	volatile BOOL	m_bRestoring;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	__time64_t m_tCostStart;

	afx_msg LRESULT OnUpdate(WPARAM wParam, LPARAM lParam);
	CProgressCtrl m_progressRestore;
};
