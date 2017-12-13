#pragma once
#include "PolicyObj.h"

class CPolicyBase
{
public:
	virtual HRESULT STDMETHODCALLTYPE Init() { return S_OK; };
	virtual void STDMETHODCALLTYPE UnInit() {};
	virtual HRESULT STDMETHODCALLTYPE PolicyHandler(IPolicyObj* pPolicyObj) { return S_OK; };
};

