#include "stdafx.h"
#include "PreFilterPolicy.h"

CPreFilterPolicy::CPreFilterPolicy()
{
}

CPreFilterPolicy::~CPreFilterPolicy()
{
}

HRESULT STDMETHODCALLTYPE CPreFilterPolicy::PolicyHandler(IPolicyObj* pPolicyObj)
{
	// 过滤规则：
	// 1、系统基本窗口过滤（e.g：任务栏、工具栏、桌面、开始菜单等、计算机等） - 基于系统固定窗口属性的
	// 2、气泡等小型窗口的过滤（但是，需要有排除列表，比如一些小红包等）
	// 3、隐藏窗口、无效窗口的过滤
	// 4、窗口事件的过滤
	// 5、Location事件处理


	// 包含窗口信息获取，以及缓存表

	return S_OK;
}