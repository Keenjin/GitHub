#include "stdafx.h"
#include "TaskContainerImpl.h"

CTaskContainerImpl::CTaskContainerImpl()
{

}

CTaskContainerImpl::~CTaskContainerImpl()
{

}

void CTaskContainerImpl::RemoveWnd(HWND hWnd)
{
	CAutoCriticalSection lock(m_csTaskObjs);
	for (std::list<CComPtr<IPolicyObj>>::iterator itor = m_qTaskObjs.begin(); itor != m_qTaskObjs.end();)
	{
		if (hWnd == (HWND)GetValue<ULONGLONG>(*itor, POLICY_INDEX_HWND))
		{
			itor = m_qTaskObjs.erase(itor);
		}
		else
		{
			itor++;
		}
	}
}