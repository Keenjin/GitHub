#pragma once
#include "PolicyObj.h"
#include "IUnknownImpl.h"

class CPolicyBase
	: public IUnknownImplT<IUnknown>
{
public:
	virtual HRESULT STDMETHODCALLTYPE Init() { return S_OK; };
	virtual void STDMETHODCALLTYPE UnInit() {};
	virtual HRESULT STDMETHODCALLTYPE PolicyHandler(IPolicyObj* pPolicyObj) { return S_OK; };
};

class __declspec(uuid("{BCC210E9-91C2-44C8-A5D3-CFE72DAD5DB4}")) CPreFilterPolicy;


// ע�⣺ÿ���һ���࣬��Ҫ��PolicyFactory.h�ļ�������£�
// 1��HRESULT CPolicyFactory::Create(GUID guid)�����ڣ�����һ��
//	  e.g:
//	  hr &= TryCreate<CPreFilterPolicy>(strGuid, guid);

#include "PreFilterPolicy.h"