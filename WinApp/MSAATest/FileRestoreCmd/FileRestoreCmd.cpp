// FileRestoreCmd.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "FileClusterTag.h"
#include <atlpath.h>
#include <iostream>


int _tmain(int argc, _TCHAR* argv[])
{
	std::cout << "��ʼִ��" << std::endl;

	CAtlString strDir = L".\\";
	if (argc >= 2)
	{
		strDir = argv[1];
	}

	if (!ATLPath::FileExists(strDir))
	{
		std::wcout << L"Ŀ��Ŀ¼������" << std::endl;
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

	std::wcout << L"��ԭ�������ɹ���ԭ�ĵ�������" << dwSucCnt << std::endl;

	return 0;
}

