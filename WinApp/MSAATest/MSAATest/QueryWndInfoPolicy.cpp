#include "stdafx.h"
#include "QueryWndInfoPolicy.h"

CQueryWndInfoPolicy::CQueryWndInfoPolicy()
{
}

CQueryWndInfoPolicy::~CQueryWndInfoPolicy()
{
}

HRESULT STDMETHODCALLTYPE CQueryWndInfoPolicy::Init()
{
	return S_OK;
}

void STDMETHODCALLTYPE CQueryWndInfoPolicy::UnInit()
{

}

HRESULT STDMETHODCALLTYPE CQueryWndInfoPolicy::PolicyHandler(IPolicyObj* pPolicyObj)
{
	return S_OK;
}