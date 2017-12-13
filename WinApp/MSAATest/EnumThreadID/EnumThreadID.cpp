// EnumThreadID.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <TlHelp32.h>
#include <iostream>
#include <string>

using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
	cout << "枚举线程所属的进程ID" << endl;

	while (true)
	{
		cout << "待枚举的线程ID:";
		DWORD dwThreadID = 0;
		cin >> dwThreadID;
		if (dwThreadID == 0)
		{
			break;
		}

		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(pe32);
		THREADENTRY32 te32;
		te32.dwSize = sizeof(te32);
		HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD, 0);

		do
		{
			if (INVALID_HANDLE_VALUE == hSnap)
			{
				break;
			}

			BOOL bFind = FALSE;

			BOOL bMore = ::Thread32First(hSnap, &te32);
			while (bMore)
			{
				if (dwThreadID == te32.th32ThreadID)
				{
					bFind = TRUE;
					CAtlString strProcessName;

					BOOL bPMore = ::Process32First(hSnap, &pe32);
					while (bPMore)
					{
						if (pe32.th32ProcessID == te32.th32OwnerProcessID)
						{
							strProcessName = pe32.szExeFile;
						}
						bPMore = ::Process32Next(hSnap, &pe32);
					}
					cout << "对应进程ID：" << te32.th32OwnerProcessID << " 进程名:" << CW2A(strProcessName.GetString(), CP_UTF8) << endl;
					break;
				}
				bMore = ::Thread32Next(hSnap, &te32);
			}

			if (!bFind)
			{
				cout << "线程句柄未找到" << endl;
			}
		} while (false);

		if (hSnap)
		{
			::CloseHandle(hSnap);
			hSnap = NULL;
		}
	}

	return 0;
}

