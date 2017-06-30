#pragma once

#include "boost\function.hpp"

void OnClickBtn1(void* pParam)
{
	std::cout << "OnClickBtn1" << std::endl;
}

void OnClickBtn2(void* pParam)
{
	std::cout << "OnClickBtn2" << std::endl;
}

class CAFunction
{
public:
	void OnShow1()
	{
		std::cout << "CAFunction OnShow1" << std::endl;
	}

	void OnShow2()
	{
		std::cout << "CAFunction OnShow2" << std::endl;
	}
};

class CBFunction
{
public:
	void OnShow1()
	{
		std::cout << "CBFunction OnShow1" << std::endl;
	}

	void OnShow2()
	{
		std::cout << "CBFunction OnShow2" << std::endl;
	}
};

class CFuncTask
{
public:
	template<typename T>
	void PostTask(T* obj, boost::function<void(T*)> f)
	{
		if (f)
		{
			f(obj);
		}
	}
};

inline void TestFunction(bool bClick)
{
	boost::function<void(void*)> f;
	boost::function<void(CAFunction*)> f1;
	CAFunction aF;
	CBFunction bF;
	CFuncTask ft;
	ft.PostTask<CAFunction>(&aF, &CAFunction::OnShow1);
	ft.PostTask<CBFunction>(&bF, &CBFunction::OnShow2);

	if (bClick)
	{
		f = OnClickBtn1;
		f1 = &CAFunction::OnShow1;
	}
	else
	{
		f = OnClickBtn2;
		f1 = &CAFunction::OnShow2;
	}

	if (f)
	{
		f(NULL);
		f1(&aF);
	}

}