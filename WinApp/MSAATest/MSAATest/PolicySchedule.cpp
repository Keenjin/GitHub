#include "stdafx.h"
#include "PolicySchedule.h"

CPolicySchedule::CPolicySchedule()
	: m_uPolicyItemIndex(0)
	, m_uPolicyGroupIndex(0)
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

CPolicyBase* CPolicySchedule::GetFirstPolicy(UINT uIndex, CComPtr<IPolicyObj> pObj)
{
	// pObj可以改变下一个执行者
	// Todo


	CComPtr<CPolicyBase> pPolicy;
	do
	{
		m_uPolicyGroupIndex = uIndex;
		m_uPolicyItemIndex = 0;
		CAtlString strGuid = GetPolicyItemGuid(m_uPolicyGroupIndex, m_uPolicyItemIndex);
		if (strGuid.IsEmpty())
		{
			break;
		}
		pPolicy = GetPolicy(strGuid);

	} while (FALSE);
	return pPolicy;
}

CPolicyBase* CPolicySchedule::GetNextPolicy(CComPtr<IPolicyObj> pObj)
{
	// pObj可以改变下一个执行者
	// Todo


	// 如果pObj里面，有改变顺序的值，则以pObj为准；如果没有，则以默认顺序为准
	m_uPolicyItemIndex++;

	CComPtr<CPolicyBase> pPolicy;
	do
	{
		CAtlString strGuid = GetPolicyItemGuid(m_uPolicyGroupIndex, m_uPolicyItemIndex);
		if (strGuid.IsEmpty())
		{
			break;
		}
		pPolicy = GetPolicy(strGuid);

	} while (FALSE);
	return pPolicy;
}
