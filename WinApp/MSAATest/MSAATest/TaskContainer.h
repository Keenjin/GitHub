#pragma once
#include <list>

template<typename TObj>
class CTaskContainer
{
	template<typename T,typename TObj,typename TContainer,UINT cThreadCnt> friend class CTaskPool;
public:
	CTaskContainer() {}
	~CTaskContainer() {}

	DWORD GetCount()
	{
		CAutoCriticalSection lock(m_csTaskObjs);
		return m_qTaskObjs.size();
	}

protected:
	// Ö»ÈÃCTaskPoolµ÷ÓÃ
	void AddTail(TObj obj)
	{
		CAutoCriticalSection lock(m_csTaskObjs);
		m_qTaskObjs.push_back(obj);
	}

	TObj PopHead()
	{
		TObj obj;

		CAutoCriticalSection lock(m_csTaskObjs);
		if (!m_qTaskObjs.empty())
		{
			obj = m_qTaskObjs.front();
			m_qTaskObjs.erase(m_qTaskObjs.begin());
		}

		return obj;
	}

	void Clear()
	{
		CAutoCriticalSection lock(m_csTaskObjs);
		m_qTaskObjs.clear();
	}

protected:
	CComAutoCriticalSection	m_csTaskObjs;
	std::list<TObj>	m_qTaskObjs;
};

