// 8_OpenDevice.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <winioctl.h>
#include "../6_HelloDDK/define.h"

// 命令行：OpenDevice.exe [设备符号链接名] [r/w]
int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "参数个数不满足" << std::endl;
		return -1;
	}

	LPCSTR lpstrDeviceSymName = argv[1];
	if (!lpstrDeviceSymName)
	{
		std::cout << "参数错误" << std::endl;
		return -1;
	}

	std::cout << "设备符号链接名：" << lpstrDeviceSymName << std::endl;

	CAtlStringA strFullDeviceSymName;
	strFullDeviceSymName.Format("\\\\.\\%s", lpstrDeviceSymName);
	HANDLE hDevice = CreateFileA(strFullDeviceSymName,
		GENERIC_WRITE | GENERIC_READ,
		0,		// 不共享，也就是独占
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		std::cout << "无法打开设备" << std::endl;
		return -1;
	}

	if (argc == 3)
	{
		CHAR* pCh = argv[2];
		if (*pCh == 'r')
		{
			UCHAR buffer[10] = { 0 };
			ULONG ulReaded = 0;
			BOOL bRet = ReadFile(hDevice, buffer, 10, &ulReaded, NULL);
			if (bRet)
			{
				std::cout << "成功读取字节数：" << ulReaded << std::endl;
				std::cout << "读取内容：";
				for (size_t i = 0; i < ulReaded; i++)
				{
					std::cout << std::hex << (int)buffer[i] << " ";
				}
				std::cout << std::endl;
			}
			else
			{
				std::cout << "读取失败" << std::endl;
			}
		}
		else if (*pCh == 'w')
		{
			UCHAR buffer[10] = { 0 };
			ULONG ulWrited = 0;
			memset(buffer, 0xBB, 10);
			BOOL bRet = WriteFile(hDevice, buffer, 10, &ulWrited, NULL);
			if (bRet)
			{
				std::cout << "成功写入字节数：" << ulWrited << std::endl;
				std::cout << "写入内容：";
				for (size_t i = 0; i < ulWrited; i++)
				{
					std::cout << std::hex << (int)buffer[i] << " ";
				}
				std::cout << std::endl;
			}
			else
			{
				std::cout << "写入失败" << std::endl;
			}
		}
		else if (*pCh == 's')
		{
			LARGE_INTEGER llFileSize;
			llFileSize.QuadPart = 0;
			llFileSize.LowPart = GetFileSize(hDevice, (LPDWORD)&llFileSize.HighPart);
			std::cout << "文件大小：" << llFileSize.QuadPart << " Bytes" << std::endl;
		}
		else if (*pCh == 'd')
		{
			UCHAR bufferIn[10] = { 0 };
			UCHAR bufferOut[10] = { 0 };
			DWORD dwRet = 0;
			// IOCTL_TEST1
			memset(bufferIn, 0xBB, 10);
			BOOL bRet = DeviceIoControl(hDevice, IOCTL_TEST1, bufferIn, 10, bufferOut, 10, &dwRet, NULL);
			if (bRet)
			{
				for (size_t i = 0; i < dwRet; i++)
				{
					std::cout << std::hex << (int)bufferOut[i] << " ";
				}
				std::cout << std::endl;
			}
			else
			{
				std::cout << "IOCTL_TEST1通信失败" << std::endl;
			}

			// IOCTL_TEST2
			memset(bufferIn, 0xCC, 10);
			dwRet = 0;
			bRet = DeviceIoControl(hDevice, IOCTL_TEST2, bufferIn, 10, bufferOut, 10, &dwRet, NULL);
			if (bRet)
			{
				std::cout << "IOCTL_TEST2通信成功" << std::endl;
				for (size_t i = 0; i < dwRet; i++)
				{
					std::cout << std::hex << (int)bufferOut[i] << " ";
				}
				std::cout << std::endl;
			}
			else
			{
				std::cout << "IOCTL_TEST2通信失败" << std::endl;
			}

			// IOCTL_TEST3
			memset(bufferIn, 0xBC, 10);
			dwRet = 0;
			bRet = DeviceIoControl(hDevice, IOCTL_TEST3, bufferIn, 10, bufferOut, 10, &dwRet, NULL);
			if (bRet)
			{
				std::cout << "IOCTL_TEST3通信成功" << std::endl;
				for (size_t i = 0; i < dwRet; i++)
				{
					std::cout << std::hex << (int)bufferOut[i] << " ";
				}
				std::cout << std::endl;
			}
			else
			{
				std::cout << "IOCTL_TEST3通信失败" << std::endl;
			}
		}
	}

	// 对设备进行读

	CloseHandle(hDevice);
    return 0;
}

