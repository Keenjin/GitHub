#include "stdafx.h"
#include "BlockWndPolicy.h"

CBlockWndPolicy::CBlockWndPolicy()
{
}

CBlockWndPolicy::~CBlockWndPolicy()
{
}

HRESULT STDMETHODCALLTYPE CBlockWndPolicy::PolicyHandler(IPolicyObj* pPolicyObj)
{
	return S_OK;
}