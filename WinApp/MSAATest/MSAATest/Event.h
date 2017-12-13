#pragma once
#include <Windows.h>

class CEvent
{
public:
	CEvent() {}
	~CEvent() { Destroy(); }

	BOOL Create(BOOL bManual, BOOL bInitState)
	{
		if (!m_hEvent)
		{
			m_hEvent = ::CreateEvent(NULL, bManual, bInitState, NULL);
		}

		return ((m_hEvent != INVALID_HANDLE_VALUE) && (m_hEvent != NULL));
	}

	BOOL SetEvent()
	{
		return ::SetEvent(m_hEvent);
	}

	BOOL ResetEvent()
	{
		return ::ResetEvent(m_hEvent);
	}

	DWORD Wait(DWORD dwMill)
	{
		return WaitForSingleObject(m_hEvent, dwMill);
	}

	void Destroy()
	{
		if (m_hEvent)
		{
			::CloseHandle(m_hEvent);
			m_hEvent = NULL;
		}
	}

private:
	HANDLE	m_hEvent;
};

