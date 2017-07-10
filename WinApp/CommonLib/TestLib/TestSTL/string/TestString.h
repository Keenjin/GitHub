#pragma once
#include <string>
#include "boost\log\core.hpp"
#include "boost\log\trivial.hpp"			// BOOST_LOG_TRIVIAL

inline void TestString()
{
	std::string str1 = "1111";
	unsigned int i = 199;

	BOOST_LOG_TRIVIAL(info) << "str1: " << str1 << ", std::stoi(str1):" << std::stoi(str1);
	BOOST_LOG_TRIVIAL(info) << "i: " << i << ", std::to_string(i):" << std::to_string(i);
}