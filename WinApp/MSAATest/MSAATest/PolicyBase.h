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
class __declspec(uuid("{B9EB5E40-136C-4F29-AFF0-77D28F8B69EC}")) CEventFilterPolicy;
class __declspec(uuid("{782D2174-75AD-4B91-92C5-8A0775E15925}")) CBlockWndPolicy;
class __declspec(uuid("{6FD70639-370F-4E21-80F9-5B780969AFF6}")) CReportPolicy;


// 注意：每添加一个类，需要在PolicyFactory.h文件添加如下：
// 1、HRESULT CPolicyFactory::Create(GUID guid)函数内，增加一项
//	  e.g:
//	  hr &= TryCreate<CPreFilterPolicy>(strGuid, guid);

#include "PreFilterPolicy.h"
#include "EventFilterPolicy.h"
#include "BlockWndPolicy.h"
#include "ReportPolicy.h"