#include "stdafx.h"
#include "PreFilterPolicy.h"
#include "HelpApi.h"

CPreFilterPolicy::CPreFilterPolicy()
{
}

CPreFilterPolicy::~CPreFilterPolicy()
{
}

HRESULT STDMETHODCALLTYPE CPreFilterPolicy::Init()
{ 
	AddDesktopWnd();
	AddShellTrayWnd();
	return S_OK; 
}

void STDMETHODCALLTYPE CPreFilterPolicy::UnInit()
{
}

HRESULT STDMETHODCALLTYPE CPreFilterPolicy::PolicyHandler(IPolicyObj* pPolicyObj)
{
	if (!pPolicyObj)
	{
		return E_FAIL;
	}

	// 过滤规则：
	// 1、系统基本窗口过滤（e.g：任务栏、工具栏、桌面、开始菜单等、计算机等） - 基于系统固定窗口属性的	——	30分钟计算一次
	// 2、气泡等小型窗口的过滤（但是，需要有排除列表，比如一些小红包等）
	// 3、隐藏窗口、无效窗口的过滤
	// 4、窗口事件的过滤
	// 5、Location事件处理


	// 包含窗口信息获取，以及缓存表

	DWORD dwEvent = GetValue<DWORD>(pPolicyObj, POLICY_INDEX_EVENT);
	DWORD dwIdObject = GetValue<DWORD>(pPolicyObj, POLICY_INDEX_IDOBJECT);
	DWORD dwIdChild = GetValue<DWORD>(pPolicyObj, POLICY_INDEX_IDCHILD);
	HWND hWnd = (HWND)GetValue<ULONGLONG>(pPolicyObj, POLICY_INDEX_HWND);
	DWORD dwThreadID = GetValue<DWORD>(pPolicyObj, POLICY_INDEX_TID);

	BOOL bFilter = TRUE;
	do
	{
		// 不可见窗口
		if (!IsValidVisibleWnd(hWnd))
		{
			break;
		}

		// 系统基本窗口
		HWND hTopWnd = HELP_API::WND_EVENT_API::GetTopParentWnd(hWnd);
		if (IsHitSysWnd(hTopWnd))
		{
			break;
		}
		SetValue(pPolicyObj, POLICY_INDEX_TOPHWND, (ULONGLONG)hTopWnd);

		// 不合规大小或者屏幕外窗口
		CRect rcWnd;
		GetWindowRect(hWnd, &rcWnd);
		if (!IsValidRect(rcWnd))
		{
			break;
		}
		SetValue(pPolicyObj, POLICY_INDEX_X, rcWnd.left);
		SetValue(pPolicyObj, POLICY_INDEX_Y, rcWnd.left);
		SetValue(pPolicyObj, POLICY_INDEX_WIDTH, rcWnd.Width());
		SetValue(pPolicyObj, POLICY_INDEX_HEIGHT, rcWnd.Height());

		bFilter = FALSE;

	} while (FALSE);

	if (bFilter)
	{
		SetValue(pPolicyObj, POLICY_INDEX_GROUP_END, TRUE);
		SetValue(pPolicyObj, POLICY_INDEX_GROUP_ITEM_END, TRUE);
	}

	return S_OK;
}

BOOL CPreFilterPolicy::IsHitSysWnd(HWND hWnd)
{
	BOOL bHit = FALSE;

	CAutoCriticalSection lock(m_csForSysWnd);
	for (size_t i = 0; i < m_vecSysWnd.size(); i++)
	{
		if (m_vecSysWnd[i] == hWnd)
		{
			bHit = TRUE;
			break;
		}
	}
	
	return FALSE;
}

void CPreFilterPolicy::AddDesktopWnd()
{
	HWND hTopWnd = ::FindWindow(L"WorkerW", NULL);//先当WIN10系统查找
	HWND hWnd = hTopWnd;
	HWND hDeskWnd = NULL;
	while (hWnd)
	{
		HWND hShellView = ::FindWindowEx(hWnd, NULL, L"SHELLDLL_DefView", NULL);
		if (hShellView)
		{
			hDeskWnd = ::FindWindowEx(hShellView, NULL, L"SysListView32", NULL);
			break;
		}
		hWnd = ::GetWindow(hWnd, GW_HWNDNEXT);
	}

	if (hDeskWnd == NULL)
	{
		//如果没找到，再按Win7、XP方式查找
		hTopWnd = ::FindWindow(L"Progman", L"Program Manager");
		hWnd = hTopWnd;
		if (hWnd)
		{
			hWnd = ::FindWindowEx(hWnd, NULL, L"SHELLDLL_DefView", NULL);
			hDeskWnd = ::FindWindowEx(hWnd, NULL, L"SysListView32", NULL);
		}
	}

	if (NULL != hDeskWnd)
	{
		CAutoCriticalSection lock(m_csForSysWnd);
		m_vecSysWnd.push_back(hTopWnd);		// 只需要顶层窗口
	}
}

void CPreFilterPolicy::AddShellTrayWnd()
{
	HWND hShellTrayWnd = ::FindWindow(L"Shell_TrayWnd", NULL);
	HWND hWnd = hShellTrayWnd;
	HWND hMsTask = NULL;
	while (hWnd)
	{
		HWND hReBarWnd = ::FindWindowEx(hWnd, NULL, L"ReBarWindow32", NULL);
		if (hReBarWnd){
			hMsTask = ::FindWindowEx(hReBarWnd, NULL, L"MsTaskSwWClass", NULL);
		}

		hWnd = ::GetWindow(hWnd, GW_HWNDNEXT);
	}

	if (NULL != hMsTask)
	{
		CAutoCriticalSection lock(m_csForSysWnd);
		m_vecSysWnd.push_back(hShellTrayWnd);		// 只需要顶层窗口
	}
}

BOOL CPreFilterPolicy::IsValidVisibleWnd(HWND hWnd)
{
	BOOL bRet = FALSE;

	do
	{
		if (!hWnd)
		{
			break;
		}

		if (!IsWindow(hWnd))
		{
			break;
		}

		if (!IsWindowVisible(hWnd))
		{
			break;
		}

		bRet = TRUE;

	} while (FALSE);

	return bRet;
}

BOOL CPreFilterPolicy::IsValidRect(const CRect& rcWnd)
{
	BOOL bRet = FALSE;

	do
	{
		if (rcWnd.Width() < 20 || rcWnd.Height() < 20)
		{
			break;
		}

		// 包括多屏幕的情况，如果窗口区域，跑到屏幕外了，就这个情况
		int nScreenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
		int nScreenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
		int nScreenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		int nScreenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
		if (rcWnd.right <= nScreenX ||
			rcWnd.left >= nScreenX + nScreenWidth ||
			rcWnd.bottom <= nScreenY ||
			rcWnd.top >= nScreenY + nScreenHeight)
		{
			break;
		}

		bRet = TRUE;

	} while (FALSE);

	return bRet;
}