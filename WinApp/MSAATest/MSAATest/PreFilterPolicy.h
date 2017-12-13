#pragma once
#include "PolicyBase.h"

class CPreFilterPolicy
	: public CPolicyBase
{
public:
	CPreFilterPolicy();
	~CPreFilterPolicy();

	virtual HRESULT STDMETHODCALLTYPE Init() { return S_OK; };
	virtual void STDMETHODCALLTYPE UnInit() {};
	virtual HRESULT STDMETHODCALLTYPE PolicyHandler(IPolicyObj* pPolicyObj);
};

