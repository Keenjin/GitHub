#pragma once
#include "boost\log\core.hpp"
#include "boost\log\trivial.hpp"			// BOOST_LOG_TRIVIAL
#include "boost\format.hpp"

inline void TestFormat()
{
	BOOST_LOG_TRIVIAL(info) << boost::format("this is %1%, it's price : %2%") % "testfoot" % 12.3;
	BOOST_LOG_TRIVIAL(info) << boost::format("(x,y)=(%+5d,%d)\n") % -23 % 25;
	BOOST_LOG_TRIVIAL(info) << boost::format("(x,y)=(%|+5d|,%d)\n") % -23 % 25;
}