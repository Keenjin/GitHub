#pragma once
#include "PolicyBase.h"
#include <map>

class CPolicyFactory
{
public:
	CPolicyFactory() {}
	~CPolicyFactory() {}

	template<typename T>
	static HRESULT Create(CPolicyBase** ppPolicy)
	{
		HRESULT hr = E_FAIL;

		do
		{
			if (!ppPolicy)
			{
				break;
			}

			*ppPolicy = new T;
			if (!(*ppPolicy))
			{
				break;
			}

			(*ppPolicy)->AddRef();

			if (FAILED((*ppPolicy)->Init()))
			{
				break;
			}

			hr = S_OK;

		} while (FALSE);

		return hr;
	}

	HRESULT Add(const CAtlString& strGuid)
	{
		HRESULT hr = E_FAIL;

		do
		{
			if (strGuid.IsEmpty())
			{
				break;
			}

			{
				CAutoCriticalSection lock(m_csForPolicy);
				// 已经创建过了，就不要创建了
				if (m_mapPolicy.find(strGuid) != m_mapPolicy.end())
				{
					hr = S_OK;
					break;
				}
			}
			
			if (Create(strGuid))
			{
				break;
			}

		} while (FALSE);

		return hr;
	}

	void Remove(const CAtlString& strGuid)
	{
		CAutoCriticalSection lock(m_csForPolicy);
		if ((m_mapPolicy.find(strGuid) != m_mapPolicy.end()) && m_mapPolicy[strGuid])
		{
			m_mapPolicy[strGuid]->UnInit();
		}
		m_mapPolicy.erase(strGuid);
	}

	void RemoveAll()
	{
		CAutoCriticalSection lock(m_csForPolicy);
		for (std::map<CAtlString, CComPtr<CPolicyBase>>::iterator itor = m_mapPolicy.begin(); 
			itor != m_mapPolicy.end(); itor++)
		{
			if (itor->second)
			{
				itor->second->UnInit();
			}
		}
		m_mapPolicy.clear();
	}

	CPolicyBase* GetPolicy(const CAtlString& strGuid)
	{
		CAutoCriticalSection lock(m_csForPolicy);
		CComPtr<CPolicyBase> pPolicy;
		if (m_mapPolicy.find(strGuid) != m_mapPolicy.end())
		{
			pPolicy = m_mapPolicy[strGuid];
		}
		return pPolicy;
	}

private:
	// 所有的对象，需要在这里创建
	HRESULT Create(const CAtlString& strGuid)
	{
		HRESULT hr = E_FAIL;

		GUID guid;
		CLSIDFromString(strGuid, &guid);

		hr &= TryCreate<CPreFilterPolicy>(strGuid, guid);
		hr &= TryCreate<CEventFilterPolicy>(strGuid, guid);
		hr &= TryCreate<CQueryWndInfoPolicy>(strGuid, guid);
		hr &= TryCreate<CBlockWndPolicy>(strGuid, guid);
		hr &= TryCreate<CReportPolicy>(strGuid, guid);

		return hr;
	}

	template<typename T>
	HRESULT TryCreate(const CAtlString& strGuid, GUID guid)
	{
		HRESULT hr = E_FAIL;

		if (guid == __uuidof(T))
		{
			CComPtr<CPolicyBase> pPolicy;
			if (SUCCEEDED(Create<T>(&pPolicy)))
			{
				CAutoCriticalSection lock(m_csForPolicy);
				m_mapPolicy.insert(std::pair<CAtlString, CComPtr<CPolicyBase>>(strGuid, pPolicy));
				hr = S_OK;
			}
		}

		return hr;
	}

protected:
	ATL::CComAutoCriticalSection m_csForPolicy;
	std::map<CAtlString, CComPtr<CPolicyBase>>	m_mapPolicy;
};
