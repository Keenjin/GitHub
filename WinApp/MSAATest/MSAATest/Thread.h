#pragma once
#include <vector>

unsigned __stdcall __THREAD_PROC(void* pArguments);

class IThreadProcCallback
{
public:
	virtual void ThreadProc() = 0;
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

template<UINT cThreadCnt = 1>
class CMutiThread : public IThreadProcCallback
{
public:
	CMutiThread() {}
	virtual ~CMutiThread() {}

	HRESULT Start(IThreadProcCallback* pThis)
	{
		HRESULT hr = S_OK;

		for (size_t i = 0; i < cThreadCnt; i++)
		{
			unsigned int uiTID = 0;
			m_hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, __THREAD_PROC, pThis, 0, &uiTID);
			if (m_hThreads[i] == INVALID_HANDLE_VALUE || m_hThreads[i] == NULL)
			{
				hr = E_FAIL;
				break;
			}
		}

		return hr;
	}
	DWORD Wait(BOOL bWaitAll, DWORD dwMilliseconds)
	{
		return WaitForMultipleObjects(cThreadCnt, m_hThreads, bWaitAll, dwMilliseconds);
	}
	void Stop()
	{
		for (size_t i = 0; i < cThreadCnt; i++)
		{
			if (m_hThreads[i])
			{
				CloseHandle(m_hThreads[i]);
			}
			m_hThreads[i] = NULL;
		}
	}

	virtual void ThreadProc() {};

protected:
	HANDLE	m_hThreads[cThreadCnt];
};

