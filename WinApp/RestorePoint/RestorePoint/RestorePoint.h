#pragma once
#include <atlcoll.h>
#include <atlstr.h>

typedef struct _BACKUP_INFO
{
	CAtlString strName;
	ULONG ulId;
	CAtlString strTime;
} BACKUP_INFO, *PBACKUP_INFO;

class CRestorePoint
{
public:
	CRestorePoint();
	~CRestorePoint();

	HRESULT BackupEnable(TCHAR szDriver);
	HRESULT BackupCreate(LPCWSTR wsDesc);

	HRESULT BackupGetList(CAtlArray<BACKUP_INFO> &ret);

	HRESULT BackupRestore(UINT nId);

private:
	BOOL InitializeCOMSecurity();

private:
	BOOL	m_bInitCom;
};

