#include "stdafx.h"
#include "PolicyMgr.h"
#include "PolicyObjImpl.h"
#include "HelpApi.h"

CPolicyMgr::CPolicyMgr()
{
}

CPolicyMgr::~CPolicyMgr()
{
}

CPolicyMgr& CPolicyMgr::Instance()
{
	static CPolicyMgr theObj;
	return theObj;
}

HRESULT CPolicyMgr::Init()
{
	LOG_PRINT(L"Enter %s, CurrentThreadID(%d)", __FUNCTIONW__, GetCurrentThreadId());

	// 根据xml，起不同策略
	HRESULT hr = E_FAIL;

	do
	{
		if (FAILED(m_TaskPool.Create(this, &CPolicyMgr::OnHandlePolicy, &m_TaskContainer)))
		{
			break;
		}

		if (FAILED(m_PolicySched.Init()))
		{
			break;
		}

		hr = S_OK;

	} while (FALSE);

	if (FAILED(hr))
	{
		UnInit();
	}

	return hr;
}

void CPolicyMgr::UnInit()
{
	LOG_PRINT(L"Enter %s", __FUNCTIONW__);
	m_PolicySched.UnInit();
	m_TaskPool.Destroy();
}

BOOL CPolicyMgr::IsEventOfWindow(DWORD event, HWND hwnd,
	LONG idObject, LONG idChild,
	DWORD dwEventThread)
{
	if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || NULL == hwnd || !::IsWindow(hwnd))
	{
		return FALSE;
	}

	return TRUE;
}

HRESULT CPolicyMgr::Fire(DWORD event, HWND hwnd,
	LONG idObject, LONG idChild,
	DWORD dwEventThread)
{
	// 大前提，得是窗口才行
	if (!IsEventOfWindow(event, hwnd, idObject, idChild, dwEventThread))
	{
		return E_FAIL;
	}

	/*LOG_PRINT(L"Enter %s, event(%s),hwnd(0x%0x),idObject(%s),idChild(%s),dwEventThread(%d),CurrentThread(%d)", 
		__FUNCTIONW__, 
		HELP_API::WND_EVENT_API::WndEventName(event),
		hwnd,
		HELP_API::WND_EVENT_API::CtrlObjectName(idObject),
		HELP_API::WND_EVENT_API::ChildIDName(idChild),
		dwEventThread,
		GetCurrentThreadId());*/

	// 任务堆积，或CPU使用率比较高，需要做平滑处理
	if (m_TaskContainer.GetCount() > 1000 || m_CpuSmooth.IsProcCpuHigh(NULL, 5))
	{
		m_CpuSmooth.Sleep(100);
	}

	CComPtr<IPolicyObj> pObj;
	if (FAILED(CPolicyObj::Create(&pObj)))
	{
		return E_FAIL;
	}

	SetValue(pObj, POLICY_INDEX_EVENT, event);
	SetValue(pObj, POLICY_INDEX_HWND, (ULONGLONG)hwnd);
	SetValue(pObj, POLICY_INDEX_IDOBJECT, idObject);
	SetValue(pObj, POLICY_INDEX_IDCHILD, idChild);
	SetValue(pObj, POLICY_INDEX_TID, dwEventThread);

	// 往任务队列里面塞任务
	m_TaskPool.AddTask(pObj);

	LOG_PRINT(L"%s, taskcnt(%d)", __FUNCTIONW__, m_TaskContainer.GetCount());

	return S_OK;
}

void CPolicyMgr::OnHandlePolicy(CComPtr<IPolicyObj> pObj)
{
	if (!pObj)
	{
		return;
	}

	DWORD dwEvent = GetValue<DWORD>(pObj, POLICY_INDEX_EVENT);
	DWORD dwIdObject = GetValue<LONG>(pObj, POLICY_INDEX_IDOBJECT);
	DWORD dwIdChild = GetValue<LONG>(pObj, POLICY_INDEX_IDCHILD);
	HWND hWnd = (HWND)GetValue<ULONGLONG>(pObj, POLICY_INDEX_HWND);
	DWORD dwThreadID = GetValue<DWORD>(pObj, POLICY_INDEX_TID);

	LOG_PRINT(L"Enter %s, event(%s),hwnd(0x%0x),idObject(%s),idChild(%s),dwEventThread(%d),CurrentThread(%d), taskcnt(%d)",
		__FUNCTIONW__,
		HELP_API::WND_EVENT_API::WndEventName(dwEvent),
		hWnd,
		HELP_API::WND_EVENT_API::CtrlObjectName(dwIdObject),
		HELP_API::WND_EVENT_API::ChildIDName(dwIdChild),
		dwThreadID,
		GetCurrentThreadId(), m_TaskContainer.GetCount());

	for (size_t i = 0; i < m_PolicySched.GetPolicyGroupCnt(); i++)
	{
		if (m_TaskContainer.GetFlag(hWnd) == TASK_OBJ_FLAG_REMOVE)
		{
			break;
		}

		// 按照分组，依次执行下去
		m_PolicySched.PolicyGroupHandler(i, pObj);
		if (GetValue<BOOL>(pObj, POLICY_INDEX_TASK_REMOVE))
		{
			// 移除任务
			m_TaskContainer.RemoveWnd(hWnd);
			break;
		}
		if (GetValue<BOOL>(pObj, POLICY_INDEX_GROUP_END))
		{
			break;
		}
	}
}