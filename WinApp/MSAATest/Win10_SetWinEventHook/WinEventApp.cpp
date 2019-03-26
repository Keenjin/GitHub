#include "stdafx.h"
#include "WinEventApp.h"
#include "HelpApi.h"
#include <atltypes.h>


CWinEventApp::CWinEventApp()
{
	CWinEventHook::Instance().Hook(this);
}


CWinEventApp::~CWinEventApp()
{
	CWinEventHook::Instance().UnHook();
}

void CWinEventApp::OnWinEventCallback(DWORD event, HWND hwnd,
	LONG idObject, LONG idChild,
	DWORD dwEventThread, DWORD dwmsEventTime)
{
	// 测试下正常步骤
	if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || NULL == hwnd || !::IsWindow(hwnd))
	{
		return;
	}

	Sleep(-1);

	DWORD dwEvent = event;
	DWORD dwEventThreadTmp = dwEventThread;

	// 获取窗口大小、title、类名等
	/*HWND hTopWnd = HELP_API::WND_EVENT_API::GetTopParentWnd(hwnd);
	if (hTopWnd != (HWND)0x3307A6)
	{
		return;
	}*/

	CRect rcWnd;
	GetWindowRect(hwnd, &rcWnd);

	// 窗口标题、类名、style、exstyle
	CAtlString strText;
	::GetWindowText(hwnd, strText.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	strText.ReleaseBuffer();

	CAtlString strClass;
	GetClassName(hwnd, strClass.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	strClass.ReleaseBuffer();

	DWORD dwProcId;
	GetWindowThreadProcessId(hwnd, &dwProcId);

	// 打印出信息
	LOG_PRINT(L"%s, hwnd(%0x),event(%s),eventthread(%d),dwmsEventTime(%d)", __FUNCTIONW__,
		hwnd, HELP_API::WND_EVENT_API::WndEventName(event), dwEventThread, dwmsEventTime);
}