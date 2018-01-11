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
	, m_eMethod(RESTORE_METHOD_UNKNOWN)
	, m_bRestoring(FALSE)
{

}

CPageBasic::~CPageBasic()
{
}

void CPageBasic::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_comboDisk);
	DDX_Control(pDX, IDC_EDIT_SAVEPATH, m_editSavePath);
	DDX_Control(pDX, IDC_BTN_FILERESTORE, m_btnRestoreFile);
	DDX_Control(pDX, IDC_BTN_DISKRESTORE, m_btnRestoreDisk);
	DDX_Control(pDX, IDC_BTN_BROWSER, m_btnBrowser);
	DDX_Control(pDX, IDC_BTN_OPEN, m_btnOpenDir);
	DDX_Control(pDX, IDC_PROGRESS_RESTORE, m_progressRestore);
}


BEGIN_MESSAGE_MAP(CPageBasic, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_DISK, &CPageBasic::OnCbnSelchangeComboDisk)
	ON_EN_CHANGE(IDC_EDIT_SAVEPATH, &CPageBasic::OnEnChangeEditSavepath)
	ON_BN_CLICKED(IDC_BTN_BROWSER, &CPageBasic::OnBnClickedBtnBrowser)
	ON_BN_CLICKED(IDC_BTN_OPEN, &CPageBasic::OnBnClickedBtnOpen)
	ON_BN_CLICKED(IDC_BTN_FILERESTORE, &CPageBasic::OnBnClickedBtnFilerestore)
	ON_BN_CLICKED(IDC_BTN_DISKRESTORE, &CPageBasic::OnBnClickedBtnDiskrestore)
	ON_WM_TIMER()
	ON_MESSAGE(WM_UPDATE_BASIC, OnUpdate)
END_MESSAGE_MAP()


LRESULT CPageBasic::OnUpdate(WPARAM wParam, LPARAM lParam)
{
	EUpdateMsgID eMsg = (EUpdateMsgID)wParam;
	static LPARAM lProgressTotal = 0;
	switch (eMsg)
	{
	case UPDATE_MSG_ID_END_THREAD:
	{
		// ɨ��������ɹ��ָ��˶��ٸ��ļ�
		CAtlString strFinish;
		strFinish.Format(L"ɨ��������ɹ��ָ�%d���ļ�", lParam);
		MessageBox(strFinish.GetString(), L"���Թܼ�", MB_OK | MB_ICONINFORMATION);
		EnableAllCtrls(TRUE);
		ShowProgress(FALSE);
	}
		break;
	case UPDATE_MSG_ID_START_PROGRESS:
	{
		lProgressTotal = lParam;
		m_progressRestore.SetRange32(0, lParam);
		m_progressRestore.OffsetPos(0);
	}
		break;
	case UPDATE_MSG_ID_END_PROGRESS:
	{
		int nStart, nEnd;
		m_progressRestore.GetRange(nStart, nEnd);
		m_progressRestore.OffsetPos(nEnd - nStart);

		StopCostTimer();

		CAtlString strPercent;
		strPercent.Format(L"100%%");
		SetDlgItemText(IDC_STATIC_PERCENT, strPercent);
	}
		break;
	case UPDATE_MSG_ID_INCREMENT_PROGRESS:
	{
		m_progressRestore.OffsetPos(lParam);
		CAtlString strPercent;
		strPercent.Format(L"%d%%", lParam * 100 / lProgressTotal);
		SetDlgItemText(IDC_STATIC_PERCENT, strPercent);
	}
		break;
	default:
		break;
	}

	return S_OK;
}

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

		// �ƶ�������λ��
		CRect rcWnd;
		pParent->GetClientRect(&rcWnd);
		rcWnd.left += 2;
		rcWnd.top += 21;
		rcWnd.right -= 3;
		rcWnd.bottom -= 2;
		MoveWindow(&rcWnd);

		CFileClusterTag	fileRestore;
		fileRestore.EnumDiskDevice(m_vecDiskInfo);

		for (size_t i = 0; i < m_vecDiskInfo.size(); i++)
		{
			m_comboDisk.InsertString(i, m_vecDiskInfo[i].strFriendName);
		}

		m_comboDisk.SetCurSel(0);

		// ���ô��̴�С
		ShowDiskSize();

		// Ĭ�ϴ��·��
		m_editSavePath.SetWindowText(L"C:\\Restore\\");

		// ������Ĭ�ϲ�չʾ
		ShowProgress(FALSE);

		bRet = TRUE;

	} while (FALSE);

	return bRet;
}

BOOL CPageBasic::Show(BOOL bShow)
{
	int nCmd = bShow ? SW_SHOW : SW_HIDE;
	return CDialogEx::ShowWindow(nCmd);
}

void CPageBasic::OnCbnSelchangeComboDisk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	ShowDiskSize();
}


void CPageBasic::ShowDiskSize()
{
#define GB_UNIT		1024ull * 1024ull * 1024ull
#define MB_UNIT		1024ull * 1024ull
#define KB_UNIT		1024ull

	ULONGLONG ullSize = m_vecDiskInfo[m_comboDisk.GetCurSel()].ullDiskSize;
	CAtlString strSize;
	if (ullSize >= GB_UNIT)
	{
		strSize.Format(L"%.2f GB", (double)ullSize / (GB_UNIT * 1.0));
	}
	else if (ullSize >= MB_UNIT && ullSize < GB_UNIT)
	{
		strSize.Format(L"%.2f MB", (double)ullSize / (MB_UNIT * 1.0));
	}
	else if (ullSize >= KB_UNIT && ullSize < MB_UNIT)
	{
		strSize.Format(L"%.2f B", (double)ullSize / (KB_UNIT * 1.0));
	}
	else
	{
		strSize.Format(L"%I64u B", ullSize);
	}
	
	SetDlgItemText(IDC_STATIC_DISKSIZE, strSize);
}

void CPageBasic::OnEnChangeEditSavepath()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CPageBasic::OnBnClickedBtnBrowser()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	// ���ļ���
	CAtlString strDir;
	do
	{
		TCHAR szDir[MAX_PATH] = { 0 };
		BROWSEINFO    bi;
		bi.hwndOwner = GetSafeHwnd();
		bi.pidlRoot = NULL;
		bi.pszDisplayName = szDir;
		bi.lpszTitle = L"ѡ����·��";
		bi.ulFlags = BIF_RETURNONLYFSDIRS;
		bi.lpfn = NULL;
		bi.lParam = 0;
		bi.iImage = 0;

		ITEMIDLIST *pidl = SHBrowseForFolder(&bi);
		if (!pidl)
		{
			break;
		}

		if (!SHGetPathFromIDList(pidl, szDir))
		{
			break;
		}

		strDir = szDir;

	} while (FALSE);

	if (!strDir.IsEmpty())
	{
		m_editSavePath.SetWindowText(strDir);
	}
}


void CPageBasic::OnBnClickedBtnOpen()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	// ��ĳ��Ŀ¼
	CString strDir;
	m_editSavePath.GetWindowText(strDir);
	if (!ATLPath::FileExists(strDir))
	{
		CreateDirectory(strDir, NULL);
	}

	if (!ATLPath::FileExists(strDir))
	{
		MessageBox(L"��ЧĿ¼", L"���Թܼ�", MB_OK | MB_ICONWARNING);
	}
	else
	{
		OpenExplorer(strDir.GetString());
	}
}

BOOL CPageBasic::OpenExplorer(const CAtlString& strDir)
{
	BOOL bRet = FALSE;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	do
	{
		// ��ȡwindowsĿ¼
		CAtlString strPath;
		if (GetWindowsDirectory(strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH + 1) == 0)
		{
			strPath = L"C:\\Windows\\";
		}
		else
		{
			strPath.ReleaseBuffer();
		}

		if (strPath.Right(1) != L'\\' && strPath.Right(1) != L'/')
		{
			strPath += L"\\";
		}
		strPath += L"explorer.exe";

		CAtlString strCmdLine;
		strCmdLine.Format(L"\"%s\" %s", strPath.GetString(), strDir.GetString());

		// Start the child process. 
		if (!CreateProcess(NULL,   // No module name (use command line)
			strCmdLine.GetBuffer(),        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			CREATE_NO_WINDOW/*CREATE_NO_WINDOW*/,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi)           // Pointer to PROCESS_INFORMATION structure
			)
		{
			break;
		}

		bRet = TRUE;

	} while (FALSE);

	if (pi.hProcess)
	{
		CloseHandle(pi.hProcess);
		pi.hProcess = NULL;
	}
	if (pi.hThread)
	{
		CloseHandle(pi.hThread);
		pi.hThread = NULL;
	}

	return bRet;
}

void CPageBasic::ShowProgress(BOOL bShow)
{
	int nCmd = bShow ? SW_SHOWNOACTIVATE : SW_HIDE;
	GetDlgItem(IDC_LABEL_COSTTIME)->ShowWindow(nCmd);
	GetDlgItem(IDC_STATIC_COSTTIME)->ShowWindow(nCmd);
	GetDlgItem(IDC_STATIC_PERCENT)->ShowWindow(nCmd);

	m_progressRestore.ShowWindow(nCmd);
}

void CPageBasic::OnBnClickedBtnFilerestore()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CFileDialog fileDlg(TRUE, _T(""), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("All Files (*.*)|*.*||"), NULL);
	if (fileDlg.DoModal() == IDOK)
	{
		EnableAllCtrls(FALSE);
		m_strFileForRestore = fileDlg.GetFileName();
		m_eMethod = RESTORE_METHOD_FILE;

		//ShowProgress(TRUE);

		m_thread.Start(this);
	}
}


void CPageBasic::OnBnClickedBtnDiskrestore()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int nRet = MessageBox(L"ȫ�ָ̻���Ҫ���ѽϳ�ʱ�䣬��ȷ����Ҫ����ô��", L"���Թܼ�", MB_OKCANCEL | MB_ICONINFORMATION);
	if (nRet == IDOK)
	{
		EnableAllCtrls(FALSE);
		m_eMethod = RESTORE_METHOD_DISK;

		ShowProgress(TRUE);
		StartCostTimer();

		m_thread.Start(this);
	}
}

void CPageBasic::EnableAllCtrls(BOOL bEnable)
{
	m_btnRestoreFile.EnableWindow(bEnable);
	m_btnRestoreDisk.EnableWindow(bEnable);
	m_comboDisk.EnableWindow(bEnable);
	m_editSavePath.EnableWindow(bEnable);
	m_btnBrowser.EnableWindow(bEnable);
	m_btnOpenDir.EnableWindow(bEnable);
}

void CPageBasic::ThreadProc()
{
	CString strDir;
	m_editSavePath.GetWindowText(strDir);

	InterlockedExchange((volatile long*)&m_bRestoring, TRUE);

	if (m_eMethod == RESTORE_METHOD_FILE)
	{
		CFileClusterTag	fileRestore;
		fileRestore.RemoveTag(m_strFileForRestore, strDir);
	}
	else if (m_eMethod == RESTORE_METHOD_DISK)
	{
		CFileClusterTag	fileRestore;
		CAtlString strDevName;
		strDevName.Format(L"\\\\.\\PHYSICALDRIVE%d", m_vecDiskInfo[m_comboDisk.GetCurSel()].dwPhysicNum);
		fileRestore.DiskRestore(strDevName, 0, m_vecDiskInfo[m_comboDisk.GetCurSel()].ullDiskSize, strDir, this);
	}

	InterlockedExchange((volatile long*)&m_bRestoring, FALSE);

	NotifyUpdate(UPDATE_MSG_ID_END_THREAD);
}

void CPageBasic::NotifyUpdate(EUpdateMsgID eMsg, LPARAM lParam)
{
	PostMessage(WM_UPDATE_BASIC, eMsg, lParam);
}

LRESULT CPageBasic::OnProgressStart(ULONGLONG ullTotal)
{
	NotifyUpdate(UPDATE_MSG_ID_START_PROGRESS, ullTotal / (1024ll*1024ll));
	return S_OK;
}

LRESULT CPageBasic::OnProgressIncrement(ULONGLONG ullCurrent)
{
	NotifyUpdate(UPDATE_MSG_ID_INCREMENT_PROGRESS, ullCurrent / (1024ll * 1024ll));
	return S_OK;
}

LRESULT CPageBasic::OnProgressEnd(DWORD dwSuccCnt)
{
	NotifyUpdate(UPDATE_MSG_ID_END_PROGRESS, dwSuccCnt);
	return S_OK;
}

void CPageBasic::OnWndClose(BOOL& bCancelClose)
{
	// ���ڻָ��еĻ�����Ҫ��֪�û�����ǰ�Ƿ�ر�
	if (IsRestoring())
	{
		bCancelClose = TRUE;

		int nRet = MessageBox(L"��ǰ���ڽ����ļ��ָ����رս�ȡ���ָ����̡��Ƿ�ȡ����", L"���Թܼ�", MB_YESNO | MB_ICONINFORMATION);
		if (nRet == IDYES)
		{
			bCancelClose = FALSE;
			m_thread.Term();
		}
	}

	if (!bCancelClose)
	{
		EndDialog(0);
	}
}

BOOL CPageBasic::IsRestoring()
{
	BOOL bRestoring = FALSE;
	InterlockedExchange((volatile long*)&bRestoring, m_bRestoring);
	return bRestoring;
}


void CPageBasic::StartCostTimer()
{
	SetTimer(CostTimerID, 1000, NULL);
	m_tCostStart = _time64(NULL);
}

void CPageBasic::StopCostTimer()
{
	KillTimer(CostTimerID);
}

void CPageBasic::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent == CostTimerID)
	{
		CTimeSpan ts(_time64(NULL) - m_tCostStart);
		CAtlString strTime;
		strTime.Format(L"%02d:%02d:%02d", ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());
		SetDlgItemText(IDC_STATIC_COSTTIME, strTime);
	}

	__super::OnTimer(nIDEvent);
}
