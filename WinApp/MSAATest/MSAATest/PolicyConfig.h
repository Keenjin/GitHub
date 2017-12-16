#pragma once
#include "XmlParser.h"

class CPolicyConfig
{
public:
	CPolicyConfig();
	~CPolicyConfig();

	BOOL Load();
	void UnLoad();

private:
	CXmlParser<>	m_xmlParser;
	DWORD	m_dwVersion;
};

