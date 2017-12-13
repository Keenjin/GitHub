#pragma once
#include "PolicyObj.h"
#include "PolicySchedule.h"

class CPolicyMgr
{
public:
	static CPolicyMgr& Instance();

	HRESULT Init();
	void UnInit();

	HRESULT Fire(DWORD event, HWND hwnd,
		LONG idObject, LONG idChild,
		DWORD dwEventThread);

	void OnHandlePolicy(CComPtr<IPolicyObj> pObj);

private:
	CPolicyMgr();
	virtual ~CPolicyMgr();

	BOOL IsEventOfWindow(DWORD event, HWND hwnd,
		LONG idObject, LONG idChild,
		DWORD dwEventThread);

private:
	CPolicySchedule<CPolicyMgr, CComPtr<IPolicyObj>>	m_PolicySched;
};

