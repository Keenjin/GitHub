#pragma once
#include <queue>
#include "Event.h"
#include "Thread.h"
#include "AutoCriticalSection.h"

template<typename T, 
	typename TObj,
	UINT cThreadCnt = 3>
class CPolicySchedule : public CMutiThread<cThreadCnt>
{
	typedef void(T::*fPolicyProc)(TObj obj);

public:
	CPolicySchedule() 
		: m_bQuit(FALSE)
	{}
	~CPolicySchedule() {}

	HRESULT Create(T* pobj, fPolicyProc fn)
	{
		HRESULT hr = E_FAIL;

		do
		{
			if (!pobj || !fn)
			{
				break;
			}

			m_pObj = pobj;
			m_fProc = fn;
			m_eventTask.Create(FALSE, FALSE);
			m_eventQuit.Create(TRUE, TRUE);
			hr = Start(this);

		} while (FALSE);
		
		return hr;
	}

	void Destroy()
	{
		{
			CAutoCriticalSection lock(m_csQuit);
			m_bQuit = TRUE;
		}

		// 清空任务堆积的队列
		{
			CAutoCriticalSection lock(m_csQuit);
			while (!m_qTaskObjs.empty())
			{
				m_qTaskObjs.pop();
			}
		}

		m_eventQuit.Wait(INFINITE);
		Stop();
	}

	void AddTask(TObj& obj)
	{
		if (IsQuit())
		{
			return;
		}

		{
			CAutoCriticalSection lock(m_csTaskObjs);
			m_qTaskObjs.push(obj);
		}
		
		m_eventTask.SetEvent();
	}

	virtual void ThreadProc()
	{
		while (!IsQuit())
		{
			m_eventTask.Wait(INFINITE);
			DWORD dwIndex = Wait(FALSE, INFINITE);
			dwIndex -= WAIT_OBJECT_0;
			if (dwIndex >= 0 && dwIndex < cThreadCnt)
			{
				TObj obj;
				{
					CAutoCriticalSection lock(m_csQuit);
					obj = m_qTaskObjs.front();
					m_qTaskObjs.pop();
				}
				
				(m_pObj->*m_fProc)(obj);
			}
		}

		m_eventQuit.SetEvent();
	}

private:
	BOOL IsQuit()
	{
		CAutoCriticalSection lock(m_csQuit);
		if (!m_bQuit)
		{
			m_eventQuit.ResetEvent();
		}
		return m_bQuit;
	}

private:
	CComAutoCriticalSection	m_csTaskObjs;
	std::queue<TObj>	m_qTaskObjs;

	T* m_pObj;
	fPolicyProc	m_fProc;

	CEvent	m_eventTask;

	CComAutoCriticalSection	m_csQuit;
	CEvent	m_eventQuit;
	BOOL	m_bQuit;
};

