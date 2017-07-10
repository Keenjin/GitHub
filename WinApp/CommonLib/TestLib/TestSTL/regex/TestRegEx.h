#pragma once
#include <regex>		// c++11才有的
#include "boost\log\core.hpp"
#include "boost\log\trivial.hpp"			// BOOST_LOG_TRIVIAL

/******************************
* purpose: 正则表达式查找匹配字符
*           形如: src="abcde" match="cd" 匹配成功
*                   src="abcde" match="ce" 匹配失败
src="abcde" match="c.*e" 匹配成功
* param: src 来源字符串 match 匹配关键字 bCase 为false 表示不区分大小写
*
* return: 是否匹配成功
*******************************/
bool RegexSearch(const std::string& src, const std::string& match, bool bCase)
{
	std::regex_constants::syntax_option_type optype = std::regex_constants::ECMAScript;
	if (!bCase)
		optype = std::regex_constants::icase;
	std::regex parter(match, optype);
	std::smatch matchres;

	return std::regex_search(src, matchres, parter);
}

void RegexPrintMatch(const std::string& src, const std::string& match, bool bCase)
{
	std::regex_constants::syntax_option_type optype = std::regex_constants::ECMAScript;
	if (!bCase)
		optype = std::regex_constants::icase;
	std::regex parter(match, optype);
	std::smatch matchres;

	std::string::const_iterator start = src.begin();
	while (std::regex_search(start, src.end(), matchres, parter))
	{
		start = matchres[0].second;
		BOOST_LOG_TRIVIAL(info) << "match: " << matchres.str();
	}
}

inline void TestRegex()
{
	std::string strSrc = "C:\\users\\keenjin\\appdata\\keenain\\test.txt";
	std::string strMatch = "keen[abj]in";
	bool bSearch = RegexSearch(strSrc, strMatch, false);

	BOOST_LOG_TRIVIAL(info) << "src : " << strSrc << ", match : " << strMatch << ", Regex Search Result: " << bSearch;

	RegexPrintMatch(strSrc, strMatch, false);
}