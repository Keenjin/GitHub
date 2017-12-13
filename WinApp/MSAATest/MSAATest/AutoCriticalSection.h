#pragma once
#include <atlcore.h>

class CAutoCriticalSection
{
public:
	CAutoCriticalSection(ATL::CComAutoCriticalSection& cs)
		: m_cs(cs)
	{
		m_cs.Lock();
	}

	~CAutoCriticalSection()
	{
		m_cs.Unlock();
	}

private:
	ATL::CComAutoCriticalSection& m_cs;
};

