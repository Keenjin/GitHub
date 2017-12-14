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

	// ����xml����ͬ����
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
	// ��ǰ�ᣬ���Ǵ��ڲ���
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

	// ����ѻ�����CPUʹ���ʱȽϸߣ���Ҫ��ƽ������
	if (m_TaskContainer.GetCount() > 1000 || m_CpuSmooth.IsProcCpuHigh(NULL, 5))
	{
		m_CpuSmooth.Sleep(100);
	}

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

	// �������������������
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

	for (;;)
	{
		// ���ò��ԣ�����ִ����ȥ��ֱ�������ɣ�����ѭ��
		CPolicyBase* pPolicyHandle = GetNextPolicy(pObj);
		if (!pPolicyHandle)
		{
			break;
		}
		pPolicyHandle->PolicyHandler(pObj);
	}
}