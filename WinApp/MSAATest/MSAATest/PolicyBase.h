#pragma once
#include "PolicyObj.h"
#include "IUnknownImpl.h"

class CPolicyBase
	: public IUnknownImplT<IUnknown>
{
public:
	virtual HRESULT STDMETHODCALLTYPE Init() { return S_OK; };
	virtual void STDMETHODCALLTYPE UnInit() {};
	virtual HRESULT STDMETHODCALLTYPE PolicyHandler(IPolicyObj* pPolicyObj) { return S_OK; };
};

