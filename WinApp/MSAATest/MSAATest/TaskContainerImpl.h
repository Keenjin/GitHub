#pragma once
#include "TaskContainer.h"
#include "PolicyObj.h"
#include "AutoCriticalSection.h"

enum ETaskObjFlag
{
	TASK_OBJ_FLAG_NONE = 0,
	TASK_OBJ_FLAG_REMOVE,
};

class CTaskContainerImpl
	: public CTaskContainer<CComPtr<IPolicyObj>>
{
public:
	CTaskContainerImpl();
	~CTaskContainerImpl();

	void RemoveWnd(HWND hWnd);
	DWORD GetFlag(HWND hWnd);
};