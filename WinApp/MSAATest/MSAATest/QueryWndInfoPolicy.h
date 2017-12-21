#pragma once
#include "PolicyBase.h"

class CQueryWndInfoPolicy
	: public CPolicyBase
{
public:
	CQueryWndInfoPolicy();
	~CQueryWndInfoPolicy();

	virtual HRESULT STDMETHODCALLTYPE Init();
	virtual void STDMETHODCALLTYPE UnInit();
	virtual HRESULT STDMETHODCALLTYPE PolicyHandler(IPolicyObj* pPolicyObj);

private:
	BOOL GetProcCmdline(HANDLE hProcess, CAtlString& strCmdline, LPDWORD lpParentPID = NULL);
	BOOL DeviceDosPathToNTPath(CAtlString& strPath);

private:
	HANDLE	m_hToken;
};
