#pragma once
#include "WinEventHook.h"

class CWinEventApp
	: public IWinEventHookCallback
{
public:
	CWinEventApp();
	~CWinEventApp();

	virtual void OnWinEventCallback(DWORD event, HWND hwnd,
		LONG idObject, LONG idChild,
		DWORD dwEventThread, DWORD dwmsEventTime);
};

