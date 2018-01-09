#include "stdafx.h"
#include "FileClusterTag.h"
#include <time.h>
#include <atlfile.h>

#pragma pack(push, 1)
static const GUID GUID_SAFEBK_ID = { 0x1a90e7c9, 0x4b3a, 0x49a8,{ 0xae, 0xa0, 0xe1, 0xd4, 0x8a, 0x25, 0x66, 0xab } };
#pragma pack(pop)

CFileClusterRejust::CFileClusterRejust(ULONGLONG llScanBegin, ULONGLONG llScanEnd)
	: m_ullScanBegin(llScanBegin)
	, m_ullScanEnd(llScanEnd)
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

	BOOL bFirstBlock = (pBlock->curr == 0);
	CAtlString strFile;
	std::map<GUID, CAtlString>::iterator itor = m_mapFile.find(pBlock->fileid);
	if (itor == m_mapFile.end())
	{
		// �鿴�Ƿ��һ���飬��һ����洢���ļ�·����Ϣ�����û�У���洢һ����fileid+�ļ�����Ϊ·����
		if (bFirstBlock)
		{
			// ֱ�ӻ�ȡԭʼ����
		}
		else
		{
			// ȡfileidΪ����
		}
		strFile = GetFilePath(pBlock);
		m_mapFile.insert(std::pair<GUID, CAtlString>(pBlock->fileid, strFile));
	}
	else
	{
		strFile = itor->second;
	}

	// ���ļ���������д����
	CAtlFile file;
	file.Create(strFile, GENERIC_WRITE, 0, CREATE_ALWAYS);
	file.Seek(0, FILE_END);
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
		char *bfBlock = new char[nBlockSize];	CAutoPtr<char> _auto_free1(bfBlock);
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
	HRESULT hr = E_FAIL;

	do
	{
		CAtlFile fSrc;
		hr = fSrc.Create(wsFileTag, GENERIC_READ, 0, OPEN_EXISTING);
		if (FAILED(hr))
			break;

		ULONGLONG ullen = 0;
		fSrc.GetSize(ullen);

		int nBlockSize = GetBytesPerCluster(wsFileTag);;		// �ȼ���ش�С��ƽ̨��С
		int nBuffSize = nBlockSize - SAFEBK_HDR_SIZE;
		CAtlFile fDst;

		int nCacheMaxSize = 64 * 1024;
		if (nCacheMaxSize < nBlockSize)
			nCacheMaxSize = nBlockSize;
		int nCacheSize = ullen > nCacheMaxSize ? nCacheMaxSize : ullen;		// 64KB��д���������
		char *bfCache = new char[nCacheSize];		CAutoPtr<char> _auto_free1(bfCache);
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

			// ��ȷ���£��ش�С�Ƿ�����ȷ�ģ����ǵĻ�����������������Ҹ����ش�С
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
					// ȡ��һ���������
					bFirst = FALSE;

					// У�����ݿ�
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
						break;

					WCHAR* pFilePath = new WCHAR[pInfo->filenamelen / 2 + 1];		CAutoPtr<WCHAR> _auto_free2(pFilePath);
					memset(pFilePath, 0, pInfo->filenamelen + 2);
					memcpy(pFilePath, pInfo->filename, pInfo->filenamelen);

					if (wsFileSrc && dwBufBytes > pInfo->filenamelen)
					{
						memcpy(wsFileSrc, pInfo->filename, pInfo->filenamelen);
					}

					WCHAR fileNewPath[1024] = { 0 };

					if (wsFileNewDir)
					{
						// �洢����Ŀ¼��
						WCHAR fileName[256] = { 0 };
						wcscpy(fileName, PathFindFileName(pFilePath));
						wsprintf(fileNewPath, L"%s%s", wsFileNewDir, fileName);

						pFilePath = fileNewPath;
					}

					hr = fDst.Create(pFilePath, GENERIC_WRITE, 0, CREATE_ALWAYS);
					if (FAILED(hr))
						break;

					continue;
				}

				PSAFEBK_BLOCK_HDR pBlock = (PSAFEBK_BLOCK_HDR)(bfCache + i * nBlockSize);

				// У�����ݿ�
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

	return S_OK;
}

HRESULT CFileClusterTag::DiskRestore(LPCWSTR wsDevName, ULONGLONG llScanBegin, ULONGLONG llScanEnd)
{
	HRESULT hr;

	CFileClusterRejust	oCluster(llScanBegin, llScanEnd);

	WCHAR wsTmp[100];

	int nBlockSize = 4 * 1024;		// ����Ϊ��ǰ����4k��С�Ĵأ��Ӷ�ȥȡ��ʵ�Ĵش�С����������ǽ���Ļ������Գ��Զ��������
	int nBuffSize = nBlockSize - SAFEBK_HDR_SIZE;
	char *bfBlock = new char[nBlockSize];	CAutoPtr<char> _auto_free1(bfBlock);
	memset(bfBlock, 0, nBlockSize);

	int nCacheBuffSize = 64 * 1024 * 1024;
	char *bfCache = new char[nCacheBuffSize];	CAutoPtr<char> _auto_free2(bfCache);
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

			// ȷʵ���ҵ��Ե��ˣ���ô��һ�´ش�С�Ƿ�����
			if (!bAdjustCluster && (pBlock->clustersize != nBlockSize))
			{
				bAdjustCluster = TRUE;
				f.Seek(ullCurrOffset, FILE_BEGIN);

				// �����ش�С
				nBlockSize = pBlock->clustersize;
				nBuffSize = nBlockSize - SAFEBK_HDR_SIZE;
				bfBlock = new char[nBlockSize];
				memset(bfBlock, 0, nBlockSize);
				_auto_free1.Attach(bfBlock);

				// ����cache��С
				if (nBlockSize > nCacheBuffSize)
				{
					nCacheBuffSize = nBlockSize;
					bfCache = new char[nCacheBuffSize];
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

			// �ҵ����ļ�id����Ҫ���뵽map�У��Ա�����ҵ���һ��fileid���洢����Ӧ�ļ�����ȥ
			oCluster.AddCluster(pBlock, ullCurrOffset);

			StringFromGUID2(pBlock->fileid, wsTmp, _countof(wsTmp));
			wprintf(L"block pos: %p, fid: %s, curr: %lld, total: %lld, size: %d\n", ullCurrOffset, wsTmp, pBlock->curr, pBlock->total, pBlock->size);
		}

		if (!bAdjustCluster)
		{
			ullPos += nCacheBuffSize;
		}
	}

	if (FAILED(hr))
		return hr;
	return hr;
}

