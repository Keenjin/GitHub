// Exception.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

LONG NTAPI MY_PVECTORED_EXCEPTION_HANDLER(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	return EXCEPTION_CONTINUE_SEARCH;
}

LONG WINAPI MY_PTOP_LEVEL_EXCEPTION_FILTER(_In_ struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	return EXCEPTION_CONTINUE_SEARCH;
}

LONG MY_EXCEPTION_FILTER(DWORD dwErrCode)
{
	return EXCEPTION_CONTINUE_SEARCH;
}

int main()
{
	PVOID pPreHandler = AddVectoredExceptionHandler(1, MY_PVECTORED_EXCEPTION_HANDLER);

	LPTOP_LEVEL_EXCEPTION_FILTER pPreUnhandled = SetUnhandledExceptionFilter(MY_PTOP_LEVEL_EXCEPTION_FILTER);

	__try
	{
		int x = 3;
		int y = 0;
		int z = x / y;
	}
	__except (MY_EXCEPTION_FILTER(GetExceptionCode()))
	{

	}

	SetUnhandledExceptionFilter(pPreUnhandled);
	
	RemoveVectoredExceptionHandler(pPreHandler);
    return 0;
}

