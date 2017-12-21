#include "stdafx.h"
#include "HelpApi.h"
#include <TlHelp32.h>

DWORD HELP_API::PROCESS_THREAD_API::GetProcessIDFromThreadID(DWORD dwThreadID)
{
	DWORD dwProcessID = 0;

	THREADENTRY32 te32;
	te32.dwSize = sizeof(te32);
	HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	do
	{
		if (INVALID_HANDLE_VALUE == hSnap)
		{
			break;
		}

		BOOL bMore = ::Thread32First(hSnap, &te32);
		while (bMore)
		{
			if (dwThreadID == te32.th32ThreadID)
			{
				dwProcessID = te32.th32OwnerProcessID;
				break;
			}
			bMore = ::Thread32Next(hSnap, &te32);
		}


	} while (false);

	if (hSnap)
	{
		::CloseHandle(hSnap);
		hSnap = NULL;
	}

	return dwProcessID;
}

DWORD HELP_API::PROCESS_THREAD_API::GetProcessIDFromName(CAtlString strName)
{
	static DWORD dwProcessID = 0;
	if (dwProcessID)
	{
		return dwProcessID;
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	do
	{
		if (INVALID_HANDLE_VALUE == hSnap)
		{
			break;
		}

		BOOL bMore = ::Process32First(hSnap, &pe32);
		while (bMore)
		{
			if (strName.CompareNoCase(pe32.szExeFile) == 0)
			{
				dwProcessID = pe32.th32ProcessID;
				break;
			}
			bMore = ::Process32Next(hSnap, &pe32);
		}


	} while (false);

	if (hSnap)
	{
		::CloseHandle(hSnap);
		hSnap = NULL;
	}

	return dwProcessID;
}


BOOL HELP_API::PROCESS_THREAD_API::EnablePrivilege(HANDLE hToken, LPCWSTR szPrivName)
{
	TOKEN_PRIVILEGES tkp;

	LookupPrivilegeValue(NULL, szPrivName, &tkp.Privileges[0].Luid);//修改进程权限  
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL);//通知系统修改进程权限  

	return((GetLastError() == ERROR_SUCCESS));

}