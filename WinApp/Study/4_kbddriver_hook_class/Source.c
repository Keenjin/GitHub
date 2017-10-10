#include <ntddk.h>
#include <ntstrsafe.h>
#include <ntddkbd.h>

#pragma warning(disable: 4054)

extern POBJECT_TYPE*		IoDriverObjectType;

#define KBD_DRIVER_NAME		L"\\Driver\\Kbdclass"
#define DELAY_ONE_MICROSECOND (-10)
#define DELAY_ONE_MILLISECOND (DELAY_ONE_MICROSECOND * 1000)
#define DELAY_ONE_SECOND (DELAY_ONE_MILLISECOND * 1000)

ULONG	gc2pKeyCount;

PDRIVER_DISPATCH gOldDispatchFunc;

// 声明非公开函数
NTSTATUS ObReferenceObjectByName(PUNICODE_STRING ObjectName,
	ULONG Attributes,
	PACCESS_STATE AccessState,
	ACCESS_MASK DesiredAccess,
	POBJECT_TYPE ObjectType,
	KPROCESSOR_MODE AccessMode,
	PVOID ParseContext,
	PVOID *Object);

PDRIVER_OBJECT OpenKbdClass()
{
	// 找到kbdclass类驱动对象地址
	UNICODE_STRING uniNtNameString;

	PDRIVER_OBJECT kbdDriverObject = NULL;

	// 初始化kdbClass驱动的名字为一个字符串
	// 打开驱动对象

	RtlInitUnicodeString(&uniNtNameString, KBD_DRIVER_NAME);

	NTSTATUS status = ObReferenceObjectByName(
		&uniNtNameString,
		OBJ_CASE_INSENSITIVE,
		NULL,
		0,
		*IoDriverObjectType,
		KernelMode,
		NULL,
		&kbdDriverObject
	);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("ObReferenceObjectByName error. %d", status);
		return NULL;
	}
	else
	{
		// 会导致驱动对象引用计数+1，需要解引用
		ObReferenceObject(kbdDriverObject);
	}

	return kbdDriverObject;
}

VOID c2pUnload(PDRIVER_OBJECT driver)
{
	UNREFERENCED_PARAMETER(driver);

	DbgPrint("Enter c2pUnload.\n");

	// 当前线程设置为低实模式，以便让它的运行尽量少影响其他程序
	PRKTHREAD CurrentThread = KeGetCurrentThread();
	KeSetPriorityThread(CurrentThread, LOW_REALTIME_PRIORITY);

	// 还原分发函数
	PDRIVER_OBJECT pDrv = OpenKbdClass();
	if (pDrv != NULL)
	{
		pDrv->MajorFunction[IRP_MJ_READ] = gOldDispatchFunc;
	}

	ASSERT(NULL == driver->DeviceObject);
	LARGE_INTEGER lDelay = RtlConvertLongToLargeInteger(100 * DELAY_ONE_MILLISECOND);
	while (gc2pKeyCount)
	{
		KeDelayExecutionThread(KernelMode, FALSE, &lDelay);
	}

	DbgPrint("Leave c2pUnload.\n");
}

NTSTATUS c2pReadComplete(PDEVICE_OBJECT device, PIRP irp, PVOID context)
{
	UNREFERENCED_PARAMETER(device);
	UNREFERENCED_PARAMETER(context);

	DbgPrint("Enter c2pReadComplete.\n");

	//PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(irp);

	if (NT_SUCCESS(irp->IoStatus.Status))
	{
		PKEYBOARD_INPUT_DATA keyData = (PKEYBOARD_INPUT_DATA)irp->AssociatedIrp.SystemBuffer;
		ULONG numKeys = irp->IoStatus.Information / sizeof(KEYBOARD_INPUT_DATA);

		for (size_t i = 0; i < numKeys; i++)
		{
			DbgPrint("hook kbdclass: %2x, keyaction: %s",
				keyData[i].MakeCode, keyData[i].Flags ? "up" : "down");

		}
	}

	gc2pKeyCount--;

	if (irp->PendingReturned)
	{
		IoMarkIrpPending(irp);
	}

	return irp->IoStatus.Status;
}

NTSTATUS c2pDispatchRead(PDEVICE_OBJECT device, PIRP irp)
{
	DbgPrint("Enter c2pDispatchRead.\n");

	if (irp->CurrentLocation == 1)
	{
		NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
		irp->IoStatus.Status = status;
		irp->IoStatus.Information = 0;

		IoCompleteRequest(irp, IO_NO_INCREMENT);
		return status;
	}

	PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(irp);
	irpSp->Control = SL_INVOKE_ON_SUCCESS | SL_INVOKE_ON_ERROR | SL_INVOKE_ON_CANCEL;
	//保留原来的完成函数，如果有的话
	irpSp->Context = (PVOID)irpSp->CompletionRoutine;
	irpSp->CompletionRoutine = (PIO_COMPLETION_ROUTINE)c2pReadComplete;

	gc2pKeyCount++;
	return gOldDispatchFunc(device, irp);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
	UNREFERENCED_PARAMETER(reg_path);

	DbgPrint("Enter kbddriver hook class.\n");

	driver->DriverUnload = c2pUnload;

	PDRIVER_OBJECT pDrv = OpenKbdClass();
	if (pDrv != NULL)
	{
		// 只替换Read分发函数
		gOldDispatchFunc = pDrv->MajorFunction[IRP_MJ_READ];

		// 进行原子交换
		InterlockedExchangePointer((volatile PVOID*)&(pDrv->MajorFunction[IRP_MJ_READ]), (PVOID)c2pDispatchRead);
	}

	return STATUS_SUCCESS;
}