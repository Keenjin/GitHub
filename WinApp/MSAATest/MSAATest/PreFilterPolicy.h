#pragma once
#include "PolicyBase.h"
#include <vector>
#include "AutoCriticalSection.h"

class CPreFilterPolicy
	: public CPolicyBase
{
public:
	CPreFilterPolicy();
	~CPreFilterPolicy();

	virtual HRESULT STDMETHODCALLTYPE Init();
	virtual void STDMETHODCALLTYPE UnInit();
	virtual HRESULT STDMETHODCALLTYPE PolicyHandler(CComPtr<IPolicyObj> pPolicyObj);

private:
	BOOL IsHitSysWnd(HWND hWnd);
	void AddDesktopWnd();
	void AddShellTrayWnd();

	BOOL IsValidVisibleWnd(HWND hWnd);
	BOOL IsValidRect(const CRect& rcWnd);

private:
	CComAutoCriticalSection	m_csForSysWnd;
	std::vector<HWND>	m_vecSysWnd;
};

