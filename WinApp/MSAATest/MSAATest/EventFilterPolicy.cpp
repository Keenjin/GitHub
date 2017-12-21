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

	// �����¼���ع���
	// 1�������¼��Ĺ��ˣ���Ҫ�ǰѲ���Ҫ�Ĵ��ڴ��������Ƴ�
	// 2��Location�¼�����

	DWORD dwEvent = GetValue<DWORD>(pPolicyObj, POLICY_INDEX_EVENT);
	HWND hWnd = (HWND)GetValue<ULONGLONG>(pPolicyObj, POLICY_INDEX_HWND);

	do
	{
		// һЩ���ڵ����ٶ���
		if (dwEvent == EVENT_OBJECT_DESTROY ||
			dwEvent == EVENT_OBJECT_HIDE ||
			dwEvent == EVENT_SYSTEM_DIALOGEND ||
			// �˵���Ĳ���
			dwEvent == EVENT_SYSTEM_MENUPOPUPSTART ||
			dwEvent == EVENT_SYSTEM_MENUPOPUPEND ||
			// ���뷨�Ĳ���
			dwEvent == EVENT_OBJECT_IME_HIDE ||
			dwEvent == EVENT_OBJECT_IME_SHOW ||
			dwEvent == EVENT_OBJECT_IME_CHANGE)
		{
			// ��Ҫ������ǰGroup�ڵ�ִ�У���Ҫ��������Group��ִ�У�����Ҫ������������Ƴ�������
			SetValue(pPolicyObj, POLICY_INDEX_GROUP_END, TRUE);
			SetValue(pPolicyObj, POLICY_INDEX_GROUP_ITEM_END, TRUE);
			SetValue(pPolicyObj, POLICY_INDEX_TASK_REMOVE, TRUE);
			break;
		}

		// �����Location�Ĳ���������Ҫ�ȴ���ֹʱ
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

				// ��ྭ��5s�ȴ����ٶ��ˣ��Ϳ����ǶԿ������
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

				// ��¼���µĴ��ڴ�С��Ϣ
				SetValue(pPolicyObj, POLICY_INDEX_X, rcWnd.left);
				SetValue(pPolicyObj, POLICY_INDEX_Y, rcWnd.left);
				SetValue(pPolicyObj, POLICY_INDEX_WIDTH, rcWnd.Width());
				SetValue(pPolicyObj, POLICY_INDEX_HEIGHT, rcWnd.Height());
				RemoveLocationChangeCache(hWnd);

				// ��Ϊ��ʱ�ˣ�������Ҫ���ж�һ�£������Ƿ�������Ч
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