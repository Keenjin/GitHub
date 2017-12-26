#pragma once
#include "CrashHandler.h"

class CCrashReport
{
public:
	static CCrashReport& Instance();

	void ProccessEnter();
	void ThreadEnter();

private:
	CCrashReport();
	~CCrashReport();

	BOOL IsProcEntered();

private:
	CCrashHandler	m_oCrashHandler;
	BOOL	m_bProcEntered;
};

