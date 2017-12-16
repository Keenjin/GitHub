#include "stdafx.h"
#include "PolicyConfig.h"
#include <atlpath.h>

// XML≈‰÷√Ω·µ„
#define POLICY_CONFIG_ROOT						"Config"
#define POLICY_CONFIG_ROOT_VER					"version"

#define POLICY_CONFIG_POLICY					"Policy"
#define POLICY_CONFIG_POLICY_GROUP				"PolicyGroup"
#define POLICY_CONFIG_POLICY_GROUP_ATTR_NAME	"name"
#define POLICY_CONFIG_POLICY_GROUP_ITEM			"PolicyItem"
#define POLICY_CONFIG_POLICY_GROUP_ITEMATTR_NAME	"name"
#define POLICY_CONFIG_POLICY_GROUP_ITEMATTR_GUID	"guid"

CPolicyConfig::CPolicyConfig()
{
}

CPolicyConfig::~CPolicyConfig()
{
}

BOOL CPolicyConfig::Load()
{
	BOOL bRet = FALSE;

	do
	{
		CAtlString strFilePath = L"PolicyConfig.xml";
		if (!ATLPath::FileExists(strFilePath))
		{
			break;
		}
		LOG_PRINT(L"%s, config file(%s)", __FUNCTIONW__, strFilePath);

		if (!m_xmlParser.Load(strFilePath))
		{
			break;
		}

		if (!m_xmlParser.FindElem(POLICY_CONFIG_ROOT))
		{
			break;
		}

		m_dwVersion = m_xmlParser.GetAttrib<DWORD>(POLICY_CONFIG_ROOT_VER);
		LOG_PRINT(L"%s, config version(%d)", __FUNCTIONW__, m_dwVersion);

		m_xmlParser.IntoElem();

		if (!m_xmlParser.FindElem(POLICY_CONFIG_POLICY))
		{
			break;
		}

		m_xmlParser.IntoElem();
		while (m_xmlParser.FindElem(POLICY_CONFIG_POLICY_GROUP))
		{
			CAtlString strValue = m_xmlParser.GetAttrib<CAtlString>(POLICY_CONFIG_POLICY_GROUP_ATTR_NAME);
			LOG_PRINT(L"%s, PolicyGroup name(%s)", __FUNCTIONW__, strValue);

			m_xmlParser.IntoElem();
			while (m_xmlParser.FindElem(POLICY_CONFIG_POLICY_GROUP_ITEM))
			{
				CAtlString strName = m_xmlParser.GetAttrib<CAtlString>(POLICY_CONFIG_POLICY_GROUP_ITEMATTR_NAME);
				CAtlString strGuid = m_xmlParser.GetAttrib<CAtlString>(POLICY_CONFIG_POLICY_GROUP_ITEMATTR_NAME);
				LOG_PRINT(L"%s, PolicyItem name(%s) guid(%s)", __FUNCTIONW__, strName, strGuid);
			}
			m_xmlParser.OutOfElem();
		}
		m_xmlParser.OutOfElem();
		m_xmlParser.OutOfElem();
		
		bRet = TRUE;

	} while (FALSE);

	if (!bRet)
	{
		UnLoad();
	}
	
	return bRet;
}

void CPolicyConfig::UnLoad()
{
	m_xmlParser.UnLoad();
}