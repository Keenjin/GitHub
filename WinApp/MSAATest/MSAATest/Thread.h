#pragma once
#include <vector>
#include "Event.h"

unsigned __stdcall __THREAD_PROC(void* pArguments);

class IThreadProcCallback
{
public:
	virtual void ThreadProc() = 0;
};

class IThreadProcCallbackEx
{
public:
	virtual void ThreadProc(unsigned int nIndex) = 0;
};

class CThread : public IThreadProcCallback
{
public:
	CThread();
	virtual ~CThread();

	HRESULT Start(IThreadProcCallback* pThis);
	DWORD Wait(DWORD dwMilliseconds);
	void Stop();

	virtual void ThreadProc() {};

	DWORD GetThreadID() const { return m_nThreadID; }
	HANDLE GetThreadHandle() const { return m_hThread; }

protected:
	unsigned int	m_nThreadID;
	HANDLE	m_hThread;
};

unsigned __stdcall __MUL_THREAD_PROC(void* pArguments);

typedef struct _THREAD_PROC_PARAM
{
	IThreadProcCallbackEx* pCallbackObj;
	DWORD	dwIndex;
	CEvent	event;
}THREAD_PROC_PARAM, *PTHREAD_PROC_PARAM;

template<UINT cThreadCnt = 1>
class CMutiThread : public IThreadProcCallbackEx
{
public:
	CMutiThread() : m_bStop(FALSE) {}
	virtual ~CMutiThread() {}

	HRESULT Start(IThreadProcCallbackEx* pThis)
	{
		HRESULT hr = S_OK;

		for (size_t i = 0; i < cThreadCnt; i++)
		{
			unsigned int uiTID = 0;
			m_threadParam[i].event.Create(FALSE, FALSE);
			m_threadParam[i].pCallbackObj = pThis;
			m_threadParam[i].dwIndex = i;
			m_hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, __MUL_THREAD_PROC, &m_threadParam[i], 0, &uiTID);
			if (m_hThreads[i] == INVALID_HANDLE_VALUE || m_hThreads[i] == NULL)
			{
				hr = E_FAIL;
				break;
			}
		}

		return hr;
	}
	DWORD WaitEvent(DWORD dwIndex, DWORD dwMilliseconds)
	{
		if (dwIndex < cThreadCnt)
		{
			return m_threadParam[dwIndex].event.Wait(dwMilliseconds);
		}
		return WAIT_FAILED;
	}
	BOOL SetEvent(DWORD dwIndex)
	{
		if (dwIndex >= cThreadCnt)
		{
			return FALSE;
		}
		return m_threadParam[dwIndex].event.Set();
	}
	BOOL SetEventAll()
	{
		BOOL bRet = TRUE;
		for (size_t i = 0; i < cThreadCnt; i++)
		{
			bRet &= m_threadParam[i].event.Set();
		}
		return bRet;
	}
	DWORD Wait(BOOL bWaitAll, DWORD dwMilliseconds)
	{
		return WaitForMultipleObjects(cThreadCnt, m_hThreads, bWaitAll, dwMilliseconds);
	}
	void Stop()
	{
		InterlockedExchange((volatile long*)&m_bStop, TRUE);
		SetEventAll();
		Wait(TRUE, INFINITE);
		for (size_t i = 0; i < cThreadCnt; i++)
		{
			m_event[i].Destroy();
			if (m_hThreads[i])
			{
				CloseHandle(m_hThreads[i]);
				m_hThreads[i] = NULL;
			}
		}
	}
	BOOL IsStop()
	{
		BOOL bQuit = FALSE;
		InterlockedExchange((volatile long*)&bQuit, m_bStop);
		return bQuit;
	}

	virtual void ThreadProc(unsigned int nIndex) {};

protected:
	HANDLE	m_hThreads[cThreadCnt];
	THREAD_PROC_PARAM	m_threadParam[cThreadCnt];
	CEvent	m_event[cThreadCnt];
	BOOL	m_bStop;
};

