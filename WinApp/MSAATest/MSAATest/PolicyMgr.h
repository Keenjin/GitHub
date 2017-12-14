#pragma once
#include "PolicyObj.h"
#include "TaskPool.h"
#include "TaskContainerImpl.h"
#include "CpuSmooth.h"
#include "PolicyBase.h"
#include "PolicySchedule.h"

class CPolicyMgr
{
public:
	static CPolicyMgr& Instance();

	HRESULT Init();
	void UnInit();

	HRESULT Fire(DWORD event, HWND hwnd,
		LONG idObject, LONG idChild,
		DWORD dwEventThread);

	void OnHandlePolicy(CComPtr<IPolicyObj> pObj);

private:
	CPolicyMgr();
	virtual ~CPolicyMgr();

	BOOL IsEventOfWindow(DWORD event, HWND hwnd,
		LONG idObject, LONG idChild,
		DWORD dwEventThread);

private:
	CTaskPool<CPolicyMgr, CComPtr<IPolicyObj>, CTaskContainerImpl>	m_TaskPool;
	CTaskContainerImpl	m_TaskContainer;
	CCpuSmooth	m_CpuSmooth;
	CPolicySchedule	m_PolicySched;
};

