// FileSave.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "FileClusterTag.h"


int _tmain(int argc, _TCHAR* argv[])
{
	DWORD dwSectorPerCluster, dwBytesPerSector, dwNumOfFreeCluster, dwTotalCluster;
	GetDiskFreeSpace(L"C:\\", &dwSectorPerCluster, &dwBytesPerSector, &dwNumOfFreeCluster, &dwTotalCluster);
	GetDiskFreeSpace(L"d:\\", &dwSectorPerCluster, &dwBytesPerSector, &dwNumOfFreeCluster, &dwTotalCluster);
	GetDiskFreeSpace(L"e:\\", &dwSectorPerCluster, &dwBytesPerSector, &dwNumOfFreeCluster, &dwTotalCluster);
	GetDiskFreeSpace(L"f:\\", &dwSectorPerCluster, &dwBytesPerSector, &dwNumOfFreeCluster, &dwTotalCluster);
	GetDiskFreeSpace(L"g:\\", &dwSectorPerCluster, &dwBytesPerSector, &dwNumOfFreeCluster, &dwTotalCluster);
	GetDiskFreeSpace(L"h:\\", &dwSectorPerCluster, &dwBytesPerSector, &dwNumOfFreeCluster, &dwTotalCluster);
	GetDiskFreeSpace(L"i:\\", &dwSectorPerCluster, &dwBytesPerSector, &dwNumOfFreeCluster, &dwTotalCluster);
	GetDiskFreeSpace(L"j:\\", &dwSectorPerCluster, &dwBytesPerSector, &dwNumOfFreeCluster, &dwTotalCluster);
	GetDiskFreeSpace(L"k:\\", &dwSectorPerCluster, &dwBytesPerSector, &dwNumOfFreeCluster, &dwTotalCluster);
	GetDiskFreeSpace(L"l:\\", &dwSectorPerCluster, &dwBytesPerSector, &dwNumOfFreeCluster, &dwTotalCluster);

	return 0;

	CFileClusterTag fileTag;
	fileTag.AddTag(L"I:\\HCI-����թ�ĵ����ݱ�������-Luxozhang-201703281739.fw.png", L"I:\\HCI-����թ�ĵ����ݱ�������-Luxozhang-201703281739.fw_tag.png", FALSE);
	fileTag.RemoveTag(L"I:\\HCI-����թ�ĵ����ݱ�������-Luxozhang-201703281739.fw_tag.png", NULL, FALSE);
	return 0;
}

