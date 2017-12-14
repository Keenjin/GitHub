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
	return S_OK;
}

void CPolicySchedule::UnInit()
{
	CAutoCriticalSection lock(m_csForPolicyInst);
	for (std::vector<CComPtr<CPolicyBase>>::iterator itor = m_vecPolicyInst.begin();
		 itor != m_vecPolicyInst.end(); itor++)
	{
		(*itor)->UnInit();
	}

	m_vecPolicyInst.clear();
}

CPolicyBase* CPolicySchedule::GetPolicy(UINT uIndex)
{
	CAutoCriticalSection lock(m_csForPolicyInst);
	if (uIndex < m_vecPolicyInst.size())
	{
		return m_vecPolicyInst[uIndex];
	}
	return NULL;
}