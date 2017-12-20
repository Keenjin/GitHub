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

	// ���˹���
	// 1��ϵͳ�������ڹ��ˣ�e.g���������������������桢��ʼ�˵��ȡ�������ȣ� - ����ϵͳ�̶��������Ե�	����	30���Ӽ���һ��
	// 2�����ݵ�С�ʹ��ڵĹ��ˣ����ǣ���Ҫ���ų��б�����һЩС����ȣ�
	// 3�����ش��ڡ���Ч���ڵĹ���
	// 4�������¼��Ĺ���
	// 5��Location�¼�����


	// ����������Ϣ��ȡ���Լ������

	DWORD dwEvent = GetValue<DWORD>(pPolicyObj, POLICY_INDEX_EVENT);
	DWORD dwIdObject = GetValue<DWORD>(pPolicyObj, POLICY_INDEX_IDOBJECT);
	DWORD dwIdChild = GetValue<DWORD>(pPolicyObj, POLICY_INDEX_IDCHILD);
	HWND hWnd = (HWND)GetValue<ULONGLONG>(pPolicyObj, POLICY_INDEX_HWND);
	DWORD dwThreadID = GetValue<DWORD>(pPolicyObj, POLICY_INDEX_TID);

	BOOL bFilter = TRUE;
	do
	{
		// ���ɼ�����
		if (!IsValidVisibleWnd(hWnd))
		{
			break;
		}

		// ϵͳ��������
		HWND hTopWnd = HELP_API::WND_EVENT_API::GetTopParentWnd(hWnd);
		if (IsHitSysWnd(hTopWnd))
		{
			break;
		}
		SetValue(pPolicyObj, POLICY_INDEX_TOPHWND, (ULONGLONG)hTopWnd);

		// ���Ϲ��С������Ļ�ⴰ��
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
	HWND hTopWnd = ::FindWindow(L"WorkerW", NULL);//�ȵ�WIN10ϵͳ����
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
		//���û�ҵ����ٰ�Win7��XP��ʽ����
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
		m_vecSysWnd.push_back(hTopWnd);		// ֻ��Ҫ���㴰��
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
		m_vecSysWnd.push_back(hShellTrayWnd);		// ֻ��Ҫ���㴰��
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

		// ��������Ļ�������������������ܵ���Ļ���ˣ���������
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