#include "stdafx.h"
#include "QueryWndInfoPolicy.h"
#include "HelpApi.h"
#include <Winternl.h>
#include <bcrypt.h>
#include <Psapi.h>

CQueryWndInfoPolicy::CQueryWndInfoPolicy()
	: m_hToken(NULL)
{
}

CQueryWndInfoPolicy::~CQueryWndInfoPolicy()
{
}

HRESULT STDMETHODCALLTYPE CQueryWndInfoPolicy::Init()
{
	// 提升进程权限
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &m_hToken);
	HELP_API::PROCESS_THREAD_API::EnablePrivilege(m_hToken, SE_DEBUG_NAME);
	return S_OK;
}

void STDMETHODCALLTYPE CQueryWndInfoPolicy::UnInit()
{
	if (m_hToken)
	{
		CloseHandle(m_hToken);
		m_hToken = NULL;
	}
}

HRESULT STDMETHODCALLTYPE CQueryWndInfoPolicy::PolicyHandler(IPolicyObj* pPolicyObj)
{
	if (!pPolicyObj)
	{
		return E_FAIL;
	}

	HWND hWnd = (HWND)GetValue<ULONGLONG>(pPolicyObj, POLICY_INDEX_HWND);

	// 窗口标题、类名、style、exstyle
	CAtlString strText;
	::GetWindowText(hWnd, strText.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	strText.ReleaseBuffer();
	SetValue(pPolicyObj, POLICY_INDEX_TITLE, strText);

	CAtlString strClass;
	GetClassName(hWnd, strClass.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	strClass.ReleaseBuffer();
	SetValue(pPolicyObj, POLICY_INDEX_CLASS, strClass);

	SetValue(pPolicyObj, POLICY_INDEX_STYLE, GetWindowLong(hWnd, GWL_STYLE));
	SetValue(pPolicyObj, POLICY_INDEX_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE));

	// 所属进程、父进程，包括：pid、exe名、exe路径
	DWORD dwProcId;
	GetWindowThreadProcessId(hWnd, &dwProcId);
	SetValue(pPolicyObj, POLICY_INDEX_PID, dwProcId);

	// 建立一个PID缓存表，每次先从PID缓存表里面取数据，能够取出来，就以PID缓存表为准，无法取出来，则重新取

	CHandle hProcess;
	hProcess.Attach(OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcId));
	if (hProcess && hProcess.m_h != INVALID_HANDLE_VALUE)
	{
		CAtlString strProcPath;
		GetModuleFileName((HMODULE)hProcess.m_h, strProcPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
		strProcPath.ReleaseBuffer();

		if (!strProcPath.IsEmpty())
		{
			SetValue(pPolicyObj, POLICY_INDEX_EXE_PATH, strProcPath);
			CAtlString strFileName = PathFindFileName(strProcPath.GetString());
			SetValue(pPolicyObj, POLICY_INDEX_EXE_NAME, strFileName);
		}
		else
		{
			GetProcessImageFileName(hProcess.m_h, strProcPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
			strProcPath.ReleaseBuffer();
			if (!strProcPath.IsEmpty() && DeviceDosPathToNTPath(strProcPath))
			{
				SetValue(pPolicyObj, POLICY_INDEX_EXE_PATH, strProcPath);
				CAtlString strFileName = PathFindFileName(strProcPath.GetString());
				SetValue(pPolicyObj, POLICY_INDEX_EXE_NAME, strFileName);
			}
		}

		CAtlString strCmdline;
		DWORD dwParentPID = 0;
		GetProcCmdline(hProcess.m_h, strCmdline, &dwParentPID);
		SetValue(pPolicyObj, POLICY_INDEX_PARENT_PID, dwParentPID);
		SetValue(pPolicyObj, POLICY_INDEX_CMDLINE, strCmdline);
	}

	LOG_PRINT(L"%s, hWnd(0x%0x),Title(%s),Class(%s),Rect(%d,%d,%d,%d),Size(%d,%d),Style(0x%0x),ExStyle(0x%0x)",
		__FUNCTIONW__,
		(HWND)GetValue<ULONGLONG>(pPolicyObj, POLICY_INDEX_HWND),
		GetValue<CAtlString>(pPolicyObj, POLICY_INDEX_TITLE),
		GetValue<CAtlString>(pPolicyObj, POLICY_INDEX_CLASS),
		GetValue<LONG>(pPolicyObj, POLICY_INDEX_X),
		GetValue<LONG>(pPolicyObj, POLICY_INDEX_Y),
		GetValue<LONG>(pPolicyObj, POLICY_INDEX_X) + GetValue<LONG>(pPolicyObj, POLICY_INDEX_WIDTH),
		GetValue<LONG>(pPolicyObj, POLICY_INDEX_Y) + GetValue<LONG>(pPolicyObj, POLICY_INDEX_HEIGHT),
		GetValue<LONG>(pPolicyObj, POLICY_INDEX_WIDTH),
		GetValue<LONG>(pPolicyObj, POLICY_INDEX_HEIGHT),
		GetValue<LONG>(pPolicyObj, POLICY_INDEX_STYLE),
		GetValue<LONG>(pPolicyObj, POLICY_INDEX_EXSTYLE));
	LOG_PRINT(L"%s, hWnd(0x%0x),PID(%d),TID(%d),ExePath(%s),ExeName(%s),ParentPID(%d),Cmdline(%s)",
		__FUNCTIONW__,
		(HWND)GetValue<ULONGLONG>(pPolicyObj, POLICY_INDEX_HWND),
		GetValue<LONG>(pPolicyObj, POLICY_INDEX_PID),
		GetValue<LONG>(pPolicyObj, POLICY_INDEX_TID),
		GetValue<CAtlString>(pPolicyObj, POLICY_INDEX_EXE_PATH),
		GetValue<CAtlString>(pPolicyObj, POLICY_INDEX_EXE_NAME),
		GetValue<LONG>(pPolicyObj, POLICY_INDEX_PARENT_PID),
		GetValue<CAtlString>(pPolicyObj, POLICY_INDEX_CMDLINE));

	return S_OK;
}

BOOL CQueryWndInfoPolicy::GetProcCmdline(HANDLE hProcess, CAtlString& strCmdline, LPDWORD lpParentPID/* = NULL*/)
{
	BOOL bRet = FALSE;

	typedef NTSTATUS(WINAPI *NtQueryInformationProcessFake)(HANDLE, DWORD, PVOID, ULONG, PULONG);

	// 父进程ID及进程命令行
	CHandle hNewDup;
	if (DuplicateHandle(GetCurrentProcess(), hProcess, GetCurrentProcess(), &hNewDup.m_h, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
		PROCESS_BASIC_INFORMATION pbi;
		NtQueryInformationProcessFake ntQ = (NtQueryInformationProcessFake)GetProcAddress(LoadLibrary(_T("Ntdll.dll")), "NtQueryInformationProcess");
		if (ntQ)
		{
			NTSTATUS isok = ntQ(hNewDup, ProcessBasicInformation, (PVOID)&pbi, sizeof(PROCESS_BASIC_INFORMATION), 0);
			if (BCRYPT_SUCCESS(isok))
			{
				if (lpParentPID)
				{
					*lpParentPID = (DWORD)pbi.Reserved3;
				}
				PEB peb;
				if (ReadProcessMemory(hNewDup, pbi.PebBaseAddress, &peb, sizeof(PEB), 0))
				{
					RTL_USER_PROCESS_PARAMETERS upps;
					if (ReadProcessMemory(hNewDup, peb.ProcessParameters, &upps, sizeof(RTL_USER_PROCESS_PARAMETERS), 0))
					{
						ReadProcessMemory(hNewDup, upps.CommandLine.Buffer, strCmdline.GetBufferSetLength(upps.CommandLine.Length + 1), upps.CommandLine.Length, 0);
						strCmdline.ReleaseBuffer();
						bRet = TRUE;
					}
				}
			}
		}
	}

	return bRet;
}

BOOL CQueryWndInfoPolicy::DeviceDosPathToNTPath(CAtlString& strPath)
{
	BOOL bRet = FALSE;

	TCHAR szDriveStr[MAX_PATH + 1] = { 0 };
	if (GetLogicalDriveStrings(MAX_PATH, szDriveStr))
	{
		TCHAR szDriver[3] = { 0 };
		TCHAR szDevName[MAX_PATH + 1] = { 0 };
		for (size_t i = 0; szDriveStr[i]; i += 4)
		{
			if (!lstrcmpi(&(szDriveStr[i]), _T("A:\\")) || !lstrcmpi(&(szDriveStr[i]), _T("B:\\")))
				continue;

			szDriver[0] = szDriveStr[i];
			szDriver[1] = szDriveStr[i + 1];
			szDriver[2] = '\0';
			if (!QueryDosDevice(szDriver, szDevName, MAX_PATH))	// 查询 Dos 设备名 
			{
				break;
			}

			UINT cchDevName = lstrlen(szDevName);
			if (_tcsnicmp(strPath.GetString(), szDevName, cchDevName) == 0)//命中    
			{
				CAtlString strNTPath;
				strNTPath.Format(L"%s%s", szDriver, strPath.Mid(cchDevName));   
				strPath = strNTPath;
				bRet = TRUE;
				break;
			}
		}
	}

	return bRet;
}