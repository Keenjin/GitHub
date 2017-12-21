#pragma once
#include <map>
#include <atlcomcli.h>
#include "IUnknownImpl.h"

enum EPolicyGroupIndex
{
	POLICY_GROUP_INDEX_PREFILTER = 0,
	POLICY_GROUP_INDEX_ADBLOCK,
	POLICY_GROUP_INDEX_DOC_MACRO,
	POLICY_GROUP_INDEX_WND_DISPATCH,
};

enum EPolicyObjIndex
{
	// 基本信息
	POLICY_INDEX_EVENT = 0,
	POLICY_INDEX_HWND,
	POLICY_INDEX_IDOBJECT,
	POLICY_INDEX_IDCHILD,
	POLICY_INDEX_TID,
	POLICY_INDEX_TOPHWND,
	POLICY_INDEX_X,
	POLICY_INDEX_Y,
	POLICY_INDEX_WIDTH,
	POLICY_INDEX_HEIGHT,
	POLICY_INDEX_PID,

	// 其他辅助信息
	POLICY_INDEX_GROUP_END = 100,
	POLICY_INDEX_GROUP_ITEM_END,
	POLICY_INDEX_GROUP_CURRENT_GID,
	POLICY_INDEX_TASK_REMOVE,
	POLICY_INDEX_TASK_ADD,
	POLICY_INDEX_LAST_LOCATION_CHANGE,

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

template<typename T>
inline BOOL SetValue(IPolicyObj* pObj, EPolicyObjIndex uIndex, T value);

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
inline LONG GetValue(IPolicyObj* pObj, EPolicyObjIndex uIndex)
{
	LONG lValue = 0;
	if (pObj)	{
		CComVariant varVal;
		pObj->GetParam(uIndex, &varVal);
		lValue = varVal.lVal;
	}
	return lValue;
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


template<typename T>
inline BOOL SetValue(IPolicyObj* pObj, EPolicyObjIndex uIndex, T value)
{
	BOOL bRet = FALSE;

	do
	{
		if (!pObj)
		{
			break;
		}

		bRet = SUCCEEDED(pObj->SetParam(uIndex, CComVariant(value)));

	} while (FALSE);

	return bRet;
}

template<>
inline BOOL SetValue(IPolicyObj* pObj, EPolicyObjIndex uIndex, DWORD value)
{
	BOOL bRet = FALSE;

	do
	{
		if (!pObj)
		{
			break;
		}

		bRet = SUCCEEDED(pObj->SetParam(uIndex, CComVariant((unsigned int)value)));

	} while (FALSE);

	return bRet;
}

template<>
inline BOOL SetValue(IPolicyObj* pObj, EPolicyObjIndex uIndex, CAtlString value)
{
	BOOL bRet = FALSE;

	do
	{
		if (!pObj)
		{
			break;
		}

		bRet = SUCCEEDED(pObj->SetParam(uIndex, CComVariant(value.GetString())));

	} while (FALSE);

	return bRet;
}