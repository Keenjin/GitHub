#pragma once

namespace HELP_API
{
	namespace WND_EVENT_API
	{
		CAtlString WndEventName(DWORD dwID);
		CAtlString CtrlObjectName(DWORD dwID);
		CAtlString ChildIDName(DWORD dwID);
		HWND GetTopParentWnd(HWND hWnd);
	};

	namespace PROCESS_THREAD_API
	{
		DWORD GetProcessIDFromThreadID(DWORD dwThreadID);
		DWORD GetProcessIDFromName(CAtlString strName);

		BOOL EnablePrivilege(HANDLE hToken, LPCWSTR szPrivName);
	};
};
