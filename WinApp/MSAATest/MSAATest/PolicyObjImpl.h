#pragma once
#include "PolicyObj.h"
#include "AutoCriticalSection.h"

class CPolicyObj
	: public IUnknownImplT<IPolicyObj>
{
public:
	static HRESULT Create(IPolicyObj** ppObj)
	{
		*ppObj = new CPolicyObj;
		if (NULL != *ppObj)
		{
			(*ppObj)->AddRef();
			return S_OK;
		}
		return E_FAIL;
	}

	virtual HRESULT STDMETHODCALLTYPE SetParam(UINT uIndex, VARIANT varVal)
	{
		CAutoCriticalSection lock(m_csForMap);
		m_mapKeyValue.insert(std::pair<UINT, ATL::CComVariant>(uIndex, varVal));
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetParam(UINT uIndex, VARIANT* pvarVal)
	{
		CAutoCriticalSection lock(m_csForMap);
		if (m_mapKeyValue.find(uIndex) != m_mapKeyValue.end())
		{
			*pvarVal = m_mapKeyValue[uIndex];
			return S_OK;
		}

		return E_FAIL;
	}

private:
	CPolicyObj(){}
	virtual ~CPolicyObj() 
	{
		CAutoCriticalSection lock(m_csForMap);
		m_mapKeyValue.clear();
	}

private:
	ATL::CComAutoCriticalSection m_csForMap;
	std::map<UINT, ATL::CComVariant> m_mapKeyValue;
};

