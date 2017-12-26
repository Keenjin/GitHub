#include "stdafx.h"
#include "CrashReport.h"



CCrashReport::CCrashReport()
	: m_bProcEntered(FALSE)
{
}

CCrashReport::~CCrashReport()
{
}

CCrashReport& CCrashReport::Instance()
{
	static CCrashReport theObj;
	return theObj;
}

void CCrashReport::ProccessEnter()
{
	if (!IsProcEntered())
	{
		InterlockedExchange((volatile long*)&m_bProcEntered, TRUE);

		m_oCrashHandler.SetProcessExceptionHandlers();
		m_oCrashHandler.SetThreadExceptionHandlers();
	}
}

void CCrashReport::ThreadEnter()
{
	m_oCrashHandler.SetThreadExceptionHandlers();
}

BOOL CCrashReport::IsProcEntered()
{
	BOOL bEverEnter = FALSE;
	InterlockedExchange((volatile long*)&bEverEnter, m_bProcEntered);
	return bEverEnter;
}

