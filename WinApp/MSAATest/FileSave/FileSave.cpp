// FileSave.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "FileClusterTag.h"


int _tmain(int argc, _TCHAR* argv[])
{
	CFileClusterTag fileTag;
	fileTag.AddTag(L"I:\\test1.txt", L"I:\\test1_tag.txt", FALSE);
	return 0;
}

