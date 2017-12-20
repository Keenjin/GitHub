#include "stdafx.h"
#include "PolicyConfig.h"
#include <atlpath.h>
#include "XmlParser.h"

// XML≈‰÷√Ω·µ„
#define POLICY_CONFIG_ROOT						"Config"
#define POLICY_CONFIG_ROOT_VER					"version"

#define POLICY_CONFIG_POLICY					"Policy"
#define POLICY_CONFIG_POLICY_GROUP				"PolicyGroup"
#define POLICY_CONFIG_POLICY_GROUP_ATTR_GID		"gid"
#define POLICY_CONFIG_POLICY_GROUP_ATTR_NAME	"name"
#define POLICY_CONFIG_POLICY_GROUP_ITEM			"PolicyItem"
#define POLICY_CONFIG_POLICY_GROUP_ITEMATTR_NAME	"name"
#define POLICY_CONFIG_POLICY_GROUP_ITEMATTR_GUID	"guid"

CPolicyConfig::CPolicyConfig()
	: m_dwVersion(0)
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

		CXmlParser<> xmlParser;
		if (!xmlParser.Load(strFilePath))
		{
			break;
		}

		if (!xmlParser.FindElem(POLICY_CONFIG_ROOT))
		{
			break;
		}

		m_dwVersion = xmlParser.GetAttrib<DWORD>(POLICY_CONFIG_ROOT_VER);
		LOG_PRINT(L"%s, config version(%d)", __FUNCTIONW__, m_dwVersion);

		xmlParser.IntoElem();

		if (!xmlParser.FindElem(POLICY_CONFIG_POLICY))
		{
			break;
		}

		CAutoCriticalSection lock(m_csForPolicyCfg);
		xmlParser.IntoElem();
		while (xmlParser.FindElem(POLICY_CONFIG_POLICY_GROUP))
		{
			POLICY_GROUP policyGroup;

			policyGroup.dwGID = xmlParser.GetAttrib<DWORD>(POLICY_CONFIG_POLICY_GROUP_ATTR_GID);
			policyGroup.strName = xmlParser.GetAttrib<CAtlString>(POLICY_CONFIG_POLICY_GROUP_ATTR_NAME);
			LOG_PRINT(L"%s, PolicyGroup gid(%d),name(%s)", __FUNCTIONW__, policyGroup.dwGID, policyGroup.strName);

			xmlParser.IntoElem();
			while (xmlParser.FindElem(POLICY_CONFIG_POLICY_GROUP_ITEM))
			{
				POLICY_ITEM policyItem;

				policyItem.strName = xmlParser.GetAttrib<CAtlString>(POLICY_CONFIG_POLICY_GROUP_ITEMATTR_NAME);
				policyItem.strGuid = xmlParser.GetAttrib<CAtlString>(POLICY_CONFIG_POLICY_GROUP_ITEMATTR_GUID);
				LOG_PRINT(L"%s, PolicyItem name(%s) guid(%s)", __FUNCTIONW__, policyItem.strName, policyItem.strGuid);

				Add(policyItem.strGuid);

				policyGroup.vecPolicyItems.push_back(policyItem);
			}
			xmlParser.OutOfElem();

			m_vecPolicyCfg.push_back(policyGroup);
		}
		xmlParser.OutOfElem();
		
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
	RemoveAll();

	{
		CAutoCriticalSection lock(m_csForPolicyCfg);
		m_vecPolicyCfg.clear();
	}
}

UINT CPolicyConfig::GetPolicyGroupCnt()
{
	CAutoCriticalSection lock(m_csForPolicyCfg);
	return m_vecPolicyCfg.size();
}

UINT CPolicyConfig::GetPolicyItemCount(UINT uIndex)
{
	UINT uCount = 0;
	CAutoCriticalSection lock(m_csForPolicyCfg);
	if (uIndex < m_vecPolicyCfg.size())
	{
		uCount = m_vecPolicyCfg[uIndex].vecPolicyItems.size();
	}
	return uCount;
}

CAtlString CPolicyConfig::GetPolicyItemGuid(UINT uGroupIndex, UINT uItemIndex)
{
	CAtlString strGuid;

	CAutoCriticalSection lock(m_csForPolicyCfg);
	if (uGroupIndex < m_vecPolicyCfg.size() && m_vecPolicyCfg[uGroupIndex].vecPolicyItems.size() > uItemIndex)
	{
		strGuid = m_vecPolicyCfg[uGroupIndex].vecPolicyItems[uItemIndex].strGuid;
	}

	return strGuid;
}

UINT CPolicyConfig::GetGroupID(UINT uIndex)
{
	UINT uID = 0;
	CAutoCriticalSection lock(m_csForPolicyCfg);
	if (uIndex < m_vecPolicyCfg.size())
	{
		uID = m_vecPolicyCfg[uIndex].uGID;
	}
	return uID;
}