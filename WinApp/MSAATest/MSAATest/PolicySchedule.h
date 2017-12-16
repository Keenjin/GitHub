#pragma once
#include "AutoCriticalSection.h"
#include <vector>
#include "PolicyConfig.h"

class CPolicySchedule
	: public CPolicyConfig
{
public:
	CPolicySchedule();
	~CPolicySchedule();

	HRESULT Init();
	void UnInit();

	CPolicyBase* GetFirstPolicy(UINT uIndex, CComPtr<IPolicyObj> pObj);
	CPolicyBase* GetNextPolicy(CComPtr<IPolicyObj> pObj);

private:
	UINT	m_uPolicyGroupIndex;
	UINT	m_uPolicyItemIndex;
};

