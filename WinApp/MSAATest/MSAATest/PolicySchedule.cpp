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

void CPolicySchedule::PolicyGroupHandler(UINT uIndex, CComPtr<IPolicyObj> pObj)
{
	if (!pObj)
	{
		return;
	}

	// 当前走到的分组ID
	SetValue(pObj, POLICY_INDEX_GROUP_CURRENT_GID, GetGroupID(uIndex));

	for (size_t i = 0; i < GetPolicyItemCount(uIndex); i++)
	{
		if (GetValue<BOOL>(pObj, POLICY_INDEX_TASK_REMOVE))
		{
			break;
		}

		CAtlString strGuid = GetPolicyItemGuid(uIndex, i);
		CComPtr<CPolicyBase> pPolicy = GetPolicy(strGuid);
		if (!pPolicy)
		{
			break;
		}
		pPolicy->PolicyHandler(pObj);
		
		if (GetValue<BOOL>(pObj, POLICY_INDEX_GROUP_ITEM_END))
		{
			break;
		}
	}

	SetValue(pObj, POLICY_INDEX_GROUP_ITEM_END, FALSE);		// 清除标记位
}
