#pragma once
#include "rapid\rapidxml.hpp"
#include "Log.h"
#include <atlfile.h>
#include <typeinfo.h>
#include <iostream>
#include <sstream>

template<typename T = char>
class CXmlParser
{
public:
	CXmlParser();
	~CXmlParser();

	BOOL Load(LPCWSTR lpstrFilePath);
	void UnLoad();
	BOOL LoadFromData(T* data);
	BOOL FindElem(const T* ElemName);
	void IntoElem();
	void OutOfElem();
	template<typename TRet>
	TRet GetAttrib(const T* AttrKey);
	LPCWSTR GetValue(const T* ValueKey);
	LPCWSTR GetFilePath();

private:
	BOOL LoadFile(LPCWSTR lpstrFilePath, CAutoVectorPtr<char>& pFileData);
	template<typename TRet>
	TRet Convert(T* Value);

private:
	rapidxml::xml_document<T>	m_xmlDoc;
	ATL::CAtlString	m_strFile;

	rapidxml::xml_node<T>*	m_pNodeFirst;
	rapidxml::xml_node<T>*	m_pNodeNext;
};

template<typename T>
inline CXmlParser<T>::CXmlParser()
	: m_pNodeFirst(NULL)
	, m_pNodeNext(NULL)
{

}

template<typename T>
inline CXmlParser<T>::~CXmlParser()
{
	UnLoad();
}

template<>
inline BOOL CXmlParser<char>::Load(LPCWSTR lpstrFilePath)
{
	BOOL bRet = FALSE;
	m_strFile = lpstrFilePath;

	do
	{
		CAutoVectorPtr<char> pData;
		if (!LoadFile(lpstrFilePath, pData))
		{
			break;
		}

		if (!pData.m_p)
		{
			break;
		}

		if (!LoadFromData(pData.m_p))
		{
			break;
		}

		bRet = TRUE;

	} while (FALSE);

	return bRet;
}

template<>
inline BOOL CXmlParser<wchar_t>::Load(LPCWSTR lpstrFilePath)
{
	BOOL bRet = FALSE;
	m_strFile = lpstrFilePath;

	do
	{
		CAutoVectorPtr<char> pData;
		if (!LoadFile(lpstrFilePath, pData))
		{
			break;
		}

		if (!pData.m_p)
		{
			break;
		}

		CAutoVectorPtr<wchar_t> pwData;
		pwData.Allocate(_msize(pData.m_p) / sizeof(char));
		pwData.Attach(CA2W(pData.Detach(), CP_UTF8));
		if (!pwData.m_p)
		{
			break;
		}

		bRet = LoadFromData(pwData.m_p);

	} while (FALSE);

	return bRet;
}

template<typename T>
inline void CXmlParser<T>::UnLoad()
{
	m_pNodeFirst = NULL;
	m_pNodeNext = NULL;
	m_xmlDoc.clear();
}

template<typename T>
inline BOOL CXmlParser<T>::LoadFile(LPCWSTR lpstrFilePath, CAutoVectorPtr<char>& pFileData)
{
	BOOL bRet = FALSE;

	do
	{
		ATL::CAtlFile file;
		HRESULT hr = file.Create(
			lpstrFilePath,
			GENERIC_READ,
			FILE_SHARE_READ,
			OPEN_EXISTING);
		if (FAILED(hr))
		{
			break;
		}
		ULONG nSize = 0;
		{
			ULONGLONG nFileSize = 0;
			file.GetSize(nFileSize);
			if (nFileSize == 0)
			{
				break;
			}
			nSize = (ULONG)nFileSize;
		}

		if (!pFileData.Allocate(nSize))
		{
			break;
		}

		if (FAILED(file.Read(
			pFileData.m_p,
			nSize)))
		{
			break;
		}
		file.Close();

		bRet = TRUE;

	} while (FALSE);

	return bRet;
}

// 显示实例化，并且禁止其他实例化
template<>
inline BOOL CXmlParser<char>::LoadFromData(char* data)
{
	BOOL bRet = FALSE;

	do
	{
		if (!data)
		{
			break;
		}

		//data = CA2A(data, CP_UTF8);

		try
		{
			m_xmlDoc.parse<0>(data);
		}
		catch (rapidxml::parse_error& err)
		{
			LOG_PRINT(L"%s, xml(%s) parse error, where(%s) what(%s)",
				__FUNCTIONW__,
				m_strFile,
				/*(LPCWSTR)((typeid(err.where<T>()) == typeid(char*)) ? CA2W(err.where<T>(), CP_UTF8) : err.where<T>()),*/
				(LPCWSTR)CA2W(err.where<char>(), CP_UTF8),
				(LPCWSTR)(CA2W(err.what(), CP_UTF8)));
			break;
		}

		bRet = TRUE;

	} while (FALSE);
	
	return bRet;
}

template<>
inline BOOL CXmlParser<wchar_t>::LoadFromData(wchar_t* data)
{
	BOOL bRet = FALSE;

	do
	{
		if (!data)
		{
			break;
		}

		try
		{
			m_xmlDoc.parse<0>(data);
		}
		catch (rapidxml::parse_error& err)
		{
			LOG_PRINT(L"%s, xml(%s) parse error, where(%s) what(%s)",
				__FUNCTIONW__,
				m_strFile,
				/*(LPCWSTR)((typeid(err.where<T>()) == typeid(char*)) ? CA2W(err.where<T>(), CP_UTF8) : err.where<T>()),*/
				err.where<wchar_t>(),
				(LPCWSTR)(CA2W(err.what(), CP_UTF8)));
			break;
		}

		bRet = TRUE;

	} while (FALSE);

	return bRet;
}

template<typename T>
inline BOOL CXmlParser<T>::FindElem(const T* ElemName)
{
	rapidxml::xml_node<T>* pNode = NULL;
	if (!m_pNodeFirst)
	{
		pNode = m_xmlDoc.first_node(ElemName);
		if (pNode)
		{
			m_pNodeFirst = pNode;
		}
	}
	else
	{
		if (!m_pNodeNext)
		{
			pNode = m_pNodeFirst->first_node(ElemName);
			if (pNode)
			{
				m_pNodeNext = pNode;
			}
		}
		else
		{
			pNode = m_pNodeNext->next_sibling(ElemName);
			if (pNode)
			{
				m_pNodeNext = pNode;
			}
		}
	}
	
	return (pNode != NULL);
}

template<typename T>
inline void CXmlParser<T>::IntoElem()
{
	if (m_pNodeNext)
	{
		m_pNodeFirst = m_pNodeNext;
		m_pNodeNext = NULL;
	}
}

template<typename T>
inline void CXmlParser<T>::OutOfElem()
{
	if (m_pNodeFirst)
	{
		m_pNodeFirst = m_pNodeFirst->parent();
		m_pNodeNext = m_pNodeFirst;
	}
}

template<>
template<typename TRet>
inline TRet CXmlParser<char>::GetAttrib(const char* AttrKey)
{
	TRet ret;

	try
	{
		std::istringstream iss(m_pNodeNext->first_attribute(AttrKey)->value());
		iss >> ret;
	}
	catch (...)
	{ }

	return ret;
}

template<>
template<typename TRet>
inline TRet CXmlParser<wchar_t>::GetAttrib(const wchar_t* AttrKey)
{
	TRet ret;

	try
	{
		std::wistringstream iss(m_pNodeNext->first_attribute(AttrKey).value());
		iss >> ret;
	}
	catch (...)
	{ }

	return ret;
}

template<>
template<>
inline CAtlString CXmlParser<char>::GetAttrib(const char* AttrKey)
{
	CAtlString ret;

	try
	{
		ret = CA2W(m_pNodeNext->first_attribute(AttrKey)->value(), CP_UTF8);
	}
	catch (...)
	{ }

	return ret;
}

template<>
template<>
inline CAtlString CXmlParser<wchar_t>::GetAttrib(const wchar_t* AttrKey)
{
	CAtlString ret;

	try
	{
		ret = m_pNodeNext->first_attribute(AttrKey)->value();
	}
	catch (...)
	{ }

	return ret;
}

template<typename T>
inline LPCWSTR CXmlParser<T>::GetValue(const T* ValueKey)
{
	ATL::CAtlString strValue;

	try
	{
		if (typeid(m_pNodeNext->value()) == typeid(char*))
		{
			strValue = CA2W(m_pNodeNext->value(), CP_UTF8);
		}
		else
		{
			strValue = m_pNodeNext->value();
		}
	}
	catch (...)
	{ }
	
	return strValue
}

template<typename T>
inline LPCWSTR CXmlParser<T>::GetFilePath()
{
	return m_strFile.GetString();
}