#include "stdafx.h"
#include "EventFilterPolicy.h"

CEventFilterPolicy::CEventFilterPolicy()
{
}

CEventFilterPolicy::~CEventFilterPolicy()
{
}

HRESULT STDMETHODCALLTYPE CEventFilterPolicy::Init()
{
	return S_OK;
}

void STDMETHODCALLTYPE CEventFilterPolicy::UnInit()
{

}

HRESULT STDMETHODCALLTYPE CEventFilterPolicy::PolicyHandler(IPolicyObj* pPolicyObj)
{
	return S_OK;
}