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
	// ���˹���
	// 1��ϵͳ�������ڹ��ˣ�e.g���������������������桢��ʼ�˵��ȡ�������ȣ� - ����ϵͳ�̶��������Ե�
	// 2�����ݵ�С�ʹ��ڵĹ��ˣ����ǣ���Ҫ���ų��б�����һЩС����ȣ�
	// 3�����ش��ڡ���Ч���ڵĹ���
	// 4�������¼��Ĺ���
	// 5��Location�¼�����


	// ����������Ϣ��ȡ���Լ������

	return S_OK;
}