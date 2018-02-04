#pragma once
#include <atlstr.h>
#include <debugapi.h>

#if defined(_LOG) || defined(_DEBUG)
#define LOG_PRINT(format, ...)	\
	{CAtlString strLog;	\
	strLog.Format(format L"\n", __VA_ARGS__);		\
	OutputDebugString(strLog);}
#else
#define LOG_PRINT(format, ...)
#endif