#pragma once
#include <stdint.h>



class CFileClusterTag
{
public:
	CFileClusterTag();
	~CFileClusterTag();

	HRESULT AddTag(__in LPCWSTR wsFileNoTag, __in LPCWSTR wsFileTag, __in BOOL bDelOld = TRUE);
	HRESULT RemoveTag(__in LPCWSTR wsFileTag, __out LPWSTR wsFileNoTag, __in BOOL bDelOld = TRUE);

	// ±©Á¦ËÑË÷
	HRESULT DiskRestore(LPCWSTR wsDevName, ULONGLONG llScanBegin, ULONGLONG llScanEnd);
};

