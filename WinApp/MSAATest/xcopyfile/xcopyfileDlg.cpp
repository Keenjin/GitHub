
// xcopyfileDlg.cpp : 实现文件
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


// CxcopyfileDlg 对话框



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


// CxcopyfileDlg 消息处理程序

BOOL CxcopyfileDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	m_oDstDir.SetWindowText(L"C:\\Users\\Keen\\Desktop\\work-win7");
	m_oSrcRoot.SetWindowText(L"C:\\");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CxcopyfileDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CxcopyfileDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CxcopyfileDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	CAtlString strDir;
	do
	{
		TCHAR szDir[MAX_PATH] = { 0 };
		BROWSEINFO    bi;
		bi.hwndOwner = GetSafeHwnd();
		bi.pidlRoot = NULL;
		bi.pszDisplayName = szDir;
		bi.lpszTitle = L"选择存放路径";
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
	// TODO:  在此添加控件通知处理程序代码
	// 目标目录
	CString strDstDir;
	m_oDstDir.GetWindowText(strDstDir);

	CString strSrcRoot;
	m_oSrcRoot.GetWindowText(strSrcRoot);

	if (strDstDir.IsEmpty())
	{
		MessageBox(L"目标目录为空", L"提示", MB_OK | MB_ICONERROR);
		return;
	}

	if (!ATLPath::FileExists(strDstDir))
	{
		if (!CreateDirectory(strDstDir, NULL))
		{
			MessageBox(L"目录创建失败", L"提示", MB_OK | MB_ICONERROR);
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
		MessageBox(L"文件创建失败", L"提示", MB_OK | MB_ICONERROR);
		return;
	}
	

	// 从input取出来每一行，写入文件
	CAtlString strBuf;
	for (size_t i = 0; i < m_oInput.GetLineCount(); i++)
	{
		int nLineLen = m_oInput.LineLength(m_oInput.LineIndex(i));
		m_oInput.GetLine(i, strBuf.GetBufferSetLength(nLineLen), nLineLen);
		strBuf += L"\0";

		// 替换根目录
		CAtlString strNewline;
		strNewline.Format(L"xcopy \"%s\" \"%s%s\" /s /h /d /y\r\n", strBuf, strDstDir, strBuf.Right(strBuf.GetLength() - strSrcRoot.GetLength()));

		CAtlStringA strAscii = CW2A(strNewline, CP_ACP);
		file.Write(strAscii.GetString(), strAscii.GetLength());
	}

	// 保存到目标文件名
	m_oOutputFile.SetWindowText(strDstFileName);
}


void CxcopyfileDlg::OnBnClickedButton4()
{
	// TODO:  在此添加控件通知处理程序代码
	BOOL bRet = FALSE;

	CString strDir;
	m_oDstDir.GetWindowText(strDir);
	if (!ATLPath::FileExists(strDir))
	{
		if (!CreateDirectory(strDir, NULL))
		{
			MessageBox(L"目录创建失败", L"提示", MB_OK | MB_ICONERROR);
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
		// 获取windows目录
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
	// TODO:  在此添加控件通知处理程序代码
	// 用Notepad打开bat文档
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
		MessageBox(L"文件不存在", L"提示", MB_OK | MB_ICONERROR);
		return;
	}

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	do
	{
		// 获取windows目录
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
	// TODO:  在此添加控件通知处理程序代码
	// 执行脚本
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
		MessageBox(L"文件不存在", L"提示", MB_OK | MB_ICONERROR);
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
