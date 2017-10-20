// 6_LoadNTDriver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <atlpath.h>

// 函数声明
BOOL LoadNTDriver(LPCSTR lpstrDriverName, LPCSTR lpstrDriverPath);
BOOL UnloadNTDriver(LPCSTR lpstrDriverName);

// 命令行格式：
// LoadNTDriver.exe [drivername] [driverpath]
int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cout << "参数个数不满足" << std::endl;
		return -1;
	}

	LPCSTR lpstrDriverName = argv[1];
	LPCSTR lpstrDriverPath = argv[2];
	if (!lpstrDriverName || !lpstrDriverPath)
	{
		std::cout << "参数错误" << std::endl;
		return -1;
	}

	std::cout << "服务名：" << lpstrDriverName << "驱动文件：" << lpstrDriverPath << std::endl;

	if (!ATLPath::FileExists(lpstrDriverPath) || ATLPath::IsDirectory(lpstrDriverPath))
	{
		std::cout << "驱动文件不存在" << std::endl;
		return -1;
	}

	// 加载驱动
	BOOL bRet = LoadNTDriver(lpstrDriverName, lpstrDriverPath);
	if (!bRet)
	{
		std::cout << "驱动加载失败" << std::endl;
		return -1;
	}

	std::cout << "按任意键卸载驱动：";
	char ch;
	std::cin >> ch;

	// 卸载驱动
	bRet = UnloadNTDriver(lpstrDriverName);
	if (!bRet)
	{
		std::cout << "驱动卸载失败" << std::endl;
		return -1;
	}

    return 0;
}

BOOL LoadNTDriver(LPCSTR lpstrDriverName, LPCSTR lpstrDriverPath)
{
	// 得到完整的驱动路径
	char szDriverImagePath[MAX_PATH] = { 0 };
	GetFullPathNameA(lpstrDriverPath, MAX_PATH, szDriverImagePath, NULL);

	std::cout << "驱动名：" << lpstrDriverName << "，驱动完整路径：" << szDriverImagePath << std::endl;

	BOOL bRet = FALSE;
	SC_HANDLE hServiceMgr = NULL;
	SC_HANDLE hServiceDDK = NULL;
	do
	{
		// 打开服务控制管理器
		hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (!hServiceMgr)
		{
			std::cout << "打开服务控制管理器失败" << std::endl;
			break;
		}

		hServiceDDK = CreateServiceA(hServiceMgr,		// 服务管理器
			lpstrDriverName,			// 服务名
			lpstrDriverName,			// 显示名称
			SERVICE_ALL_ACCESS,			// 加载驱动程序的访问权限
			SERVICE_KERNEL_DRIVER,		// 表示加载的服务是驱动程序
			SERVICE_DEMAND_START,		// 手动方式启动驱动
			SERVICE_ERROR_IGNORE,
			szDriverImagePath,
			NULL,NULL,NULL,NULL,NULL);

		if (!hServiceDDK)
		{
			DWORD dwErrorCode = GetLastError();
			if (dwErrorCode != ERROR_IO_PENDING && dwErrorCode != ERROR_SERVICE_EXISTS)
			{
				std::cout << "创建服务失败，错误码：" << dwErrorCode << std::endl;
				break;
			}
			std::cout << "服务已存在" << std::endl;

			hServiceDDK = OpenServiceA(hServiceMgr, lpstrDriverName, SERVICE_ALL_ACCESS);
			if (!hServiceDDK)
			{
				std::cout << "服务存在，但打开服务失败，错误码：" << GetLastError() << std::endl;
				break;
			}
		}

		// 开启此服务
		bRet = StartServiceA(hServiceDDK, NULL, NULL);
		if (!bRet)
		{
			DWORD dwErrorCode = GetLastError();
			if (dwErrorCode != ERROR_IO_PENDING && dwErrorCode != ERROR_SERVICE_ALREADY_RUNNING)
			{
				std::cout << "启动服务失败，错误码：" << dwErrorCode << std::endl;
				break;
			}
			
			if (dwErrorCode != ERROR_IO_PENDING)
			{
				std::cout << "设备被挂起" << std::endl;
				break;
			}

			std::cout << "服务已启动" << std::endl;
		}

		bRet = TRUE;

	} while (FALSE);

	if (hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
		hServiceDDK = NULL;
	}

	if (hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
		hServiceMgr = NULL;
	}

	return bRet;
}

BOOL UnloadNTDriver(LPCSTR lpstrDriverName)
{
	std::cout << "驱动名：" << lpstrDriverName << std::endl;

	BOOL bRet = FALSE;
	SC_HANDLE hServiceMgr = NULL;
	SC_HANDLE hServiceDDK = NULL;
	do
	{
		// 打开服务控制管理器
		hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (!hServiceMgr)
		{
			std::cout << "打开服务控制管理器失败" << std::endl;
			break;
		}

		hServiceDDK = OpenServiceA(hServiceMgr, lpstrDriverName, SERVICE_ALL_ACCESS);
		if (!hServiceDDK)
		{
			std::cout << "打开服务失败，错误码：" << GetLastError() << std::endl;
			break;
		}

		// 停止驱动程序
		SERVICE_STATUS srvStatus;
		bRet = ControlService(hServiceDDK, SERVICE_CONTROL_STOP, &srvStatus);
		if (!bRet)
		{
			std::cout << "停止服务失败，错误码：" << GetLastError() << std::endl;
		}

		// 卸载驱动程序
		bRet = DeleteService(hServiceDDK);
		if (!bRet)
		{
			std::cout << "卸载驱动失败，错误码：" << GetLastError() << std::endl;
			break;
		}

		bRet = TRUE;

	} while (FALSE);

	if (hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
		hServiceDDK = NULL;
	}

	if (hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
		hServiceMgr = NULL;
	}

	return bRet;
}