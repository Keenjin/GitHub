// 6_LoadNTDriver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <atlpath.h>

// ��������
BOOL LoadNTDriver(LPCSTR lpstrDriverName, LPCSTR lpstrDriverPath);
BOOL UnloadNTDriver(LPCSTR lpstrDriverName);

// �����и�ʽ��
// LoadNTDriver.exe [drivername] [driverpath]
int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cout << "��������������" << std::endl;
		return -1;
	}

	LPCSTR lpstrDriverName = argv[1];
	LPCSTR lpstrDriverPath = argv[2];
	if (!lpstrDriverName || !lpstrDriverPath)
	{
		std::cout << "��������" << std::endl;
		return -1;
	}

	std::cout << "��������" << lpstrDriverName << "�����ļ���" << lpstrDriverPath << std::endl;

	if (!ATLPath::FileExists(lpstrDriverPath) || ATLPath::IsDirectory(lpstrDriverPath))
	{
		std::cout << "�����ļ�������" << std::endl;
		return -1;
	}

	// ��������
	BOOL bRet = LoadNTDriver(lpstrDriverName, lpstrDriverPath);
	if (!bRet)
	{
		std::cout << "��������ʧ��" << std::endl;
		return -1;
	}

	std::cout << "�������ж��������";
	char ch;
	std::cin >> ch;

	// ж������
	bRet = UnloadNTDriver(lpstrDriverName);
	if (!bRet)
	{
		std::cout << "����ж��ʧ��" << std::endl;
		return -1;
	}

    return 0;
}

BOOL LoadNTDriver(LPCSTR lpstrDriverName, LPCSTR lpstrDriverPath)
{
	// �õ�����������·��
	char szDriverImagePath[MAX_PATH] = { 0 };
	GetFullPathNameA(lpstrDriverPath, MAX_PATH, szDriverImagePath, NULL);

	std::cout << "��������" << lpstrDriverName << "����������·����" << szDriverImagePath << std::endl;

	BOOL bRet = FALSE;
	SC_HANDLE hServiceMgr = NULL;
	SC_HANDLE hServiceDDK = NULL;
	do
	{
		// �򿪷�����ƹ�����
		hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (!hServiceMgr)
		{
			std::cout << "�򿪷�����ƹ�����ʧ��" << std::endl;
			break;
		}

		hServiceDDK = CreateServiceA(hServiceMgr,		// ���������
			lpstrDriverName,			// ������
			lpstrDriverName,			// ��ʾ����
			SERVICE_ALL_ACCESS,			// ������������ķ���Ȩ��
			SERVICE_KERNEL_DRIVER,		// ��ʾ���صķ�������������
			SERVICE_DEMAND_START,		// �ֶ���ʽ��������
			SERVICE_ERROR_IGNORE,
			szDriverImagePath,
			NULL,NULL,NULL,NULL,NULL);

		if (!hServiceDDK)
		{
			DWORD dwErrorCode = GetLastError();
			if (dwErrorCode != ERROR_IO_PENDING && dwErrorCode != ERROR_SERVICE_EXISTS)
			{
				std::cout << "��������ʧ�ܣ������룺" << dwErrorCode << std::endl;
				break;
			}
			std::cout << "�����Ѵ���" << std::endl;

			hServiceDDK = OpenServiceA(hServiceMgr, lpstrDriverName, SERVICE_ALL_ACCESS);
			if (!hServiceDDK)
			{
				std::cout << "������ڣ����򿪷���ʧ�ܣ������룺" << GetLastError() << std::endl;
				break;
			}
		}

		// �����˷���
		bRet = StartServiceA(hServiceDDK, NULL, NULL);
		if (!bRet)
		{
			DWORD dwErrorCode = GetLastError();
			if (dwErrorCode != ERROR_IO_PENDING && dwErrorCode != ERROR_SERVICE_ALREADY_RUNNING)
			{
				std::cout << "��������ʧ�ܣ������룺" << dwErrorCode << std::endl;
				break;
			}
			
			if (dwErrorCode != ERROR_IO_PENDING)
			{
				std::cout << "�豸������" << std::endl;
				break;
			}

			std::cout << "����������" << std::endl;
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
	std::cout << "��������" << lpstrDriverName << std::endl;

	BOOL bRet = FALSE;
	SC_HANDLE hServiceMgr = NULL;
	SC_HANDLE hServiceDDK = NULL;
	do
	{
		// �򿪷�����ƹ�����
		hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (!hServiceMgr)
		{
			std::cout << "�򿪷�����ƹ�����ʧ��" << std::endl;
			break;
		}

		hServiceDDK = OpenServiceA(hServiceMgr, lpstrDriverName, SERVICE_ALL_ACCESS);
		if (!hServiceDDK)
		{
			std::cout << "�򿪷���ʧ�ܣ������룺" << GetLastError() << std::endl;
			break;
		}

		// ֹͣ��������
		SERVICE_STATUS srvStatus;
		bRet = ControlService(hServiceDDK, SERVICE_CONTROL_STOP, &srvStatus);
		if (!bRet)
		{
			std::cout << "ֹͣ����ʧ�ܣ������룺" << GetLastError() << std::endl;
		}

		// ж����������
		bRet = DeleteService(hServiceDDK);
		if (!bRet)
		{
			std::cout << "ж������ʧ�ܣ������룺" << GetLastError() << std::endl;
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