#pragma once
#include "boost\log\core.hpp"
#include "boost\log\trivial.hpp"			// BOOST_LOG_TRIVIAL
#include "boost\log\expressions.hpp"		// boost::log::trivial::severity
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>	// boost::log::add_file_log
#include <boost/log/utility/setup/common_attributes.hpp>		// boost::log::add_common_attributes
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

inline void LogCosonleInit()
{
	// 针对控制台，默认会添加时间戳到日志信息里面去
	boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
}

inline void TestCosonleLog()
{
	LogCosonleInit();
	BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
	BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
	BOOST_LOG_TRIVIAL(info) << "An informational severity message";
	BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
	BOOST_LOG_TRIVIAL(error) << "An error severity message";
	BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";
}

inline void LogFileInit()
{
	// 针对文本，默认不会添加时间戳到日志信息里面去，需要时间戳的话，需要自定义logger
	boost::log::add_file_log("sample.log");
	boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
}

inline void TestFileLog()
{
	LogFileInit();

	BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
	BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
	BOOST_LOG_TRIVIAL(info) << "An informational severity message";
	BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
	BOOST_LOG_TRIVIAL(error) << "An error severity message";
	BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";
}

// 必须配合boost_log_setup-vc120-mt-gd-1_64使用
inline void LogFileInit2()
{
	// 添加时间戳和一些默认参数信息到日志记录里
	boost::log::add_file_log
		(
		boost::log::keywords::file_name = "sample_%N.log",                                        /*< file name pattern >*/
		boost::log::keywords::rotation_size = 10 * 1024 * 1024,                                   /*< rotate files every 10 MiB... >*/
		boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0), /*< ...or at midnight >*/
		boost::log::keywords::format = "[%TimeStamp%]: %Message%"                                 /*< log record format >*/
		);

	boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
}

inline void TestFileLog2()
{
	LogFileInit2();

	boost::log::add_common_attributes();

	using namespace boost::log::trivial;
	boost::log::sources::severity_logger< severity_level > lg;

	BOOST_LOG_SEV(lg, trace) << "A trace severity message";
	BOOST_LOG_SEV(lg, debug) << "A debug severity message";
	BOOST_LOG_SEV(lg, info) << "An informational severity message";
	BOOST_LOG_SEV(lg, warning) << "A warning severity message";
	BOOST_LOG_SEV(lg, error) << "An error severity message";
	BOOST_LOG_SEV(lg, fatal) << "A fatal severity message";

	BOOST_LOG_SEV(lg, fatal) << "test output " << std::hex << 33;
}

inline void TestLog()
{
	// TestCosonleLog();
	// TestFileLog();
	TestFileLog2();
}