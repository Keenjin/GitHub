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

	CRect rcWnd;
	rcWnd.left = GetValue<LONG>(pPolicyObj, POLICY_INDEX_X);
	rcWnd.top = GetValue<LONG>(pPolicyObj, POLICY_INDEX_Y);
	rcWnd.right = rcWnd.left + GetValue<LONG>(pPolicyObj, POLICY_INDEX_WIDTH);
	rcWnd.bottom = rcWnd.top + GetValue<LONG>(pPolicyObj, POLICY_INDEX_HEIGHT);

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
			// ����LocationChange�¼����ȿ����ػ�����û�У����û�У�����ӵ����棬ͬʱ�����ô��ڻ��ף�����У����뱾�ػ����λ�ñȶԣ���λ����ͬ��ʱ����˵��λ��ȷʵ�����ˣ���ն������ظ����ڣ���������һ��
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
				lock.~CAutoCriticalSection();		// �ͷ�������ֹ����

				BOOL bStop = FALSE;

				// �������һ��LocationChange�¼���˵���������Լ����׵ģ���Ҫ��ʱһ����鿴�Ƿ��б仯�����������һ����˵�����²����ģ�ֱ�ӱȶ�λ��
				if (GetValue<BOOL>(pPolicyObj, POLICY_INDEX_LAST_LOCATION_CHANGE))
				{
					Sleep(10);		// Ŀǰ��ֻ�ӳ�10ms�������Ƿ�仯

					CRect rcNowWnd;
					GetWindowRect(hWnd, &rcNowWnd);
					if (rcNowWnd == rcLast)
					{
						// ˵���Ѿ���ֹ��
						bStop = TRUE;
					}
				}
				else
				{
					if (rcWnd == rcLast)
					{
						// ˵���Ѿ���ֹ��
						bStop = TRUE;
					}
				}

				if (!bStop)
				{
					// ��û�о�ֹ�����������
					SetValue(pPolicyObj, POLICY_INDEX_GROUP_END, TRUE);
					SetValue(pPolicyObj, POLICY_INDEX_GROUP_ITEM_END, TRUE);
					SetValue(pPolicyObj, POLICY_INDEX_TASK_ADD, TRUE);
					SetValue(pPolicyObj, POLICY_INDEX_LAST_LOCATION_CHANGE, TRUE);
				}
				else
				{
					// �Ѿ���ֹ�ˣ���������ö���
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