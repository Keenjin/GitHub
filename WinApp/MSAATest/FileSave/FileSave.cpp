// FileSave.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "FileClusterTag.h"
#include <iostream>
#include <atlstr.h>
#include <atlpath.h>


int _tmain(int argc, _TCHAR* argv[])
{
	std::wcout.imbue(std::locale("chs"));

	CFileClusterTag fileTag;
	std::vector<CAtlString> vecDisk;
	fileTag.EnumDiskDevice(vecDisk);
	//fileTag.AddTag(L"I:\\HCI-����թ�ĵ����ݱ�������-Luxozhang-201703281739.fw.png", L"I:\\HCI-����թ�ĵ����ݱ�������-Luxozhang-201703281739.fw_tag.png", FALSE);
	//fileTag.DiskRestore();
	/*std::cout << "���뷶����" << std::endl;
	WCHAR szExePath[1024] = { 0 };
	GetModuleFileName(NULL, szExePath, 1023);
	ATL::CAtlString strOutput;
	strOutput.Format(L"\"%s\" C:\\test_with_tag.txt D:\\Restore\\", szExePath);
	
	std::wcout << std::wstring(strOutput.GetString()).c_str() << std::endl;

	if (argc != 3)
	{
		std::cout << "�����������" << std::endl;
		return 0;
	}

	CAtlString strSrcFile = argv[1];
	CAtlString strDstDir = argv[2];

	if (!ATLPath::FileExists(strSrcFile))
	{
		std::wcout << L"Դ�ļ�������(" << std::wstring(strSrcFile.GetString()).c_str() << L")" << std::endl;
		return 0;
	}

	if (!ATLPath::FileExists(strDstDir))
	{
		std::wcout << L"Ŀ��Ŀ¼������(" << std::wstring(strDstDir.GetString()).c_str() << L")" << std::endl;
		return 0;
	}

	if (strDstDir.Right(1) != L'\\' && strDstDir.Right(1) != L'/')
	{
		std::wcout << L"Ŀ��Ŀ¼��Ҫ�Է�б�ܽ�β�������룺" << std::wstring(strDstDir.GetString()).c_str() << L"\\" << std::endl;
		return 0;
	}

	std::wcout << std::endl;
	std::wcout << L"��ʼ��ȡ ..." << std::endl;

	CFileClusterTag fileTag;
	CAtlString strFileOld;
	fileTag.RemoveTag(strSrcFile, strDstDir, strFileOld.GetBufferSetLength(1024), 1023, FALSE);
	strFileOld.ReleaseBuffer();

	std::wcout << L"��������" << std::endl;
	std::wcout << L"ԭʼ�ļ�����" << std::wstring(strFileOld.GetString()).c_str() << std::endl;*/

	return 0;
}

