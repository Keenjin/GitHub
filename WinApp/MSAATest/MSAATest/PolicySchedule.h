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

	void PolicyGroupHandler(UINT uIndex, CComPtr<IPolicyObj> pObj);
};

