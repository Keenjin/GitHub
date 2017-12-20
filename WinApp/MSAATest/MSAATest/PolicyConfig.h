#pragma once
#include <vector>
#include "atlstr.h"
#include "AutoCriticalSection.h"
#include "PolicyFactory.h"

class CPolicyConfig
	: public CPolicyFactory
{
public:
	CPolicyConfig();
	~CPolicyConfig();

	BOOL Load();
	void UnLoad();

	UINT GetPolicyGroupCnt();
	UINT GetPolicyItemCount(UINT uIndex);
	UINT GetGroupID(UINT uIndex);
	CAtlString GetPolicyItemGuid(UINT uGroupIndex, UINT uItemIndex);

protected:
	DWORD	m_dwVersion;

	typedef struct _POLICY_ITEM
	{
		ATL::CAtlString strName;
		ATL::CAtlString strGuid;
	}POLICY_ITEM, *PPOLICY_ITEM;

	typedef struct _POLICY_GROUP
	{
		UINT	uGID;
		ATL::CAtlString strName;
		std::vector<POLICY_ITEM> vecPolicyItems;
		_POLICY_GROUP() : uGID(0) {}
	}POLICY_GROUP, *PPOLICY_GROUP;
	ATL::CComAutoCriticalSection m_csForPolicyCfg;
	std::vector<POLICY_GROUP>	m_vecPolicyCfg;
};

