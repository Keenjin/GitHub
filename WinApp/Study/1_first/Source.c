#include <ntddk.h>

void DriverUnload(PDRIVER_OBJECT driver)
{
	UNREFERENCED_PARAMETER(driver);
	DbgPrint("Enter First.sys DriverUnload\r\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
	UNREFERENCED_PARAMETER(reg_path);

#if DBG
	_asm int 3
#endif // DBG
	
	DbgPrint("Enter First.sys DriverEntry\r\n");

	driver->DriverUnload = DriverUnload;

	return STATUS_SUCCESS;
}


//NTSTATUS TestFunc()
//{
//	NTSTATUS status = STATUS_SUCCESS;
//
//	status = ZwCreateFile(...);
//	if (!NT_SUCCESS(status))
//	{
//		return status;
//	}
//
//	...
//
//	return status;
//}

//UNICODE_STRING str = RTL_CONSTANT_STRING(L"My first Kernel String!");
//DbgPrint("%wZ", str);

//NTSTATUS MyDispatch(PDEVICE_OBJECT device, PIRP irp);