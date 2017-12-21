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

	CRect rcWnd;
	rcWnd.left = GetValue<LONG>(pPolicyObj, POLICY_INDEX_X);
	rcWnd.top = GetValue<LONG>(pPolicyObj, POLICY_INDEX_Y);
	rcWnd.right = rcWnd.left + GetValue<LONG>(pPolicyObj, POLICY_INDEX_WIDTH);
	rcWnd.bottom = rcWnd.top + GetValue<LONG>(pPolicyObj, POLICY_INDEX_HEIGHT);

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
			// 对于LocationChange事件，先看本地缓存有没有；如果没有，则添加到缓存，同时，将该窗口回抛；如果有，则与本地缓存的位置比对，当位置相同的时候，则说明位置确实不动了，清空队列里重复窗口，继续走下一步
			CAutoCriticalSection lock(m_csForLCMap);
			if (m_mapLocationChangeCache.find(hWnd) == m_mapLocationChangeCache.end())
			{
				m_mapLocationChangeCache.insert(std::pair<HWND, CRect>(hWnd, rcWnd));

				SetValue(pPolicyObj, POLICY_INDEX_GROUP_END, TRUE);
				SetValue(pPolicyObj, POLICY_INDEX_GROUP_ITEM_END, TRUE);
				SetValue(pPolicyObj, POLICY_INDEX_TASK_ADD, TRUE);
				SetValue(pPolicyObj, POLICY_INDEX_LAST_LOCATION_CHANGE, TRUE);
				break;
			}
			else
			{
				CRect rcLast = m_mapLocationChangeCache[hWnd];
				lock.~CAutoCriticalSection();		// 释放锁，防止死锁

				BOOL bStop = FALSE;

				// 如果是上一个LocationChange事件，说明是我们自己回抛的，就要延时一会儿查看是否有变化；如果不是上一个，说明是新产生的，直接比对位置
				if (GetValue<BOOL>(pPolicyObj, POLICY_INDEX_LAST_LOCATION_CHANGE))
				{
					Sleep(10);		// 目前先只延迟10ms，看看是否变化

					CRect rcNowWnd;
					GetWindowRect(hWnd, &rcNowWnd);
					if (rcNowWnd == rcLast)
					{
						// 说明已经静止了
						bStop = TRUE;
					}
				}
				else
				{
					if (rcWnd == rcLast)
					{
						// 说明已经静止了
						bStop = TRUE;
					}
				}

				if (!bStop)
				{
					// 还没有静止，则继续回抛
					SetValue(pPolicyObj, POLICY_INDEX_GROUP_END, TRUE);
					SetValue(pPolicyObj, POLICY_INDEX_GROUP_ITEM_END, TRUE);
					SetValue(pPolicyObj, POLICY_INDEX_TASK_ADD, TRUE);
					SetValue(pPolicyObj, POLICY_INDEX_LAST_LOCATION_CHANGE, TRUE);
				}
				else
				{
					// 已经静止了，则清空无用队列
					SetValue(pPolicyObj, POLICY_INDEX_TASK_REMOVE, TRUE);
					RemoveLocationChangeCache(hWnd);
				}
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