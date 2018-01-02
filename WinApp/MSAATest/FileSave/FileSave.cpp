// FileSave.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "FileClusterTag.h"


int _tmain(int argc, _TCHAR* argv[])
{
	CFileClusterTag fileTag;
	fileTag.AddTag(L"I:\\HCI-防敲诈文档备份保护方案-Luxozhang-201703281739.fw.png", L"I:\\HCI-防敲诈文档备份保护方案-Luxozhang-201703281739.fw_tag.png", FALSE);
	fileTag.RemoveTag(L"I:\\HCI-防敲诈文档备份保护方案-Luxozhang-201703281739.fw_tag.png", NULL, FALSE);
	return 0;
}

