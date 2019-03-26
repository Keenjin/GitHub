#pragma once
class IWinEventHookCallback
{
public:
	virtual void OnWinEventCallback(DWORD event, HWND hwnd,
		LONG idObject, LONG idChild,
		DWORD dwEventThread, DWORD dwmsEventTime) = 0;
};

class CWinEventHook
{
public:
	static CWinEventHook& Instance();

	HRESULT Hook(IWinEventHookCallback* pCallback);
	void UnHook();

public:
	static void CALLBACK HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
		LONG idObject, LONG idChild,
		DWORD dwEventThread, DWORD dwmsEventTime);

	virtual void OnWinEventCallback(DWORD event, HWND hwnd,
		LONG idObject, LONG idChild,
		DWORD dwEventThread, DWORD dwmsEventTime);

private:
	CWinEventHook();
	virtual ~CWinEventHook();

protected:
	BOOL	m_bInitCOM;
	HWINEVENTHOOK	m_hWinEventHook;
	IWinEventHookCallback*	m_pCallback;
};

