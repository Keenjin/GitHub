#pragma once
#include <stdint.h>
#include <map>
#include <list>
#include <algorithm>
#include <atlpath.h>
#include <vector>

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

typedef struct _FILEINFO
{
	CAtlString strFilePath;
	CAtlString strFileName;
	CAtlString strFileExt;
	BOOL bGetFirstBlock;
	uint64_t uClusterTotalCnt;
	uint64_t uCurClusterCnt;
	uint64_t uFileSize;
	time_t uBkTime;

	_FILEINFO()
		: bGetFirstBlock(FALSE)
		, uClusterTotalCnt(0)
		, uCurClusterCnt(0)
		, uFileSize(0)
		, uBkTime(0)
	{}
} FILEINFO, *PFILEINFO;

class CFileClusterRejust
{
	class GUIDComparator
	{
	public:
		bool operator()(const GUID& key1, const GUID& key2) const
		{
			if (key1.Data1 < key2.Data1)
			{
				return true;
			}
			else if (key1.Data1 > key2.Data1)
			{
				return false;
			}
			else if (key1.Data2 < key2.Data2)
			{
				return true;
			}
			else if (key1.Data2 > key2.Data2)
			{
				return false;
			}
			else if (key1.Data3 < key2.Data3)
			{
				return true;
			}
			else if (key1.Data3 > key2.Data3)
			{
				return false;
			}
			else
			{
				ULONGLONG ullKey1_4 = (key1.Data4[0] << 3) + (key1.Data4[1] << 2) + (key1.Data4[2] << 1) + (key1.Data4[3]);
				ULONGLONG ullKey2_4 = (key2.Data4[0] << 3) + (key2.Data4[1] << 2) + (key2.Data4[2] << 1) + (key2.Data4[3]);
				return ullKey1_4 < ullKey2_4;
			}
		}
	};

public:
	CFileClusterRejust(LPCWSTR wsDevName, ULONGLONG llScanBegin, ULONGLONG llScanEnd);
	~CFileClusterRejust();

	void AddCluster(PSAFEBK_BLOCK_HDR pBlock, ULONGLONG ullCurrOffset);
	BOOL BuildFiles(LPCWSTR wsNewDir);
	void GetFileList(std::list<FILEINFO>& listFiles);

private:
	std::map<GUID, std::list<std::pair<uint64_t, ULONGLONG>>, GUIDComparator>	m_mapFileCluster;
	std::map<GUID, FILEINFO, GUIDComparator>	m_mapFileInfo;
	ULONGLONG	m_ullScanBegin;
	ULONGLONG	m_ullScanEnd;
	CAtlString	m_strDevName;
	uint32_t	m_uClusterSize;
};

class CFileClusterTag
{
public:
	CFileClusterTag();
	~CFileClusterTag();

	HRESULT AddTag(__in LPCWSTR wsFileNoTag, __in LPCWSTR wsFileTag, __in BOOL bDelOld = TRUE);
	HRESULT RemoveTag(__in LPCWSTR wsFileTag, __in LPCWSTR wsFileNewDir = NULL, __inout LPWSTR wsFileSrc = NULL, __in DWORD dwBufBytes = 0, __in BOOL bDelOld = TRUE);

	// 暴力搜索
	HRESULT DiskRestore(LPCWSTR wsDevName, ULONGLONG llScanBegin, ULONGLONG llScanEnd, LPCWSTR wsNewDir);

	// 磁盘信息获取
	void EnumDiskDevice(std::vector<CAtlString>& vecDiskDev);
	void GetFileExtension(LPCWSTR wsFileNoTag, char* wsFileType, int nFileTypeBufferSize);
	uint64_t CalcBlockCRC(PSAFEBK_BLOCK_HDR pBlock, int nBlock);
	uint32_t GetBytesPerCluster(LPCWSTR wsFilePath);
};

