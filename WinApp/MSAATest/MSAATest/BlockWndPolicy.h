#pragma once
#include "PolicyBase.h"

class CBlockWndPolicy
	: public CPolicyBase
{
public:
	CBlockWndPolicy();
	~CBlockWndPolicy();

	virtual HRESULT STDMETHODCALLTYPE Init() { return S_OK; };
	virtual void STDMETHODCALLTYPE UnInit() {};
	virtual HRESULT STDMETHODCALLTYPE PolicyHandler(IPolicyObj* pPolicyObj);

private:

};

