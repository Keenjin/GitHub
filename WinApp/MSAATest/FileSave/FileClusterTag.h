#pragma once
#include <stdint.h>
#include <map>

#pragma pack(push, 1)
typedef struct _SAFEBK_BLOCK_HDR
{
	GUID _head;
	uint16_t version;
	uint32_t clustersize;
	GUID fileid;
	uint64_t curr;
	uint64_t total;
	uint16_t size;
	uint64_t crc;
	char filetype[16];
	GUID _tail;

	uint8_t buff[1];

} SAFEBK_BLOCK_HDR, *PSAFEBK_BLOCK_HDR;
#define SAFEBK_HDR_SIZE (SIZE_T)(sizeof(SAFEBK_BLOCK_HDR) - sizeof(((PSAFEBK_BLOCK_HDR)0)->buff))

#define SAFEBK_VERSION_001	1


typedef struct _SAFEBK_FIRST_BLOCK
{
	time_t bktime;
	uint64_t filesize;
	uint16_t filenamelen;
	WCHAR filename[1];
} SAFEBK_FIRST_BLOCK, *PSAFEBK_FIRST_BLOCK;
#pragma pack(pop)

class CFileClusterRejust
{
public:
	CFileClusterRejust(ULONGLONG llScanBegin, ULONGLONG llScanEnd);
	~CFileClusterRejust();

	void AddCluster(PSAFEBK_BLOCK_HDR pBlock, ULONGLONG ullCurrOffset);

private:
	std::map<GUID, CAtlString>	m_mapFile;
	ULONGLONG	m_ullScanBegin;
	ULONGLONG	m_ullScanEnd;
};

class CFileClusterTag
{
public:
	CFileClusterTag();
	~CFileClusterTag();

	HRESULT AddTag(__in LPCWSTR wsFileNoTag, __in LPCWSTR wsFileTag, __in BOOL bDelOld = TRUE);
	HRESULT RemoveTag(__in LPCWSTR wsFileTag, __in LPCWSTR wsFileNewDir = NULL, __inout LPWSTR wsFileSrc = NULL, __in DWORD dwBufBytes = 0, __in BOOL bDelOld = TRUE);

	// 暴力搜索
	HRESULT DiskRestore(LPCWSTR wsDevName, ULONGLONG llScanBegin, ULONGLONG llScanEnd);

private:
	// 磁盘信息获取
	void GetFileExtension(LPCWSTR wsFileNoTag, char* wsFileType, int nFileTypeBufferSize);
	uint64_t CalcBlockCRC(PSAFEBK_BLOCK_HDR pBlock, int nBlock);
	uint32_t GetBytesPerCluster(LPCWSTR wsFilePath);
};

