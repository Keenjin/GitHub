#pragma once
#include "Thread.h"
#include "WinEventHook.h"

class CWinEventApp
	: public CThread
	, public IWinEventHookCallback
{
public:
	static CWinEventApp& Instance();

	HRESULT Init();
	void UnInit();

	virtual void OnWinEventCallback(DWORD event, HWND hwnd,
		LONG idObject, LONG idChild,
		DWORD dwEventThread, DWORD dwmsEventTime);

protected:
	virtual void ThreadProc();

protected:
	CWinEventApp();
	virtual ~CWinEventApp();

	void Loop();

	BOOL IsQuit();

private:
	BOOL m_bQuitLoop;
};

