// TestLib.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "TestZip\TestZip.h"
#include "TestBoost\TestBoost.h"

int _tmain(int argc, _TCHAR* argv[])
{
#ifdef TEST_ZIP
	TestZip();
#endif
#ifdef TEST_BOOST
	TestBoost();
#endif
	return 0;
}

