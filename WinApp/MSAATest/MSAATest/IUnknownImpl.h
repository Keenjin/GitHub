#pragma once
#include <atlcom.h>

//////////////////////////////////////////////////////////////////////////
// IUnknownImplT
template< class T >
class ATL_NO_VTABLE IUnknownImplT : public T
{
protected:
	volatile LONG m_lRefs;

public:
	IUnknownImplT(
		void)
		: m_lRefs(0)
	{
	}
	virtual ~IUnknownImplT(
		void)
	{
		m_lRefs = -(LONG_MAX / 2);
	}

public:
	// IUnknown
	virtual ULONG STDMETHODCALLTYPE AddRef(
		void)
	{
		return InterlockedIncrement(&m_lRefs);
	}

	virtual ULONG STDMETHODCALLTYPE Release(
		void)
	{
		LONG lRef = InterlockedDecrement(&m_lRefs);
		if (lRef == 0)
			delete this;
		if (lRef < -(LONG_MAX / 2))
		{
			ATLASSERT(0 && _T("lRef < -(LONG_MAX / 2)"));
		}
		return lRef;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(
		REFIID riid,
		void** ppv)
	{
		if (riid == __uuidof(T))
		{
			*ppv = (T*)this;
		}
		else if (riid == IID_IUnknown)
		{
			*ppv = (IUnknown*)this;
		}
		else
		{
			*ppv = NULL;
			return E_NOINTERFACE;
		}
		if (*ppv)
			AddRef();
		return S_OK;
	}
};