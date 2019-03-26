#include "stdafx.h"
#include "WinEventHook.h"

// Callback function that handles events.
//
void CALLBACK CWinEventHook::HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
	LONG idObject, LONG idChild,
	DWORD dwEventThread, DWORD dwmsEventTime)
{
	CWinEventHook::Instance().OnWinEventCallback(event, hwnd,
		idObject, idChild,
		dwEventThread, dwmsEventTime);
}


CWinEventHook::CWinEventHook()
	: m_hWinEventHook(NULL)
	, m_pCallback(NULL)
	, m_bInitCOM(FALSE)
{
}


CWinEventHook::~CWinEventHook()
{
	UnHook();
}

CWinEventHook& CWinEventHook::Instance()
{
	static CWinEventHook theObj;
	return theObj;
}

HRESULT CWinEventHook::Hook(IWinEventHookCallback* pCallback)
{
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		return hr;
	}

	m_bInitCOM = TRUE;

	m_pCallback = pCallback;

	m_hWinEventHook = SetWinEventHook(
		EVENT_MIN, EVENT_MAX,  // Range of events (4 to 5).
		NULL,                                          // Handle to DLL.
		HandleWinEvent,                                // The callback.
		0, 0,              // Process and thread IDs of interest (0 = all)
		WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS); // Flags.

	return hr;
}

void CWinEventHook::UnHook()
{
	if (m_hWinEventHook)
	{
		UnhookWinEvent(m_hWinEventHook);
		m_hWinEventHook = NULL;
	}
	
	if (m_bInitCOM)
	{
		CoUninitialize();
		m_bInitCOM = FALSE;
	}
}

void CWinEventHook::OnWinEventCallback(DWORD event, HWND hwnd,
	LONG idObject, LONG idChild,
	DWORD dwEventThread, DWORD dwmsEventTime)
{
	if (m_pCallback)
	{
		m_pCallback->OnWinEventCallback(event, hwnd,
			idObject, idChild,
			dwEventThread, dwmsEventTime);
	}
}