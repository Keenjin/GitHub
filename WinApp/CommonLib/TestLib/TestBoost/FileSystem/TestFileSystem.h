#pragma once
#include "boost\log\core.hpp"
#include "boost\log\trivial.hpp"			// BOOST_LOG_TRIVIAL
#include <boost/filesystem.hpp>
#include <string>
#include <boost/algorithm/string.hpp>

inline void TestFileSystem()
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

				// ±éÀúÎÄ¼þ¼Ð
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