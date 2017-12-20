#pragma once
#include "PolicyBase.h"

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

};

