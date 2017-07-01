#pragma once
#include "boost\log\trivial.hpp"

inline void TestLog()
{
	BOOST_LOG_TRIVIAL(trace) << "a trace log";
}