#pragma once
#include "PolicyBase.h"
#include "AutoCriticalSection.h"
#include <vector>
#include "PolicyConfig.h"

class CPolicySchedule
{
public:
	CPolicySchedule();
	~CPolicySchedule();

	HRESULT Init();
	void UnInit();

	CPolicyBase* GetPolicy(UINT uIndex);

public:
	template<typename T>
	static HRESULT Create(CPolicyBase** ppPolicy)
	{
		HRESULT hr = E_FAIL;

		do
		{
			if (!ppPolicy)
			{
				break;
			}

			*ppPolicy = new T;
			if (!(*ppPolicy))
			{
				break;
			}

			(*ppPolicy)->AddRef();

			if (FAILED((*ppPolicy)->Init()))
			{
				break;
			}

			hr = S_OK;

		} while (FALSE);
		
		return hr;
	}

private:
	ATL::CComAutoCriticalSection m_csForPolicyInst;
	std::vector<CComPtr<CPolicyBase>>	m_vecPolicyInst;

	CPolicyConfig	m_PolicyCfg;
};

