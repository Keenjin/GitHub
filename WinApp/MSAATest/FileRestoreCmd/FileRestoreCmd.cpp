// FileRestoreCmd.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "FileClusterTag.h"
#include <atlpath.h>
#include <iostream>


int _tmain(int argc, _TCHAR* argv[])
{
	std::cout << "开始执行" << std::endl;

	CAtlString strDir = L".\\";
	if (argc >= 2)
	{
		strDir = argv[1];
	}

	if (!ATLPath::FileExists(strDir))
	{
		std::wcout << L"目标目录不存在" << std::endl;
		return 0;
	}

	DWORD dwSucCnt = 0;

	CFileClusterTag	fileRestore;
	std::vector<DISKINFO>	vecDiskInfo;
	fileRestore.EnumDiskDevice(vecDiskInfo);
	for (size_t i = 0; i < vecDiskInfo.size(); i++)
	{
		CAtlString strDevName;
		strDevName.Format(L"\\\\.\\PHYSICALDRIVE%d", vecDiskInfo[i].dwPhysicNum);
		fileRestore.DiskRestore(strDevName, 0, vecDiskInfo[i].ullDiskSize, strDir, &dwSucCnt);
	}

	std::wcout << L"还原结束，成功还原文档个数：" << dwSucCnt << std::endl;

	return 0;
}

