#pragma once
#include "boost\log\core.hpp"
#include "boost\log\trivial.hpp"			// BOOST_LOG_TRIVIAL
#include <boost/filesystem.hpp>
#include <string>
#include <boost/algorithm/string.hpp>

inline void TestFileSystem1()
{
	std::string strFile = "E:\\BaiduNetdiskDownload\\02.mp4";
	BOOST_LOG_TRIVIAL(info) << "file : " << strFile << ", size : " << boost::filesystem::file_size(strFile) << "B";

	do
	{
		std::cout << "input file or director(!q will quit.) : ";
		std::cin >> strFile;
		if (boost::filesystem::exists(strFile))
		{
			BOOST_LOG_TRIVIAL(info) << "file : " << strFile << " exist.";
			if (boost::filesystem::is_regular_file(strFile))
			{
				BOOST_LOG_TRIVIAL(info) << "file : " << strFile << " is a regular file.";
			}
			if (boost::filesystem::is_directory(strFile))
			{
				BOOST_LOG_TRIVIAL(info) << "file : " << strFile << " is a director.";

				// 遍历文件夹
				for (boost::filesystem::directory_entry& x : boost::filesystem::directory_iterator(strFile))
				{
					BOOST_LOG_TRIVIAL(info) << "sub file : " << x.path();
				}

				std::vector<std::string> v;
				for (auto&& x : boost::filesystem::directory_iterator(strFile))
				{
					v.push_back(boost::algorithm::to_lower_copy(x.path().filename().string()));
				}

				std::sort(v.begin(), v.end());
				for (auto&& x : v)
				{
					BOOST_LOG_TRIVIAL(info) << "sort sub file : " << x;
				}
			}
			if (boost::filesystem::is_symlink(strFile))
			{
				BOOST_LOG_TRIVIAL(info) << "file : " << strFile << " is a symlink.";
			}
			if (boost::filesystem::is_empty(strFile))
			{
				BOOST_LOG_TRIVIAL(info) << "file : " << strFile << " is a empty file.";
			}
			if (boost::filesystem::symbolic_link_exists(strFile))
			{
				BOOST_LOG_TRIVIAL(info) << "file : " << strFile << " contains symlink.";
			}
			if (boost::filesystem::is_other(strFile))
			{
				BOOST_LOG_TRIVIAL(info) << "file : " << strFile << " is other.";
			}
			if (boost::filesystem::is_regular(strFile))
			{
				BOOST_LOG_TRIVIAL(info) << "file : " << strFile << " is regular.";
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(warning) << "file : " << strFile << " not exist.";
		}

	} while (strFile != "!q");
}

// 宽字符窄字符兼容 —— path
inline void TestFileSystem2()
{
	std::string str1 = "Smile";
	std::wstring str2 = L"Smile\263A";
	boost::filesystem::ofstream f1(str1);
	boost::filesystem::ofstream f2(str2);

	if (boost::filesystem::is_directory(str1))
	{
		BOOST_LOG_TRIVIAL(info) << "file : " << str1 << " is dir.";
	}
	if (boost::filesystem::is_regular_file(str1))
	{
		BOOST_LOG_TRIVIAL(info) << "file : " << str1 << " is regular file.";
	}

	if (boost::filesystem::is_directory(str2))
	{
		BOOST_LOG_TRIVIAL(info) << "file : " << str2 << " is dir.";
	}
	if (boost::filesystem::is_regular_file(str2))
	{
		BOOST_LOG_TRIVIAL(info) << "file : " << str2 << " is regular file.";
	}
}

inline void TestFileSystem3()
{
	boost::filesystem::path p = "G:\\GitHub\\WinApp\\CommonLib\\TestLib/ReadMe.txt";
	BOOST_LOG_TRIVIAL(info) << "file : " << p.string();
	BOOST_LOG_TRIVIAL(info) << "native : " << p.native();
	BOOST_LOG_TRIVIAL(info) << "c_str : " << p.c_str();
	BOOST_LOG_TRIVIAL(info) << "filename : " << p.filename();
	BOOST_LOG_TRIVIAL(info) << "root_directory : " << p.root_directory();
	BOOST_LOG_TRIVIAL(info) << "root_name : " << p.root_name();
	BOOST_LOG_TRIVIAL(info) << "root_path : " << p.root_path();
	BOOST_LOG_TRIVIAL(info) << "relative_path : " << p.relative_path();
	BOOST_LOG_TRIVIAL(info) << "parent_path : " << p.parent_path();
	BOOST_LOG_TRIVIAL(info) << "stem : " << p.stem();
	BOOST_LOG_TRIVIAL(info) << "extension : " << p.extension();
}

/*
[2017-07-02 10:20:38.341681] [0x0000086c] [info]    file : G:\GitHub\WinApp\CommonLib\TestLib/ReadMe.txt
[2017-07-02 10:20:38.343680] [0x0000086c] [info]    native : G:\GitHub\WinApp\CommonLib\TestLib/ReadMe.txt
[2017-07-02 10:20:38.344181] [0x0000086c] [info]    c_str : G:\GitHub\WinApp\CommonLib\TestLib/ReadMe.txt
[2017-07-02 10:20:38.344681] [0x0000086c] [info]    filename : "ReadMe.txt"
[2017-07-02 10:20:38.344681] [0x0000086c] [info]    root_directory : "\"
[2017-07-02 10:20:38.345181] [0x0000086c] [info]    root_name : "G:"
[2017-07-02 10:20:38.345181] [0x0000086c] [info]    root_path : "G:\"
[2017-07-02 10:20:38.345681] [0x0000086c] [info]    relative_path : "GitHub\WinApp\CommonLib\TestLib/ReadMe.txt"
[2017-07-02 10:20:38.346182] [0x0000086c] [info]    parent_path : "G:\GitHub\WinApp\CommonLib\TestLib"
[2017-07-02 10:20:38.346683] [0x0000086c] [info]    stem : "ReadMe"
[2017-07-02 10:20:38.347183] [0x0000086c] [info]    extension : ".txt"
*/


inline void TestFileSystem4()
{
	boost::system::error_code err;
	boost::filesystem::ofstream f("test.log");
	bool bRet = boost::filesystem::create_directories("keen1\\test\\", err);
	BOOST_LOG_TRIVIAL(info) << "dir : " << "create_directories(\"keen\\test\\\")" << " create : " << bRet << " err: " << err.value() << " err msg: " << err.message();

	err.clear();
	bRet = boost::filesystem::create_directory("lml", err);
	BOOST_LOG_TRIVIAL(info) << "dir : " << "create_directory(\"lml\")" << " create : " << bRet << " err: " << err.value() << " err msg: " << err.message();
	
	err.clear();
	bRet = boost::filesystem::create_directory("lml1\\test\\", err);
	BOOST_LOG_TRIVIAL(info) << "dir : " << "create_directory(\"lml1\\test\\\")" << " create : " << bRet << " err: " << err.value() << " err msg: " << err.message();

	boost::filesystem::path p = boost::filesystem::complete("test.log");

	BOOST_LOG_TRIVIAL(info) << "file : " << p.string();

	err.clear();
	//boost::filesystem::copy_directory("keen", "lml3", err);
	boost::filesystem::copy_directory(boost::filesystem::complete("keen"), "lml2", err);		// 并不会拷贝目录，只是复制目录的属性
	BOOST_LOG_TRIVIAL(info) << "copy_directory" << " err: " << err.value() << " err msg: " << err.message();

	err.clear();
	boost::filesystem::copy_file("keen", "lml", err);		// 只能拷贝单个文件
	BOOST_LOG_TRIVIAL(info) << "copy_file" << " err: " << err.value() << " err msg: " << err.message();

	BOOST_LOG_TRIVIAL(info) << "current dir: " << boost::filesystem::current_path();
}

inline void TestFileSystem()
{
	//TestFileSystem1();

	//TestFileSystem2();

	//TestFileSystem3();

	TestFileSystem4();
}