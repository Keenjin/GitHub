#pragma once
#include <stdint.h>



class CFileClusterTag
{
public:
	CFileClusterTag();
	~CFileClusterTag();

	HRESULT AddTag(LPCWSTR wsFileNoTag, LPCWSTR wsFileTag, BOOL bDelOld = TRUE);
	HRESULT RemoveTag(LPCWSTR wsFileTag, LPCWSTR wsFileNoTag, BOOL bDelOld = TRUE);

	// ±©Á¦ËÑË÷
	HRESULT DiskRestore(LPCWSTR wsDevName, ULONGLONG llScanBegin, ULONGLONG llScanEnd);
};

