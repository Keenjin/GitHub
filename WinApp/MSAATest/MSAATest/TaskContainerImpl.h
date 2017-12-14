#pragma once
#include "TaskContainer.h"
#include "PolicyObj.h"

class CTaskContainerImpl
	: public CTaskContainer<CComPtr<IPolicyObj>>
{
public:
	CTaskContainerImpl();
	~CTaskContainerImpl();
};