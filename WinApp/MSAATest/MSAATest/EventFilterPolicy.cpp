#include "stdafx.h"
#include "EventFilterPolicy.h"

CEventFilterPolicy::CEventFilterPolicy()
{
}

CEventFilterPolicy::~CEventFilterPolicy()
{
}

HRESULT STDMETHODCALLTYPE CEventFilterPolicy::Init()
{
	return S_OK;
}

void STDMETHODCALLTYPE CEventFilterPolicy::UnInit()
{

}

HRESULT STDMETHODCALLTYPE CEventFilterPolicy::PolicyHandler(IPolicyObj* pPolicyObj)
{
	if (!pPolicyObj)
	{
		return E_FAIL;
	}

	// 处理事件相关过滤
	// 1、窗口事件的过滤，主要是把不必要的窗口从任务中移除
	// 2、Location事件处理

	DWORD dwEvent = GetValue<DWORD>(pPolicyObj, POLICY_INDEX_EVENT);
	HWND hWnd = (HWND)GetValue<ULONGLONG>(pPolicyObj, POLICY_INDEX_HWND);

	do
	{
		// 一些窗口的销毁动作
		if (dwEvent == EVENT_OBJECT_DESTROY ||
			dwEvent == EVENT_OBJECT_HIDE ||
			dwEvent == EVENT_SYSTEM_DIALOGEND ||
			// 菜单项的操作
			dwEvent == EVENT_SYSTEM_MENUPOPUPSTART ||
			dwEvent == EVENT_SYSTEM_MENUPOPUPEND ||
			// 输入法的操作
			dwEvent == EVENT_OBJECT_IME_HIDE ||
			dwEvent == EVENT_OBJECT_IME_SHOW ||
			dwEvent == EVENT_OBJECT_IME_CHANGE)
		{
			// 需要结束当前Group内的执行，还要结束整个Group的执行，还需要从任务队列中移除该任务
			SetValue(pPolicyObj, POLICY_INDEX_GROUP_END, TRUE);
			SetValue(pPolicyObj, POLICY_INDEX_GROUP_ITEM_END, TRUE);
			SetValue(pPolicyObj, POLICY_INDEX_TASK_REMOVE, TRUE);
			break;
		}

		// 如果是Location的操作，则需要等待静止时
		if (dwEvent == EVENT_OBJECT_LOCATIONCHANGE)
		{
			CRect rcWnd;
			rcWnd.left = GetValue<LONG>(pPolicyObj, POLICY_INDEX_X);
			rcWnd.top = GetValue<LONG>(pPolicyObj, POLICY_INDEX_Y);
			rcWnd.right = rcWnd.left + GetValue<LONG>(pPolicyObj, POLICY_INDEX_WIDTH);
			rcWnd.bottom = rcWnd.top + GetValue<LONG>(pPolicyObj, POLICY_INDEX_HEIGHT);

			CAutoCriticalSection lock(m_csForLCMap);
			if (m_mapLocationChangeCache.find(hWnd) == m_mapLocationChangeCache.end())
			{
				m_mapLocationChangeCache.insert(std::pair<HWND, CRect>(hWnd, rcWnd));
				lock.~CAutoCriticalSection();

				// 最多经过5s等待，再多了，就可能是对抗引起的
				int nTryTimes = 5;
				CRect rcLast = rcWnd;
				while (nTryTimes--)
				{
					Sleep(1000);

					GetWindowRect(hWnd, &rcWnd);
					if (rcLast == rcWnd)
					{
						break;
					}
				}

				// 记录最新的窗口大小信息
				SetValue(pPolicyObj, POLICY_INDEX_X, rcWnd.left);
				SetValue(pPolicyObj, POLICY_INDEX_Y, rcWnd.left);
				SetValue(pPolicyObj, POLICY_INDEX_WIDTH, rcWnd.Width());
				SetValue(pPolicyObj, POLICY_INDEX_HEIGHT, rcWnd.Height());
				RemoveLocationChangeCache(hWnd);

				// 因为延时了，这里需要再判断一下，窗口是否依旧有效
				if (!::IsWindow(hWnd))
				{
					SetValue(pPolicyObj, POLICY_INDEX_GROUP_END, TRUE);
					SetValue(pPolicyObj, POLICY_INDEX_GROUP_ITEM_END, TRUE);
					break;
				}
			}
			else
			{
				SetValue(pPolicyObj, POLICY_INDEX_GROUP_END, TRUE);
				SetValue(pPolicyObj, POLICY_INDEX_GROUP_ITEM_END, TRUE);
				break;
			}
		}

	} while (FALSE);

	return S_OK;
}

void CEventFilterPolicy::RemoveLocationChangeCache(HWND hWnd)
{
	CAutoCriticalSection lock(m_csForLCMap);
	for (std::map<HWND, CRect>::iterator itor = m_mapLocationChangeCache.begin(); itor != m_mapLocationChangeCache.end();)
	{
		if ((*itor).first == hWnd)
		{
			itor = m_mapLocationChangeCache.erase(itor);
		}
		else
		{
			itor++;
		}
	}
}