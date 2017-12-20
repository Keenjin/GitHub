#include "stdafx.h"
#include "PolicySchedule.h"

CPolicySchedule::CPolicySchedule()
{
}

CPolicySchedule::~CPolicySchedule()
{
}

HRESULT CPolicySchedule::Init()
{
	HRESULT hr = E_FAIL;
	do
	{
		if (!Load())
		{
			break;
		}

		hr = S_OK;

	} while (FALSE);
	return hr;
}

void CPolicySchedule::UnInit()
{
	UnLoad();
}

void CPolicySchedule::PolicyGroupHandler(UINT uIndex, CComPtr<IPolicyObj> pObj, BOOL& bFinish)
{
	CAtlString strGuid = GetPolicyItemGuid(uIndex, 0);
	if (!strGuid.IsEmpty())
	{
		CComPtr<CPolicyBase> pPolicy = GetPolicy(strGuid);
	}
}
