#include "stdafx.h"
#include "RestorePoint.h"
#include <atlcom.h>
#include <AclAPI.h>


CRestorePoint::CRestorePoint()
	: m_bInitCom(FALSE)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (SUCCEEDED(hr))
	{
		InitializeCOMSecurity();
		m_bInitCom = TRUE;
	}
}


CRestorePoint::~CRestorePoint()
{
	if (m_bInitCom)
	{
		CoUninitialize();
		m_bInitCom = FALSE;
	}
}

BOOL CRestorePoint::InitializeCOMSecurity()
{
	// Create the security descriptor explicitly as follows because
	// CoInitializeSecurity() will not accept the relative security descriptors  
	// returned by ConvertStringSecurityDescriptorToSecurityDescriptor().

	SECURITY_DESCRIPTOR securityDesc = { 0 };
	EXPLICIT_ACCESS   ea[5] = { 0 };
	ACL        *pAcl = NULL;
	ULONGLONG  rgSidBA[(SECURITY_MAX_SID_SIZE + sizeof(ULONGLONG) - 1) / sizeof(ULONGLONG)] = { 0 };
	ULONGLONG  rgSidLS[(SECURITY_MAX_SID_SIZE + sizeof(ULONGLONG) - 1) / sizeof(ULONGLONG)] = { 0 };
	ULONGLONG  rgSidNS[(SECURITY_MAX_SID_SIZE + sizeof(ULONGLONG) - 1) / sizeof(ULONGLONG)] = { 0 };
	ULONGLONG  rgSidPS[(SECURITY_MAX_SID_SIZE + sizeof(ULONGLONG) - 1) / sizeof(ULONGLONG)] = { 0 };
	ULONGLONG  rgSidSY[(SECURITY_MAX_SID_SIZE + sizeof(ULONGLONG) - 1) / sizeof(ULONGLONG)] = { 0 };
	DWORD      cbSid = 0;
	BOOL       fRet = FALSE;
	DWORD      dwRet = ERROR_SUCCESS;
	HRESULT    hrRet = S_OK;

	//
	// This creates a security descriptor that is equivalent to the following 
	// security descriptor definition language (SDDL) string:
	//
	//   O:BAG:BAD:(A;;0x1;;;LS)(A;;0x1;;;NS)(A;;0x1;;;PS)(A;;0x1;;;SY)(A;;0x1;;;BA)
	//

	// Initialize the security descriptor.
	fRet = ::InitializeSecurityDescriptor(&securityDesc, SECURITY_DESCRIPTOR_REVISION);
	if (!fRet)
	{
		goto exit;
	}

	// Create an administrator group security identifier (SID).
	cbSid = sizeof(rgSidBA);
	fRet = ::CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, rgSidBA, &cbSid);
	if (!fRet)
	{
		goto exit;
	}

	// Create a local service security identifier (SID).
	cbSid = sizeof(rgSidLS);
	fRet = ::CreateWellKnownSid(WinLocalServiceSid, NULL, rgSidLS, &cbSid);
	if (!fRet)
	{
		goto exit;
	}

	// Create a network service security identifier (SID).
	cbSid = sizeof(rgSidNS);
	fRet = ::CreateWellKnownSid(WinNetworkServiceSid, NULL, rgSidNS, &cbSid);
	if (!fRet)
	{
		goto exit;
	}

	// Create a personal account security identifier (SID).
	cbSid = sizeof(rgSidPS);
	fRet = ::CreateWellKnownSid(WinSelfSid, NULL, rgSidPS, &cbSid);
	if (!fRet)
	{
		goto exit;
	}

	// Create a local service security identifier (SID).
	cbSid = sizeof(rgSidSY);
	fRet = ::CreateWellKnownSid(WinLocalSystemSid, NULL, rgSidSY, &cbSid);
	if (!fRet)
	{
		goto exit;
	}

	// Setup the access control entries (ACE) for COM. You may need to modify 
	// the access permissions for your application. COM_RIGHTS_EXECUTE and
	// COM_RIGHTS_EXECUTE_LOCAL are the minimum access rights required.

	ea[0].grfAccessPermissions = COM_RIGHTS_EXECUTE | COM_RIGHTS_EXECUTE_LOCAL;
	ea[0].grfAccessMode = SET_ACCESS;
	ea[0].grfInheritance = NO_INHERITANCE;
	ea[0].Trustee.pMultipleTrustee = NULL;
	ea[0].Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[0].Trustee.ptstrName = (LPTSTR)rgSidBA;

	ea[1].grfAccessPermissions = COM_RIGHTS_EXECUTE | COM_RIGHTS_EXECUTE_LOCAL;
	ea[1].grfAccessMode = SET_ACCESS;
	ea[1].grfInheritance = NO_INHERITANCE;
	ea[1].Trustee.pMultipleTrustee = NULL;
	ea[1].Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
	ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[1].Trustee.ptstrName = (LPTSTR)rgSidLS;

	ea[2].grfAccessPermissions = COM_RIGHTS_EXECUTE | COM_RIGHTS_EXECUTE_LOCAL;
	ea[2].grfAccessMode = SET_ACCESS;
	ea[2].grfInheritance = NO_INHERITANCE;
	ea[2].Trustee.pMultipleTrustee = NULL;
	ea[2].Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
	ea[2].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[2].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[2].Trustee.ptstrName = (LPTSTR)rgSidNS;

	ea[3].grfAccessPermissions = COM_RIGHTS_EXECUTE | COM_RIGHTS_EXECUTE_LOCAL;
	ea[3].grfAccessMode = SET_ACCESS;
	ea[3].grfInheritance = NO_INHERITANCE;
	ea[3].Trustee.pMultipleTrustee = NULL;
	ea[3].Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
	ea[3].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[3].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[3].Trustee.ptstrName = (LPTSTR)rgSidPS;

	ea[4].grfAccessPermissions = COM_RIGHTS_EXECUTE | COM_RIGHTS_EXECUTE_LOCAL;
	ea[4].grfAccessMode = SET_ACCESS;
	ea[4].grfInheritance = NO_INHERITANCE;
	ea[4].Trustee.pMultipleTrustee = NULL;
	ea[4].Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
	ea[4].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[4].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[4].Trustee.ptstrName = (LPTSTR)rgSidSY;

	// Create an access control list (ACL) using this ACE list.
	dwRet = ::SetEntriesInAcl(ARRAYSIZE(ea), ea, NULL, &pAcl);
	if (dwRet != ERROR_SUCCESS || pAcl == NULL)
	{
		fRet = FALSE;
		goto exit;
	}

	// Set the security descriptor owner to Administrators.
	fRet = ::SetSecurityDescriptorOwner(&securityDesc, rgSidBA, FALSE);
	if (!fRet)
	{
		goto exit;
	}

	// Set the security descriptor group to Administrators.
	fRet = ::SetSecurityDescriptorGroup(&securityDesc, rgSidBA, FALSE);
	if (!fRet)
	{
		goto exit;
	}

	// Set the discretionary access control list (DACL) to the ACL.
	fRet = ::SetSecurityDescriptorDacl(&securityDesc, TRUE, pAcl, FALSE);
	if (!fRet)
	{
		goto exit;
	}

	// Initialize COM. You may need to modify the parameters of
	// CoInitializeSecurity() for your application. Note that an
	// explicit security descriptor is being passed down.

	hrRet = ::CoInitializeSecurity(&securityDesc,
		-1,
		NULL,
		NULL,
		RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
		RPC_C_IMP_LEVEL_IDENTIFY,
		NULL,
		EOAC_DISABLE_AAA | EOAC_NO_CUSTOM_MARSHAL,
		NULL);
	if (FAILED(hrRet))
	{
		fRet = FALSE;
		goto exit;
	}

	fRet = TRUE;

exit:

	::LocalFree(pAcl);

	return fRet;
}

HRESULT CRestorePoint::BackupEnable(TCHAR szDriver)
{
	HRESULT hr;

	do
	{
		WCHAR wsPath[4] = TEXT("C:\\");
		wsPath[0] = szDriver;
		CComPtr<IDispatch> pRestoreCaller;
		hr = CoGetObject(L"winmgmts:{impersonationLevel=impersonate}!root/default:SystemRestore", NULL, IID_IDispatch, (void**)&pRestoreCaller);
		if (FAILED(hr))
			break;

		hr = pRestoreCaller.Invoke1(L"Enable", &CComVariant(wsPath));
		if (FAILED(hr))
			break;

	} while (FALSE);

	return hr;
}

HRESULT CRestorePoint::BackupGetList(CAtlArray<BACKUP_INFO> &ret)
{
	HRESULT hr;
	CComVariant vtRet;

	do
	{
		CComPtr<IDispatch> pWMI;
		hr = CoGetObject(L"winmgmts:root/default", NULL, IID_IDispatch, (void**)&pWMI);
		if (FAILED(hr))
			break;

		CComQIPtr<IDispatch> pSystemRestore;
		hr = pWMI.Invoke1(L"InstancesOf", &CComVariant(L"SystemRestore"), &vtRet);
		if (FAILED(hr))
			break;
		hr = vtRet.ChangeType(VT_UNKNOWN);
		if (FAILED(hr))
			break;
		pSystemRestore = vtRet.pdispVal;
		vtRet.Clear();

		CComQIPtr<IEnumVARIANT> pEnumPoints;
		hr = pSystemRestore.GetPropertyByName(L"_NewEnum", &vtRet);
		if (FAILED(hr))
			break;
		hr = vtRet.ChangeType(VT_UNKNOWN);
		if (FAILED(hr))
			break;
		pEnumPoints = vtRet.pdispVal;
		vtRet.Clear();

		while (1)
		{
			ULONG ulFetched = 0;
			CComQIPtr<IDispatch> pRestoreInfo;

			hr = pEnumPoints->Next(1, &vtRet, &ulFetched);
			if (FAILED(hr) || ulFetched == 0)
				break;
			hr = vtRet.ChangeType(VT_UNKNOWN);
			if (FAILED(hr))
				break;
			pRestoreInfo = vtRet.punkVal;
			vtRet.Clear();

			CComVariant vtDescription;
			hr = pRestoreInfo.GetPropertyByName(L"Description", &vtDescription);
			if (FAILED(hr))
				break;
			hr = vtDescription.ChangeType(VT_BSTR);
			if (FAILED(hr))
				break;

			CComVariant vtSequenceNumber;
			hr = pRestoreInfo.GetPropertyByName(L"SequenceNumber", &vtSequenceNumber);
			if (FAILED(hr))
				break;
			hr = vtSequenceNumber.ChangeType(VT_UI4);
			if (FAILED(hr))
				break;

			CComVariant vtCreationTime;
			hr = pRestoreInfo.GetPropertyByName(L"CreationTime", &vtCreationTime);
			if (FAILED(hr))
				break;
			hr = vtCreationTime.ChangeType(VT_BSTR);
			if (FAILED(hr))
				break;

			BACKUP_INFO info;
			info.strName = vtDescription.bstrVal;
			info.ulId = vtSequenceNumber.ulVal;
			info.strTime = vtCreationTime.bstrVal;
			ret.Add(info);
		}

	} while (FALSE);

	return hr;
}

HRESULT CRestorePoint::BackupCreate(LPCWSTR wsDesc)
{
	HRESULT hr;
	CComVariant vtRet;

	BackupEnable(L'C');

	do
	{
		CComPtr<IDispatch> pRestoreCaller;
		hr = CoGetObject(L"winmgmts:{impersonationLevel=impersonate}!root/default:SystemRestore", NULL, IID_IDispatch, (void**)&pRestoreCaller);
		if (FAILED(hr))
			break;

		CComVariant arrArgs[3] = { CComVariant(100), CComVariant(0), CComVariant(wsDesc) };
		hr = pRestoreCaller.InvokeN(L"CreateRestorePoint", arrArgs, _countof(arrArgs), &vtRet);
		if (FAILED(hr))
			break;
		hr = vtRet.ChangeType(VT_I4);
		if (FAILED(hr))
			break;

		hr = (vtRet.iVal == 0) ? S_OK : HRESULT_FROM_WIN32(ERROR_INTERNAL_ERROR);

	} while (FALSE);

	return hr;
}

HRESULT CRestorePoint::BackupRestore(UINT nId)
{
	HRESULT hr;
	CComVariant vtRet;

	do
	{
		CComPtr<IDispatch> pRestoreCaller;
		hr = CoGetObject(L"winmgmts:{impersonationLevel=impersonate}!root/default:SystemRestore", NULL, IID_IDispatch, (void**)&pRestoreCaller);
		if (FAILED(hr))
			break;

		hr = pRestoreCaller.Invoke1(L"Restore", &CComVariant(nId), &vtRet);
		if (FAILED(hr))
			break;
		hr = vtRet.ChangeType(VT_I4);
		if (FAILED(hr))
			break;

		hr = (vtRet.iVal == 0) ? S_OK : HRESULT_FROM_WIN32(ERROR_INTERNAL_ERROR);

	} while (FALSE);

	return hr;
}

