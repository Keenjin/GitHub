// TestDriverType.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>
#include <iostream>
#include <atlfile.h>

BOOL IsUSBDisk(TCHAR chDriver)
{
	BOOL bRet = FALSE;

	CAtlString strName;

	HANDLE hDevice = INVALID_HANDLE_VALUE;

	std::vector<BYTE> vBuff;

	PSTORAGE_DEVICE_DESCRIPTOR pDevDesc = NULL;

	DWORD dwBuffSize = 0;

	strName.Format(L"\\\\?\\%c:", chDriver);

	do
	{
		hDevice = CreateFile(strName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

		if (INVALID_HANDLE_VALUE == hDevice)
		{
			break;
		}

		dwBuffSize = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512;

		vBuff.resize(dwBuffSize + 1);

		pDevDesc = (PSTORAGE_DEVICE_DESCRIPTOR)&vBuff[0];

		pDevDesc->Size = dwBuffSize;

		STORAGE_PROPERTY_QUERY query;
		DWORD dwOutBytes = 0;
		BOOL bIoCtlResult = FALSE;

		query.PropertyId = StorageDeviceProperty;
		query.QueryType = PropertyStandardQuery;

		bIoCtlResult = DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(STORAGE_PROPERTY_QUERY),
			pDevDesc, dwBuffSize, &dwOutBytes, NULL);

		if (bIoCtlResult)
		{
			if (pDevDesc->BusType == BusTypeUsb)
			{
				//这个是USB设备
				bRet = TRUE;
			}
		}

	} while (0);

	if (hDevice != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hDevice);

		hDevice = INVALID_HANDLE_VALUE;
	}

	return bRet;
}

//判断磁盘是否可写
BOOL IsDiskCanWrite(TCHAR chDriver)
{
	BOOL bRet = FALSE;

	CAtlString strTempFile;

	CAtlFile tmpFile;

	strTempFile.Format(L"%c:\\test.tmp", chDriver);

	if (S_OK == tmpFile.Create(strTempFile, GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE))
	{
		bRet = TRUE;

		tmpFile.Close();
	}

	return bRet;
}


BOOL GetLocalDriver(std::vector<CAtlString>& vecDriver)
{
	vecDriver.clear();

	DWORD dwDrivers = GetLogicalDrives();
	DWORD dwFlag = 0x01;
	for (size_t i = 0; i < sizeof(DWORD) * 8; i++)
	{
		if (dwDrivers & (dwFlag << i))
		{
			CAtlString strDriver = L"A:\\";
			strDriver.SetAt(0, WCHAR(L'A' + i));

			if (DRIVE_FIXED == GetDriveType(strDriver) && !IsUSBDisk(WCHAR(L'A' + i)) && IsDiskCanWrite(WCHAR(L'A' + i)))
			{
				vecDriver.push_back(strDriver);
			}
		}
	}

	return vecDriver.size() > 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
	std::vector<CAtlString> vecDriver;
	BOOL bRet = GetLocalDriver(vecDriver);

	std::wcout.imbue(std::locale(""));

	std::wcout << L"获取磁盘结果：" << bRet << std::endl;

	for (size_t i = 0; i < vecDriver.size(); i++)
	{
		std::wcout << vecDriver[i].GetString() << L",";
	}

	std::wcout << std::endl;

	system("pause");

	return 0;
}

