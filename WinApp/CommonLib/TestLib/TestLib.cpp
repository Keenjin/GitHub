// TestLib.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ZLib.h"

int _tmain(int argc, _TCHAR* argv[])
{
	BOOL bRet = ZipExtract(L"G:\\MCC18.zip", L"G:\\");
	return 0;
}

