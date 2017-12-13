#include "stdafx.h"
#include "WinEventApp.h"
#include "PolicyMgr.h"


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
		if (FAILED(CPolicyMgr::Instance().Init()))
		{
			break;
		}

		if (FAILED(CWinEventHook::Instance().Hook(this)))
		{
			break;
		}

		Loop();

		CWinEventHook::Instance().UnHook();

		CPolicyMgr::Instance().UnInit();

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
	if (IsQuit())
	{
		return;
	}

	CPolicyMgr::Instance().Fire(event, hwnd, idObject, idChild, dwEventThread);
}