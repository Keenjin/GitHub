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
		if (FAILED(m_PolicySched.Create(this, &CPolicyMgr::OnHandlePolicy)))
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

	m_PolicySched.Destroy();
}

BOOL CPolicyMgr::IsEventOfWindow(DWORD event, HWND hwnd,
	LONG idObject, LONG idChild,
	DWORD dwEventThread)
{
	if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || NULL == hwnd)
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

	LOG_PRINT(L"Enter %s, event(%s),hwnd(0x%0x),idObject(%s),idChild(%s),dwEventThread(%d),CurrentThread(%d)", 
		__FUNCTIONW__, 
		HELP_API::WND_EVENT_API::WndEventName(event),
		hwnd,
		HELP_API::WND_EVENT_API::CtrlObjectName(idObject),
		HELP_API::WND_EVENT_API::ChildIDName(idChild),
		dwEventThread,
		GetCurrentThreadId());

	CComPtr<IPolicyObj> pObj;
	if (FAILED(CPolicyObj::Create(&pObj)))
	{
		return E_FAIL;
	}

	pObj->SetParam(POLICY_INDEX_EVENT, CComVariant((unsigned int)event));
	pObj->SetParam(POLICY_INDEX_HWND, CComVariant((ULONGLONG)hwnd));
	pObj->SetParam(POLICY_INDEX_IDOBJECT, CComVariant(idObject));
	pObj->SetParam(POLICY_INDEX_IDCHILD, CComVariant(idChild));
	pObj->SetParam(POLICY_INDEX_TID, CComVariant((unsigned int)dwEventThread));

	// 往任务队列里面塞任务
	m_PolicySched.AddTask(pObj);

	return S_OK;
}

void CPolicyMgr::OnHandlePolicy(CComPtr<IPolicyObj> pObj)
{
	if (!pObj)
	{
		return;
	}

	DWORD dwEvent = GetValue<DWORD>(pObj, POLICY_INDEX_EVENT);
	DWORD dwIdObject = GetValue<DWORD>(pObj, POLICY_INDEX_IDOBJECT);
	DWORD dwIdChild = GetValue<DWORD>(pObj, POLICY_INDEX_IDCHILD);
	HWND hWnd = (HWND)GetValue<ULONGLONG>(pObj, POLICY_INDEX_HWND);
	DWORD dwThreadID = GetValue<DWORD>(pObj, POLICY_INDEX_TID);

	LOG_PRINT(L"Enter %s, event(%s),hwnd(0x%0x),idObject(%s),idChild(%s),dwEventThread(%d),CurrentThread(%d)",
		__FUNCTIONW__,
		HELP_API::WND_EVENT_API::WndEventName(dwEvent),
		hWnd,
		HELP_API::WND_EVENT_API::CtrlObjectName(dwIdObject),
		HELP_API::WND_EVENT_API::ChildIDName(dwIdChild),
		dwThreadID,
		GetCurrentThreadId());
}