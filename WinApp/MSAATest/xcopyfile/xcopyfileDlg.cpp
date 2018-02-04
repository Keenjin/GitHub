
// xcopyfileDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "xcopyfile.h"
#include "xcopyfileDlg.h"
#include "afxdialogex.h"
#include <atlpath.h>
#include <atlstr.h>
#include <atltime.h>
#include <atlfile.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CxcopyfileDlg �Ի���



CxcopyfileDlg::CxcopyfileDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CxcopyfileDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CxcopyfileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT3, m_oInput);
	DDX_Control(pDX, IDC_EDIT1, m_oDstDir);
	DDX_Control(pDX, IDC_EDIT2, m_oOutputFile);
	DDX_Control(pDX, IDC_EDIT4, m_oSrcRoot);
}

BEGIN_MESSAGE_MAP(CxcopyfileDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CxcopyfileDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CxcopyfileDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &CxcopyfileDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CxcopyfileDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON3, &CxcopyfileDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CxcopyfileDlg ��Ϣ�������

BOOL CxcopyfileDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	m_oDstDir.SetWindowText(L"C:\\Users\\Keen\\Desktop\\work-win7");
	m_oSrcRoot.SetWindowText(L"C:\\");

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CxcopyfileDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CxcopyfileDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CxcopyfileDlg::OnBnClickedButton1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
		if (strDir.Right(1) != L'\\' && strDir.Right(1) != L'/')
		{
			strDir += L"\\";
		}

		m_oDstDir.SetWindowText(strDir);
	}
}


void CxcopyfileDlg::OnBnClickedButton2()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	// Ŀ��Ŀ¼
	CString strDstDir;
	m_oDstDir.GetWindowText(strDstDir);

	CString strSrcRoot;
	m_oSrcRoot.GetWindowText(strSrcRoot);

	if (strDstDir.IsEmpty())
	{
		MessageBox(L"Ŀ��Ŀ¼Ϊ��", L"��ʾ", MB_OK | MB_ICONERROR);
		return;
	}

	if (!ATLPath::FileExists(strDstDir))
	{
		if (!CreateDirectory(strDstDir, NULL))
		{
			MessageBox(L"Ŀ¼����ʧ��", L"��ʾ", MB_OK | MB_ICONERROR);
			return;
		}
	}

	if (strDstDir.Right(1) != L'\\' && strDstDir.Right(1) != L'/')
	{
		strDstDir += L"\\";
	}

	CTime time(_time64(NULL));

	CAtlString strDstFile = strDstDir;
	CAtlString strDstFileName;
	strDstFileName.Format(L"script%04d%02d%02d_%d.bat", time.GetYear(), time.GetMonth(), time.GetDay(), GetTickCount());
	strDstFile += strDstFileName;

	CAtlFile file;
	HRESULT hr = file.Create(strDstFile, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, CREATE_ALWAYS);
	if (FAILED(hr))
	{
		MessageBox(L"�ļ�����ʧ��", L"��ʾ", MB_OK | MB_ICONERROR);
		return;
	}
	

	// ��inputȡ����ÿһ�У�д���ļ�
	CAtlString strBuf;
	for (size_t i = 0; i < m_oInput.GetLineCount(); i++)
	{
		int nLineLen = m_oInput.LineLength(m_oInput.LineIndex(i));
		m_oInput.GetLine(i, strBuf.GetBufferSetLength(nLineLen), nLineLen);
		strBuf += L"\0";

		// �滻��Ŀ¼
		CAtlString strNewline;
		strNewline.Format(L"xcopy \"%s\" \"%s%s\" /s /h /d /y\r\n", strBuf, strDstDir, strBuf.Right(strBuf.GetLength() - strSrcRoot.GetLength()));

		CAtlStringA strAscii = CW2A(strNewline, CP_ACP);
		file.Write(strAscii.GetString(), strAscii.GetLength());
	}

	// ���浽Ŀ���ļ���
	m_oOutputFile.SetWindowText(strDstFileName);
}


void CxcopyfileDlg::OnBnClickedButton4()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BOOL bRet = FALSE;

	CString strDir;
	m_oDstDir.GetWindowText(strDir);
	if (!ATLPath::FileExists(strDir))
	{
		if (!CreateDirectory(strDir, NULL))
		{
			MessageBox(L"Ŀ¼����ʧ��", L"��ʾ", MB_OK | MB_ICONERROR);
			return;
		}
	}

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
}


void CxcopyfileDlg::OnBnClickedButton5()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	// ��Notepad��bat�ĵ�
	BOOL bRet = FALSE;

	CString strDir;
	m_oDstDir.GetWindowText(strDir);
	CString strFileName;
	m_oOutputFile.GetWindowText(strFileName);
	if (strDir.Right(1) != L'\\' && strDir.Right(1) != L'//')
	{
		strDir += L"\\";
	}
	strDir += strFileName;
	if (!ATLPath::FileExists(strDir) || ATLPath::IsDirectory(strDir))
	{
		MessageBox(L"�ļ�������", L"��ʾ", MB_OK | MB_ICONERROR);
		return;
	}

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
		strPath += L"notepad.exe";

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
}


void CxcopyfileDlg::OnBnClickedButton3()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	// ִ�нű�
	CString strCmd;
	m_oDstDir.GetWindowText(strCmd);
	CString strFileName;
	m_oOutputFile.GetWindowText(strFileName);
	if (strCmd.Right(1) != L'\\' && strCmd.Right(1) != L'//')
	{
		strCmd += L"\\";
	}
	strCmd += strFileName;
	if (!ATLPath::FileExists(strCmd) || ATLPath::IsDirectory(strCmd))
	{
		MessageBox(L"�ļ�������", L"��ʾ", MB_OK | MB_ICONERROR);
		return;
	}
	
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	if (CreateProcess(NULL,
		(LPTSTR)strCmd.GetString(),
		NULL,
		NULL,
		FALSE,
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		&si, &pi))
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}
