#include "stdafx.h"
#include "FileClusterTag.h"
#include <time.h>
#include <atlfile.h>
#include <setupapi.h>
#include <iostream>

#pragma comment(lib, "Setupapi.lib")

#pragma pack(push, 1)
static const GUID GUID_SAFEBK_ID = { 0x1a90e7c9, 0x4b3a, 0x49a8,{ 0xae, 0xa0, 0xe1, 0xd4, 0x8a, 0x25, 0x66, 0xab } };
#pragma pack(pop)

CFileClusterRejust::CFileClusterRejust(LPCWSTR wsDevName, ULONGLONG llScanBegin, ULONGLONG llScanEnd)
	: m_strDevName(wsDevName)
	, m_ullScanBegin(llScanBegin)
	, m_ullScanEnd(llScanEnd)
	, m_uClusterSize(0)
{

}

CFileClusterRejust::~CFileClusterRejust()
{

}

void CFileClusterRejust::AddCluster(PSAFEBK_BLOCK_HDR pBlock, ULONGLONG ullCurrOffset)
{
	if (pBlock == NULL)
	{
		return;
	}

	WCHAR wsTmp[100] = {0};
	StringFromGUID2(pBlock->fileid, wsTmp, _countof(wsTmp));

	m_uClusterSize = pBlock->clustersize;

	std::map<GUID, std::list<std::pair<uint64_t, ULONGLONG>>>::iterator itor = m_mapFileCluster.find(pBlock->fileid);
	if (itor == m_mapFileCluster.end())
	{
		std::list<std::pair<uint64_t, ULONGLONG>> listFileCluster;
		listFileCluster.push_back(std::pair<uint64_t, ULONGLONG>(pBlock->curr, ullCurrOffset));
		m_mapFileCluster.insert(std::pair<GUID, std::list<std::pair<uint64_t, ULONGLONG>>>(pBlock->fileid, listFileCluster));
	}
	else
	{
		itor->second.push_back(std::pair<uint64_t, ULONGLONG>(pBlock->curr, ullCurrOffset));
	}

	CAtlString strFileType = CA2W(pBlock->filetype, CP_UTF8);
	CAtlString strFilePath = wsTmp;
	strFilePath += strFileType;

	std::map<GUID, FILEINFO>::iterator itor1 = m_mapFileInfo.find(pBlock->fileid);
	if (itor1 == m_mapFileInfo.end())
	{
		FILEINFO fileinfo;
		fileinfo.uClusterTotalCnt = pBlock->total;
		fileinfo.uCurClusterCnt = 1;
		fileinfo.strFileExt = strFileType;

		if (pBlock->curr == 0)
		{
			fileinfo.bGetFirstBlock = TRUE;
			// 第一个块，尝试去找文件名
			PSAFEBK_FIRST_BLOCK pInfo = (PSAFEBK_FIRST_BLOCK)pBlock->buff;
			if (pInfo)
			{
				fileinfo.uBkTime = pInfo->bktime;
				fileinfo.uFileSize = pInfo->filesize;

				CAtlString strFilePathTmp = pInfo->filename;
				if (!strFileType.IsEmpty() &&
					!strFilePathTmp.IsEmpty() &&
					strFilePathTmp.Right(strFileType.GetLength()).CompareNoCase(strFileType) == 0)
				{
					strFilePathTmp += strFileType;
					strFilePath = strFilePathTmp;
				}
			}
		}

		fileinfo.strFilePath = strFilePath;
		fileinfo.strFileName = strFilePath;
		PathStripPath(fileinfo.strFileName.GetBuffer());
		fileinfo.strFileName.ReleaseBuffer();

		m_mapFileInfo.insert(std::pair<GUID, FILEINFO>(pBlock->fileid, fileinfo));
	}
	else if (!itor1->second.bGetFirstBlock)
	{
		itor1->second.uClusterTotalCnt = pBlock->total;
		itor1->second.uCurClusterCnt++;
		itor1->second.strFileExt = strFileType;
		if (pBlock->curr == 0)
		{
			itor1->second.bGetFirstBlock = TRUE;
			// 第一个块，尝试去找文件名
			PSAFEBK_FIRST_BLOCK pInfo = (PSAFEBK_FIRST_BLOCK)pBlock->buff;
			if (pInfo)
			{
				itor1->second.uBkTime = pInfo->bktime;
				itor1->second.uFileSize = pInfo->filesize;

				CAtlString strFilePathTmp = pInfo->filename;
				if (!strFileType.IsEmpty() &&
					!strFilePathTmp.IsEmpty() &&
					strFilePathTmp.Right(strFileType.GetLength()).CompareNoCase(strFileType) != 0)
				{
					strFilePathTmp += strFileType;
					strFilePath = strFilePathTmp;
					itor1->second.strFilePath = strFilePath;

					itor1->second.strFileName = strFilePath;
					PathStripPath(itor1->second.strFileName.GetBuffer());
					itor1->second.strFileName.ReleaseBuffer();
				}
			}
		}
	}
}

// comparison, not case sensitive.
bool SortCluster(const std::pair<uint64_t, ULONGLONG>& first, const std::pair<uint64_t, ULONGLONG>& second)
{
	return first.first < second.first;
}

// 通过文件簇，生成文件
BOOL CFileClusterRejust::BuildFiles(LPCWSTR wsNewDir)
{
	if (wsNewDir == NULL)
	{
		return FALSE;
	}

	if (!ATLPath::FileExists(wsNewDir))
	{
		CreateDirectory(wsNewDir, NULL);
	}

	// 首先针对已经获取到的文件簇，根据索引进行排序
	// 然后针对文件簇，进行相邻合并（后期优化考虑）
	// 最后依次打开文件簇，进行写入
	CAtlFile f;
	HRESULT hr = f.Create(m_strDevName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING | FILE_FLAG_RANDOM_ACCESS);
	if (FAILED(hr))
		return FALSE;

	for (std::map<GUID, std::list<std::pair<uint64_t, ULONGLONG>>>::iterator itor = m_mapFileCluster.begin();
		itor != m_mapFileCluster.end(); itor++)
	{
		itor->second.sort(SortCluster);

		// 读取文件簇
		CAtlString strFilePath = wsNewDir;
		if (strFilePath.Right(1) != L'\\' && strFilePath.Right(1) != L'/')
		{
			strFilePath += L"\\";
		}
		strFilePath += m_mapFileInfo[itor->first].strFileName;
		CAtlFile file;
		file.Create(strFilePath, GENERIC_WRITE, 0, CREATE_ALWAYS);

		int nCacheSize = m_uClusterSize;
		char *bfCache = new char[nCacheSize];		CAutoPtr<char> _auto_free1(bfCache);
		memset(bfCache, 0, nCacheSize);

		for (std::list<std::pair<uint64_t, ULONGLONG>>::iterator itor1 = itor->second.begin();
			itor1 != itor->second.end(); itor1++)
		{
			f.Seek(itor1->second, FILE_BEGIN);
			
			DWORD dwBytesRead = 0;
			hr = f.Read(bfCache, nCacheSize, dwBytesRead);
			if (FAILED(hr) || dwBytesRead == 0)
				break;

			PSAFEBK_BLOCK_HDR pBlock = (PSAFEBK_BLOCK_HDR)(bfCache);
			if (pBlock && pBlock->curr != 0)
			{
				file.Write(pBlock->buff, pBlock->size);
			}
		}
	}

	return TRUE;
}

void CFileClusterRejust::GetFileList(std::list<FILEINFO>& listFiles)
{
	for (std::map<GUID, FILEINFO>::iterator itor = m_mapFileInfo.begin();
		itor != m_mapFileInfo.end(); itor++)
	{
		listFiles.push_back(itor->second);
	}
}

uint64_t CFileClusterTag::CalcBlockCRC(PSAFEBK_BLOCK_HDR pBlock, int nBlock)
{
	uint64_t tmp = pBlock->crc;

	uint64_t crc = 0;
	pBlock->crc = 0;
	for (int i = 0; i < nBlock; i++)
		crc += (crc << 8) + ((uint8_t*)pBlock)[i];

	pBlock->crc = tmp;
	return crc;
}

uint32_t CFileClusterTag::GetBytesPerCluster(LPCWSTR wsFilePath)
{
	DWORD dwSectorPerCluster, dwBytesPerSector, dwNumOfFreeCluster, dwTotalCluster;
	WCHAR szRoot[4] = { L"c:\\" };
	szRoot[0] = *wsFilePath;
	GetDiskFreeSpace(szRoot, &dwSectorPerCluster, &dwBytesPerSector, &dwNumOfFreeCluster, &dwTotalCluster);

	uint32_t clustersize = (uint32_t)dwSectorPerCluster * (uint32_t)dwBytesPerSector;
	return clustersize;
}

void CFileClusterTag::GetFileExtension(LPCWSTR wsFileNoTag, char* wsFileType, int nFileTypeBufferSize)
{
	CAtlString strFileNoTag = wsFileNoTag;
	int nPos = strFileNoTag.ReverseFind(L'.');
	if (nPos != -1)
	{
		CAtlStringA strFileType = CW2A(strFileNoTag.Mid(nPos), CP_UTF8);
		if (!strFileType.IsEmpty())
		{
			memcpy(wsFileType, strFileType, strFileType.GetLength() > nFileTypeBufferSize ? nFileTypeBufferSize : strFileType.GetLength());
		}
	}
}


CFileClusterTag::CFileClusterTag()
{
}


CFileClusterTag::~CFileClusterTag()
{
}

HRESULT CFileClusterTag::AddTag(__in LPCWSTR wsFileNoTag, __in LPCWSTR wsFileTag, __in BOOL bDelOld/* = TRUE*/)
{
	HRESULT hr = E_FAIL;

	do
	{
		int nBlockSize = GetBytesPerCluster(wsFileNoTag);
		int nBuffSize = nBlockSize - SAFEBK_HDR_SIZE;
		char *bfBlock = new char[nBlockSize];	CAutoVectorPtr<char> _auto_free1(bfBlock);
		memset(bfBlock, 0, nBlockSize);

		CAtlFile fSrc;
		hr = fSrc.Create(wsFileNoTag, GENERIC_READ, 0, OPEN_EXISTING);
		if (FAILED(hr))
			break;

		CAtlFile fDst;
		hr = fDst.Create(wsFileTag, GENERIC_WRITE, 0, CREATE_ALWAYS);
		if (FAILED(hr))
			break;

		ULONGLONG ullLen;
		hr = fSrc.GetSize(ullLen);
		if (FAILED(hr))
			break;

		PSAFEBK_BLOCK_HDR pBlock = (PSAFEBK_BLOCK_HDR)bfBlock;
		pBlock->version = SAFEBK_VERSION_001;
		hr = CoCreateGuid(&pBlock->fileid);
		if (FAILED(hr))
			break;
		memcpy(&pBlock->_head, &GUID_SAFEBK_ID, sizeof(GUID_SAFEBK_ID));
		memcpy(&pBlock->_tail, &GUID_SAFEBK_ID, sizeof(GUID_SAFEBK_ID));
		pBlock->total = 1 + (ullLen / nBuffSize) + (ullLen % nBuffSize ? 1 : 0);
		pBlock->curr = 0;
		pBlock->clustersize = nBlockSize;
		GetFileExtension(wsFileNoTag, pBlock->filetype, sizeof(pBlock->filetype));
		

		PSAFEBK_FIRST_BLOCK pInfo = (PSAFEBK_FIRST_BLOCK)pBlock->buff;
		time(&pInfo->bktime);
		pInfo->filesize = (uint64_t)ullLen;
		size_t uNameLen = wcslen(wsFileNoTag);
		pInfo->filenamelen = uNameLen * 2;
		memcpy(pInfo->filename, wsFileNoTag, pInfo->filenamelen);
		pBlock->crc = CalcBlockCRC(pBlock, nBlockSize);

		hr = fDst.Write(pBlock, nBlockSize);
		if (FAILED(hr))
			break;

		while (1)
		{
			pBlock->curr++;
			if (pBlock->curr == pBlock->total - 1)
				memset(pBlock->buff, 0, nBuffSize);

			DWORD dwReads;
			hr = fSrc.Read(pBlock->buff, nBuffSize, dwReads);
			if (FAILED(hr) || dwReads == 0)
				break;
			pBlock->size = (uint16_t)dwReads;
			pBlock->crc = CalcBlockCRC(pBlock, nBlockSize);

			hr = fDst.Write(pBlock, nBlockSize);
			if (FAILED(hr))
				break;
		}

		if (FAILED(hr))
			break;

	} while (FALSE);

	if (bDelOld)
	{
		::DeleteFile(wsFileNoTag);
	}

	if (FAILED(hr))
		::DeleteFile(wsFileTag);

	return hr;
}

HRESULT CFileClusterTag::RemoveTag(__in LPCWSTR wsFileTag, __in LPCWSTR wsFileNewDir/* = NULL*/, __inout LPWSTR wsFileSrc/* = NULL*/, __in DWORD dwBufBytes/* = 0*/, __in BOOL bDelOld/* = TRUE*/)
{
	HRESULT hrRet = E_FAIL;

	LOG_PRINT(L"%s Enter.", __FUNCTIONW__);

	do
	{
		if (wsFileNewDir && !ATLPath::FileExists(wsFileNewDir))
		{
			CreateDirectory(wsFileNewDir, NULL);
		}

		CAtlFile fSrc;
		HRESULT hr = fSrc.Create(wsFileTag, GENERIC_READ, 0, OPEN_EXISTING);
		if (FAILED(hr))
		{
			LOG_PRINT(L"%s, read(%s) failed. err(%d)", __FUNCTIONW__, wsFileTag, GetLastError());
			break;
		}

		ULONGLONG ullen = 0;
		fSrc.GetSize(ullen);

		int nBlockSize = GetBytesPerCluster(wsFileTag);;		// 先假设簇大小是平台大小
		int nBuffSize = nBlockSize - SAFEBK_HDR_SIZE;
		CAtlFile fDst;

		int nCacheMaxSize = 64 * 1024;
		if (nCacheMaxSize < nBlockSize)
			nCacheMaxSize = nBlockSize;
		ULONGLONG nCacheSize = ullen > nCacheMaxSize ? nCacheMaxSize : ullen;		// 64KB读写，性能最佳
		char *bfCache = new char[nCacheSize];		CAutoVectorPtr<char> _auto_free1(bfCache);
		memset(bfCache, 0, nCacheSize);

		BOOL bFirst = TRUE;
		BOOL bCheckClusterSize = FALSE;
		while (1)
		{
			DWORD dwBytesRead = 0;
			hr = fSrc.Read(bfCache, nCacheSize, dwBytesRead);
			if (FAILED(hr) || dwBytesRead == 0)
				break;

			PSAFEBK_BLOCK_HDR pBlock = (PSAFEBK_BLOCK_HDR)(bfCache);

			// 先确认下，簇大小是否是正确的，不是的话，打回重新来，并且更正簇大小
			if (!bCheckClusterSize)
			{
				bCheckClusterSize = TRUE;
				if (pBlock->clustersize != nBlockSize)
				{
					nBlockSize = pBlock->clustersize;
					nBuffSize = nBlockSize - SAFEBK_HDR_SIZE;

					if (nCacheMaxSize < nBlockSize)
					{
						nCacheSize = ullen > nCacheMaxSize ? nCacheMaxSize : ullen;
						bfCache = new char[nCacheSize];
						_auto_free1.Free();
						_auto_free1.Attach(bfCache);
						memset(bfCache, 0, nCacheSize);
					}

					fSrc.Seek(0, FILE_BEGIN);
					continue;
				}
			}

			int nCnt = dwBytesRead / nBlockSize + ((dwBytesRead % nBlockSize) == 0 ? 0 : 1);

			for (size_t i = 0; i < nCnt; i++)
			{
				if (bFirst)
				{
					// 取第一个块的内容
					bFirst = FALSE;

					// 校验数据块
					if (memcmp(&pBlock->_head, &GUID_SAFEBK_ID, sizeof(GUID)) != 0)
						break;
					if (memcmp(&pBlock->_tail, &GUID_SAFEBK_ID, sizeof(GUID)) != 0)
						break;
					if (pBlock->version != 1)
						break;
					if (pBlock->size > nBlockSize)
						break;
					if (pBlock->crc != CalcBlockCRC(pBlock, nBlockSize))
						break;

					PSAFEBK_FIRST_BLOCK pInfo = (PSAFEBK_FIRST_BLOCK)pBlock->buff;
					if (pInfo->filenamelen == 0)
					{
						LOG_PRINT(L"%s, filenamelen is 0", __FUNCTIONW__);
						break;
					}

					WCHAR* pFilePath = new WCHAR[pInfo->filenamelen / 2 + 1];		CAutoVectorPtr<WCHAR> _auto_free2(pFilePath);
					memset(pFilePath, 0, pInfo->filenamelen + 2);
					memcpy(pFilePath, pInfo->filename, pInfo->filenamelen);

					if (wsFileSrc && dwBufBytes > pInfo->filenamelen)
					{
						memcpy(wsFileSrc, pInfo->filename, pInfo->filenamelen);
					}

					WCHAR fileNewPath[1024] = { 0 };

					if (wsFileNewDir)
					{
						// 存储到新目录下
						WCHAR fileName[256] = { 0 };
						wcscpy_s(fileName, PathFindFileName(pFilePath));
						wsprintf(fileNewPath, L"%s%s", wsFileNewDir, fileName);

						pFilePath = fileNewPath;
					}

					hr = fDst.Create(pFilePath, GENERIC_WRITE, 0, CREATE_ALWAYS);
					if (FAILED(hr))
					{
						LOG_PRINT(L"%s, create (%s) failed. err(%d)", __FUNCTIONW__, pFilePath, GetLastError());
						break;
					}

					hrRet = S_OK;

					continue;
				}

				PSAFEBK_BLOCK_HDR pBlock = (PSAFEBK_BLOCK_HDR)(bfCache + i * nBlockSize);

				// 校验数据块
				if (memcmp(&pBlock->_head, &GUID_SAFEBK_ID, sizeof(GUID)) != 0)
					break;
				if (memcmp(&pBlock->_tail, &GUID_SAFEBK_ID, sizeof(GUID)) != 0)
					break;
				if (pBlock->version != 1)
					break;
				if (pBlock->size > nBlockSize)
					break;
				if (pBlock->crc != CalcBlockCRC(pBlock, nBlockSize))
					break;

				fDst.Write(pBlock->buff, pBlock->size);
			}
		}

	} while (FALSE);

	return hrRet;
}

HRESULT CFileClusterTag::DiskRestore(LPCWSTR wsDevName, ULONGLONG llScanBegin, ULONGLONG llScanEnd, LPCWSTR wsNewDir, DWORD* pdwSucCnt, IFileClusterCallback* pCallback)
{
	HRESULT hr;

	CFileClusterRejust	oCluster(wsDevName, llScanBegin, llScanEnd);

	if (pCallback)
	{
		pCallback->OnProgressStart(llScanEnd - llScanBegin);
	}

	WCHAR wsTmp[100];

	int nBlockSize = 4 * 1024;		// 就认为当前就是4k大小的簇，从而去取真实的簇大小（这里如果是界面的话，可以尝试多搜索几项）
	int nBuffSize = nBlockSize - SAFEBK_HDR_SIZE;
	char *bfBlock = new char[nBlockSize];	CAutoVectorPtr<char> _auto_free1(bfBlock);
	memset(bfBlock, 0, nBlockSize);

	int nCacheBuffSize = 64 * 1024 * 1024;
	char *bfCache = new char[nCacheBuffSize];	CAutoVectorPtr<char> _auto_free2(bfCache);
	memset(bfCache, 0, nCacheBuffSize);

	CAtlFile f;
	hr = f.Create(wsDevName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING | FILE_FLAG_RANDOM_ACCESS);
	if (FAILED(hr))
		return hr;

	hr = f.Seek(llScanBegin, FILE_BEGIN);
	if (FAILED(hr))
		return hr;

	for (ULONGLONG ullPos = llScanBegin; ullPos < llScanEnd; )
	{
		DWORD dwReads = 0;
		hr = f.Read(bfCache, nCacheBuffSize, dwReads);
		if (FAILED(hr) || dwReads == 0)
			break;

		BOOL bAdjustCluster = FALSE;
		int nCnt = dwReads / 512;
		for (int j = 0; j < nCnt; j++)
		{
			PSAFEBK_BLOCK_HDR pBlock = (PSAFEBK_BLOCK_HDR)(bfCache + j * 512);
			ULONGLONG ullCurrOffset = ullPos + j * 512;
			int nCacheSize = nCacheBuffSize - j * 512;

			if (memcmp(&pBlock->_head, &GUID_SAFEBK_ID, sizeof(GUID)) != 0)
				continue;
			if (memcmp(&pBlock->_tail, &GUID_SAFEBK_ID, sizeof(GUID)) != 0)
				continue;

			// 确实是找到对的了，那么看一下簇大小是否满足
			if (!bAdjustCluster && (pBlock->clustersize != nBlockSize))
			{
				bAdjustCluster = TRUE;
				f.Seek(ullCurrOffset, FILE_BEGIN);

				// 调整簇大小
				nBlockSize = pBlock->clustersize;
				nBuffSize = nBlockSize - SAFEBK_HDR_SIZE;
				bfBlock = new char[nBlockSize];
				memset(bfBlock, 0, nBlockSize);
				_auto_free1.Attach(bfBlock);

				// 调整cache大小
				if (nBlockSize > nCacheBuffSize)
				{
					nCacheBuffSize = nBlockSize;
					bfCache = new char[nCacheBuffSize];
					_auto_free2.Free();
					_auto_free2.Attach(bfCache);
					memset(bfCache, 0, nCacheBuffSize);
				}
				break;
			}

			if (nCacheSize < nBlockSize)
			{
				hr = f.Seek(ullCurrOffset, FILE_BEGIN);
				if (FAILED(hr))
					break;
				hr = f.Read(bfBlock, nBlockSize, dwReads);
				if (FAILED(hr) || dwReads == 0)
					break;
				pBlock = (PSAFEBK_BLOCK_HDR)bfBlock;
			}

			if (pBlock->version != 1)
				continue;

			if (pBlock->crc != CalcBlockCRC(pBlock, nBlockSize))
				continue;

			// 找到了文件id，需要插入到map中，以便继续找到下一个fileid，存储到对应文件里面去
			oCluster.AddCluster(pBlock, ullCurrOffset);

			j += nBlockSize / 512 - 1;

			//StringFromGUID2(pBlock->fileid, wsTmp, _countof(wsTmp));
			//wprintf(L"block pos: %p, fid: %s, curr: %lld, total: %lld, size: %d\n", ullCurrOffset, wsTmp, pBlock->curr, pBlock->total, pBlock->size);
		}

		if (!bAdjustCluster)
		{
			ullPos += nCacheBuffSize;
		}

		if (pCallback)
		{
			pCallback->OnProgressIncrement(ullPos - llScanBegin);
		}
	}

	oCluster.BuildFiles(wsNewDir);
	std::list<FILEINFO> listFiles;
	oCluster.GetFileList(listFiles);

	if (pCallback)
	{
		if (pdwSucCnt)
		{
			*pdwSucCnt = listFiles.size();
		}
		pCallback->OnProgressEnd(listFiles.size());
	}

	if (FAILED(hr))
		return hr;
	return hr;
}

BOOL CFileClusterTag::GetDeviceNum(const CAtlString& strDevName, STORAGE_DEVICE_NUMBER& devNum)
{
	BOOL bRet = FALSE;

	HANDLE hDevice = NULL;
	do
	{
		hDevice = CreateFile(strDevName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING | FILE_FLAG_RANDOM_ACCESS, NULL);
		if (!hDevice || hDevice == INVALID_HANDLE_VALUE)
		{
			break;
		}

		DWORD dwReaded = 0;
		if (!DeviceIoControl(hDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &devNum, sizeof(devNum), &dwReaded, NULL))
		{
			DWORD dwErr = GetLastError();
			break;
		}

		STORAGE_PROPERTY_QUERY query;
		query.PropertyId = StorageDeviceProperty;
		query.QueryType = PropertyStandardQuery;
		STORAGE_DEVICE_DESCRIPTOR devDesc;
		DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query), &devDesc, sizeof(devDesc), &dwReaded, NULL);

		bRet = TRUE;

	} while (FALSE);

	if (hDevice && hDevice != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hDevice);
		hDevice = NULL;
	}

	return bRet;
}

void CFileClusterTag::EnumDiskDevice(std::vector<DISKINFO>& vecDiskDev)
{
	HDEVINFO hDevInfo = NULL;
	do
	{
		hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_DISK, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
		if (hDevInfo == INVALID_HANDLE_VALUE || hDevInfo == NULL)
		{
			break;
		}

		int nIndex = -1;
		while (true)
		{
			DISKINFO diskInfo;

			nIndex++;

			SP_DEVICE_INTERFACE_DATA devInterfaceData;
			devInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
			if (!SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &GUID_DEVINTERFACE_DISK, nIndex, &devInterfaceData))
			{
				break;
			}

			DWORD dwRequired = 0;
			if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInterfaceData, NULL, 0, &dwRequired, NULL) &&
				GetLastError() != ERROR_INSUFFICIENT_BUFFER &&
				dwRequired == 0)
			{
				break;
			}

			char* cbBuffer = new char[dwRequired];	CAutoVectorPtr<char> auto_free(cbBuffer);
			memset(cbBuffer, 0, dwRequired);
			PSP_DEVICE_INTERFACE_DETAIL_DATA pDevInData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)cbBuffer;
			pDevInData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
			if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInterfaceData, pDevInData, dwRequired, &dwRequired, NULL))
			{
				break;
			}

			diskInfo.strDevName = pDevInData->DevicePath;
			//std::wcout << L"设备名：" << diskInfo.strDevName.GetString() << std::endl;

			STORAGE_DEVICE_NUMBER devNum;
			if (!GetDeviceNum(diskInfo.strDevName, devNum))
			{
				continue;
			}

			SP_DEVINFO_DATA devData;
			devData.cbSize = sizeof(SP_DEVINFO_DATA);
			if (!SetupDiEnumDeviceInfo(hDevInfo, nIndex, &devData))
			{
				int a = GetLastError();
				continue;
			}

			// 根据dwIndex设备句柄请求FRIENDLYNAME访问  
			WCHAR PropertyBuffer[MAX_PATH] = { 0 };
			DWORD dwSize = 0;
			if (SetupDiGetDeviceRegistryProperty(hDevInfo, &devData, SPDRP_FRIENDLYNAME, 0, (PBYTE)PropertyBuffer, MAX_PATH, &dwSize) == FALSE)
			{
				int a = GetLastError();
				continue;
			}
			
			std::wcout << PropertyBuffer << std::endl;
			diskInfo.strFriendName = PropertyBuffer;
			diskInfo.dwPhysicNum = devNum.DeviceNumber;
			diskInfo.ullDiskSize = 0;
			DISK_GEOMETRY dg;
			if (GetDriveGeometry(diskInfo.dwPhysicNum, dg))
			{
				diskInfo.ullDiskSize = dg.Cylinders.QuadPart * (ULONG)dg.TracksPerCylinder *
					(ULONG)dg.SectorsPerTrack * (ULONG)dg.BytesPerSector;
			}
			vecDiskDev.push_back(diskInfo);

			//std::wcout << L"驱动器：PhysicDrive" << devNum.DeviceNumber << std::endl;
		}
	} while (FALSE);

	std::wcout << L"end." << std::endl;
	
	if (hDevInfo != NULL)
	{
		SetupDiDestroyDeviceInfoList(hDevInfo);
	}
}

BOOL CFileClusterTag::GetDriveGeometry(int nDriverNum, DISK_GEOMETRY& dg)
{
	BOOL bRet = FALSE;

	HANDLE hDevice = NULL;
	do
	{
		CAtlString strDevName;
		strDevName.Format(L"\\\\.\\PHYSICALDRIVE%d", nDriverNum);
		hDevice = CreateFile(strDevName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING | FILE_FLAG_RANDOM_ACCESS, NULL);
		if (!hDevice || hDevice == INVALID_HANDLE_VALUE)
		{
			break;
		}

		DWORD dwReaded = 0;
		if (!DeviceIoControl(hDevice, 
			IOCTL_DISK_GET_DRIVE_GEOMETRY, 
			NULL, 0,              // no input buffer
			&dg, sizeof(dg),    // output buffer
			&dwReaded,                // # bytes returned
			(LPOVERLAPPED)NULL))
		{
			DWORD dwErr = GetLastError();
			break;
		}

		bRet = TRUE;

	} while (FALSE);

	if (hDevice && hDevice != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hDevice);
		hDevice = NULL;
	}

	return bRet;
}