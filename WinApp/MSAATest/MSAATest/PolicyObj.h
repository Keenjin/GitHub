#pragma once
#include <map>
#include <atlcomcli.h>
#include "IUnknownImpl.h"

enum EPolicyObjIndex
{
	// 基本信息
	POLICY_INDEX_EVENT = 0,
	POLICY_INDEX_HWND,
	POLICY_INDEX_IDOBJECT,
	POLICY_INDEX_IDCHILD,
	POLICY_INDEX_TID,
	POLICY_INDEX_PID,


	// 其他辅助
	POLICY_INDEX_NEXT_POLICY_GROUP_INDEX = 100,
	POLICY_INDEX_NEXT_POLICY_ITEM_INDEX,

	POLICY_INDEX_MAX
};

__declspec(selectany) IID IID_IPolicyObject = { 0x72B04B47, 0x5AED, 0x432D, 0x86, 0x0E, 0x19, 0x37, 0xC4, 0xA7, 0xEE, 0x6D };

MIDL_INTERFACE("72B04B47-5AED-432D-860E-1937C4A7EE6D")
IPolicyObj : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE SetParam(UINT uIndex, VARIANT varVal) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetParam(UINT uIndex, VARIANT* pvarVal) = 0;
};


template<typename T>
inline T GetValue(IPolicyObj* pObj, EPolicyObjIndex uIndex);

template<>
inline int GetValue(IPolicyObj* pObj, EPolicyObjIndex uIndex)
{
	int nValue = 0;
	if (pObj) {
		CComVariant varVal;
		pObj->GetParam(uIndex, &varVal);
		nValue = varVal.intVal;
	}
	return nValue;
}

template<>
inline DWORD GetValue(IPolicyObj* pObj, EPolicyObjIndex uIndex)
{
	DWORD dwValue = 0;
	if (pObj)	{
		CComVariant varVal;
		pObj->GetParam(uIndex, &varVal);
		dwValue = varVal.uintVal;
	}
	return dwValue;
}

template<>
inline ULONGLONG GetValue(IPolicyObj* pObj, EPolicyObjIndex uIndex)
{
	ULONGLONG ullValue = 0;
	if (pObj) {
		CComVariant varVal;
		pObj->GetParam(uIndex, &varVal);
		ullValue = varVal.ullVal;
	}
	return ullValue;
}

template<>
inline CAtlString GetValue(IPolicyObj* pObj, EPolicyObjIndex uIndex)
{
	CAtlString strValue = 0;
	if (pObj) {
		CComVariant varVal;
		pObj->GetParam(uIndex, &varVal);
		strValue = varVal.bstrVal;
	}
	return strValue;
}