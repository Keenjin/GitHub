#pragma once
#include "Event.h"
#include "Thread.h"
#include "AutoCriticalSection.h"
#include "TaskContainer.h"

template<typename T, 
	typename TObj,
	typename TContainer = CTaskContainer<TObj>,
	UINT cThreadCnt = 3>
class CTaskPool : public CMutiThread<cThreadCnt>
{
	typedef void(T::*fPolicyProc)(TObj obj);

public:
	CTaskPool() 
		: m_bQuit(FALSE)
	{}
	~CTaskPool() {}

	HRESULT Create(T* pobj, fPolicyProc fn, TContainer* pContainer)
	{
		HRESULT hr = E_FAIL;

		do
		{
			if (!pobj || !fn || !pContainer)
			{
				break;
			}

			m_pObj = pobj;
			m_fProc = fn;
			m_pContainer = pContainer;
			hr = Start(this);

		} while (FALSE);
		
		return hr;
	}

	void Destroy()
	{
		InterlockedExchange((volatile long*)&m_bQuit, TRUE);

		// 清空任务堆积的队列
		if (m_pContainer)
		{
			m_pContainer->Clear();
			m_pContainer = NULL;
		}

		SetEventAll();

		Stop();
	}

	void AddTask(TObj& obj)
	{
		if (IsQuit())
		{
			return;
		}

		if (!m_pContainer)
		{
			return;
		}

		m_pContainer->AddTail(obj);
		SetEventAll();
	}

	virtual void ThreadProc(unsigned int nIndex)
	{
		while (!IsQuit())
		{
			WaitEvent(nIndex, INFINITE);

			if (IsQuit())
			{
				break;
			}
			
			if (!m_pContainer)
			{
				break;
			}

			if (m_pObj && m_fProc)
			{
				(m_pObj->*m_fProc)(m_pContainer->PopHead());
			}
		}
	}

private:
	BOOL IsQuit()
	{
		BOOL bRet = FALSE;

		do
		{
			if (IsStop())
			{
				bRet = TRUE;
				break;
			}

			InterlockedExchange((volatile long*)&bRet, m_bQuit);

		} while (FALSE);
		
		return bRet;
	}

private:
	T* m_pObj;
	fPolicyProc	m_fProc;
	TContainer* m_pContainer;

	BOOL	m_bQuit;
};

