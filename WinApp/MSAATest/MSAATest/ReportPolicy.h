#pragma once
#include "PolicyBase.h"

class CReportPolicy
	: public CPolicyBase
{
public:
	CReportPolicy();
	~CReportPolicy();

	virtual HRESULT STDMETHODCALLTYPE Init() { return S_OK; };
	virtual void STDMETHODCALLTYPE UnInit() {};
	virtual HRESULT STDMETHODCALLTYPE PolicyHandler(IPolicyObj* pPolicyObj);
};

