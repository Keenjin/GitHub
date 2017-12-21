#pragma once
#include "PolicyBase.h"
#include <map>
#include "AutoCriticalSection.h"

class CEventFilterPolicy
	: public CPolicyBase
{
public:
	CEventFilterPolicy();
	~CEventFilterPolicy();

	virtual HRESULT STDMETHODCALLTYPE Init();
	virtual void STDMETHODCALLTYPE UnInit();
	virtual HRESULT STDMETHODCALLTYPE PolicyHandler(IPolicyObj* pPolicyObj);

private:
	void RemoveLocationChangeCache(HWND hWnd);

private:
	ATL::CComAutoCriticalSection	m_csForLCMap;
	std::map<HWND, CRect>	m_mapLocationChangeCache;
};

