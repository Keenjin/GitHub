#include "stdafx.h"
#include "TaskContainerImpl.h"

CTaskContainerImpl::CTaskContainerImpl()
{

}

CTaskContainerImpl::~CTaskContainerImpl()
{

}

// ����remove���λ������ǧ��Ҫֱ�Ӳ���������У���Ϊ���п�����ɶ��߳�����
void CTaskContainerImpl::RemoveWnd(HWND hWnd)
{
	CAutoCriticalSection lock(m_csTaskObjs);
	for (std::list<CTaskContainerImpl::VALUE>::iterator itor = m_qTaskObjs.begin(); itor != m_qTaskObjs.end(); itor++)
	{
		if (hWnd == (HWND)GetValue<ULONGLONG>((*itor).obj, POLICY_INDEX_HWND))
		{
			(*itor).dwFlag = TASK_OBJ_FLAG_REMOVE;
		}
	}
}

DWORD CTaskContainerImpl::GetFlag(HWND hWnd)
{
	DWORD dwFlag = TASK_OBJ_FLAG_NONE;

	CAutoCriticalSection lock(m_csTaskObjs);
	for (std::list<CTaskContainerImpl::VALUE>::iterator itor = m_qTaskObjs.begin(); itor != m_qTaskObjs.end(); itor++)
	{
		if (hWnd == (HWND)GetValue<ULONGLONG>((*itor).obj, POLICY_INDEX_HWND))
		{
			dwFlag = (*itor).dwFlag;
		}
	}

	return dwFlag;
}