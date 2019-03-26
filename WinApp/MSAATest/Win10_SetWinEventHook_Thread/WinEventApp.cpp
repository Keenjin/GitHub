#include "stdafx.h"
#include "WinEventApp.h"
#include "HelpApi.h"


CWinEventApp::CWinEventApp()
	: m_bQuitLoop(FALSE)
{
}


CWinEventApp::~CWinEventApp()
{
}

CWinEventApp& CWinEventApp::Instance()
{
	static CWinEventApp theObj;
	return theObj;
}

HRESULT CWinEventApp::Init()
{
	LOG_PRINT(L"==============================================================================");
	return Start(this);
}

void CWinEventApp::UnInit()
{
	InterlockedExchange((volatile long*)&m_bQuitLoop, TRUE);
	PostThreadMessage(GetThreadID(), WM_QUIT, 0, 0);

	Wait(INFINITE);
	Stop();
	LOG_PRINT(L"==============================================================================");
}

BOOL CWinEventApp::IsQuit()
{
	BOOL bRet = FALSE;
	InterlockedExchange((volatile long*)&bRet, m_bQuitLoop);
	return bRet;
}

void  CWinEventApp::ThreadProc()
{
	do
	{
		if (FAILED(CWinEventHook::Instance().Hook(this)))
		{
			break;
		}

		Loop();

		CWinEventHook::Instance().UnHook();

	} while (FALSE);
}

void CWinEventApp::Loop()
{
	// 主消息循环: 
	MSG msg;
	while (!IsQuit() && GetMessage(&msg, NULL, 0, 0))
	{
		DispatchMessage(&msg);
	}
}

void CWinEventApp::OnWinEventCallback(DWORD event, HWND hwnd,
	LONG idObject, LONG idChild,
	DWORD dwEventThread, DWORD dwmsEventTime)
{
	//return;

	if (IsQuit())
	{
		return;
	}

	//Sleep(-1);

	// 这里执行一些特殊操作
	/*if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || NULL == hwnd || !::IsWindow(hwnd))
	{
		return;
	}*/

	/*int i = 1000000;
	while (i--);*/

	//Sleep(1000 * 10);

	// 打印出信息
	LOG_PRINT(L"%s, hwnd(%0x),event(%s),eventthread(%d),dwmsEventTime(%d),currentthread(%d)", __FUNCTIONW__,
		hwnd, HELP_API::WND_EVENT_API::WndEventName(event), dwEventThread, dwmsEventTime, GetCurrentThreadId());
}