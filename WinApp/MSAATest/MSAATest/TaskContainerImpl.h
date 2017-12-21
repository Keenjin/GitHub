#pragma once
#include "TaskContainer.h"
#include "PolicyObj.h"
#include "AutoCriticalSection.h"

class CTaskContainerImpl
	: public CTaskContainer<CComPtr<IPolicyObj>>
{
public:
	CTaskContainerImpl();
	~CTaskContainerImpl();

	void RemoveWnd(HWND hWnd);
};