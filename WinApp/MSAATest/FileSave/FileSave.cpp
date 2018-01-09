// FileSave.cpp : 定义控制台应用程序的入口点。
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
	//fileTag.AddTag(L"I:\\HCI-防敲诈文档备份保护方案-Luxozhang-201703281739.fw.png", L"I:\\HCI-防敲诈文档备份保护方案-Luxozhang-201703281739.fw_tag.png", FALSE);
	//fileTag.DiskRestore();
	/*std::cout << "输入范例：" << std::endl;
	WCHAR szExePath[1024] = { 0 };
	GetModuleFileName(NULL, szExePath, 1023);
	ATL::CAtlString strOutput;
	strOutput.Format(L"\"%s\" C:\\test_with_tag.txt D:\\Restore\\", szExePath);
	
	std::wcout << std::wstring(strOutput.GetString()).c_str() << std::endl;

	if (argc != 3)
	{
		std::cout << "输入参数错误" << std::endl;
		return 0;
	}

	CAtlString strSrcFile = argv[1];
	CAtlString strDstDir = argv[2];

	if (!ATLPath::FileExists(strSrcFile))
	{
		std::wcout << L"源文件不存在(" << std::wstring(strSrcFile.GetString()).c_str() << L")" << std::endl;
		return 0;
	}

	if (!ATLPath::FileExists(strDstDir))
	{
		std::wcout << L"目标目录不存在(" << std::wstring(strDstDir.GetString()).c_str() << L")" << std::endl;
		return 0;
	}

	if (strDstDir.Right(1) != L'\\' && strDstDir.Right(1) != L'/')
	{
		std::wcout << L"目标目录需要以反斜杠结尾，请输入：" << std::wstring(strDstDir.GetString()).c_str() << L"\\" << std::endl;
		return 0;
	}

	std::wcout << std::endl;
	std::wcout << L"开始提取 ..." << std::endl;

	CFileClusterTag fileTag;
	CAtlString strFileOld;
	fileTag.RemoveTag(strSrcFile, strDstDir, strFileOld.GetBufferSetLength(1024), 1023, FALSE);
	strFileOld.ReleaseBuffer();

	std::wcout << L"操作结束" << std::endl;
	std::wcout << L"原始文件名：" << std::wstring(strFileOld.GetString()).c_str() << std::endl;*/

	return 0;
}

