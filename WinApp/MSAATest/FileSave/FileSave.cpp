// FileSave.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "FileClusterTag.h"


int _tmain(int argc, _TCHAR* argv[])
{
	CFileClusterTag fileTag;
	fileTag.AddTag(L"I:\\HCI-����թ�ĵ����ݱ�������-Luxozhang-201703281739.fw.png", L"I:\\HCI-����թ�ĵ����ݱ�������-Luxozhang-201703281739.fw_tag.png", FALSE);
	fileTag.RemoveTag(L"I:\\HCI-����թ�ĵ����ݱ�������-Luxozhang-201703281739.fw_tag.png", NULL, FALSE);
	return 0;
}

