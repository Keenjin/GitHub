#include "stdafx.h"
#include "Thread.h"

unsigned __stdcall __THREAD_PROC(void* pArguments)
{
	IThreadProcCallback* pThis = (IThreadProcCallback*)pArguments;
	if (pThis)
	{
		pThis->ThreadProc();
	}

	_endthreadex(0);
	return 0;
}

unsigned __stdcall __MUL_THREAD_PROC(void* pArguments)
{
	THREAD_PROC_PARAM* pThis = (THREAD_PROC_PARAM*)pArguments;
	if (pThis && pThis->pCallbackObj)
	{
		pThis->pCallbackObj->ThreadProc(pThis->dwIndex);
	}

	_endthreadex(0);
	return 0;
}


CThread::CThread()
	: m_nThreadID(0)
	, m_hThread(NULL)
{
}


CThread::~CThread()
{
	Stop();
}

HRESULT CThread::Start(IThreadProcCallback* pThis)
{
	HRESULT hr = S_OK;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, __THREAD_PROC, pThis, 0, &m_nThreadID);
	if (m_hThread == INVALID_HANDLE_VALUE || m_hThread == NULL)
	{
		hr = E_FAIL;
	}
	return hr;
}

DWORD CThread::Wait(DWORD dwMilliseconds)
{
	DWORD dwRet = 0;
	if (m_hThread)
	{
		dwRet = WaitForSingleObject(m_hThread, dwMilliseconds);
	}
	return dwRet;
}

void CThread::Stop()
{
	if (m_hThread)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

