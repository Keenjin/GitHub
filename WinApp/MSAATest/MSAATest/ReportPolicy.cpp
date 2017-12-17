#include "stdafx.h"
#include "ReportPolicy.h"
#include "HelpApi.h"
using namespace HELP_API::WND_EVENT_API;
using namespace HELP_API::PROCESS_THREAD_API;


CReportPolicy::CReportPolicy()
{
}

CReportPolicy::~CReportPolicy()
{
}


HRESULT STDMETHODCALLTYPE CReportPolicy::PolicyHandler(IPolicyObj* pPolicyObj)
{
	if (!pPolicyObj)
	{
		return E_INVALIDARG;
	}

	DWORD dwEvent = GetValue<DWORD>(pPolicyObj, POLICY_INDEX_EVENT);
	DWORD dwIdObject = GetValue<DWORD>(pPolicyObj, POLICY_INDEX_IDOBJECT);
	DWORD dwIdChild = GetValue<DWORD>(pPolicyObj, POLICY_INDEX_IDCHILD);
	HWND hWnd = (HWND)GetValue<ULONGLONG>(pPolicyObj, POLICY_INDEX_HWND);
	DWORD dwThreadID = GetValue<DWORD>(pPolicyObj, POLICY_INDEX_TID);

	CAtlString strWndText;
	GetWindowText(GetTopParentWnd(hWnd), strWndText.GetBufferSetLength(1024), 1023);
	strWndText.ReleaseBuffer();
	if (strWndText.MakeLower().Find(L"debugview") != -1)
	{
		return E_FAIL;
	}

	//GetProcessIDFromName(L"dbgview.exe");

	/*if (GetProcessIDFromName(L"dbgview.exe") == GetProcessIDFromThreadID(dwEventThread))
	{
	return;
	}*/

	//return;

	IAccessible* pAcc = NULL;
	VARIANT varChild;
	HRESULT hr = AccessibleObjectFromEvent(hWnd, dwIdObject, dwIdChild, &pAcc, &varChild);
	if ((hr == S_OK) && (pAcc != NULL))
	{
		long lChildCount, lIDTopic;
		CComBSTR bstrName, bstrValue, bstrDesc, bstrHelp, bstrHelpTopic, bstrKeyboardShortcut, bstrDefAction;
		CComVariant varRole, varState;
		CAtlString strRoleText, strStateText;
		pAcc->get_accName(varChild, &bstrName);
		pAcc->get_accValue(varChild, &bstrValue);
		pAcc->get_accDescription(varChild, &bstrDesc);
		pAcc->get_accHelp(varChild, &bstrHelp);
		pAcc->get_accHelpTopic(&bstrHelpTopic, varChild, &lIDTopic);
		pAcc->get_accKeyboardShortcut(varChild, &bstrKeyboardShortcut);
		pAcc->get_accDefaultAction(varChild, &bstrDefAction);
		pAcc->get_accChildCount(&lChildCount);
		pAcc->get_accRole(varChild, &varRole);
		::GetRoleText(varRole.lVal, strRoleText.GetBufferSetLength(1024), 1023);
		strRoleText.ReleaseBuffer();
		pAcc->get_accState(varChild, &varState);
		::GetStateText(varState.lVal, strStateText.GetBufferSetLength(1024), 1023);
		strStateText.ReleaseBuffer();

		CAtlString strLog;
		strLog.Format(L"%s, event -- Event(%s),hWnd(0x%0x),idObject(%s),idChild(%s),EventThreadID(%d),CurrentThreadID(%d)",
			__FUNCTIONW__, WndEventName(dwEvent), hWnd, CtrlObjectName(dwIdObject), ChildIDName(dwIdChild), dwThreadID, GetCurrentThreadId());
		OutputDebugString(strLog);
		strLog.Format(L"%s, desc  -- ChildCount(%d),Name(%s),Role(%s),State(%s),Value(%s),Desc(%s),Help(%s),HelpTopic(%s),TopicID(%d),KeyboardShortcut(%s),defAction(%s)",
			__FUNCTIONW__, lChildCount, bstrName, strRoleText, strStateText, bstrValue, bstrDesc, bstrHelp, bstrHelpTopic, lIDTopic, bstrKeyboardShortcut, bstrDefAction);
		OutputDebugString(strLog);

		pAcc->Release();
	}

	return hr;
}