#include <ntddk.h>
#include <ntstrsafe.h>
#include <ntddkbd.h>

extern POBJECT_TYPE*		IoDriverObjectType;

#define KBD_DRIVER_NAME		L"\\Driver\\Kbdclass"
#define DELAY_ONE_MICROSECOND (-10)
#define DELAY_ONE_MILLISECOND (DELAY_ONE_MICROSECOND * 1000)
#define DELAY_ONE_SECOND (DELAY_ONE_MILLISECOND * 1000)

ULONG	gc2pKeyCount;

// 声明非公开函数
NTSTATUS ObReferenceObjectByName(PUNICODE_STRING ObjectName,
	ULONG Attributes,
	PACCESS_STATE AccessState,
	ACCESS_MASK DesiredAccess,
	POBJECT_TYPE ObjectType,
	KPROCESSOR_MODE AccessMode,
	PVOID ParseContext,
	PVOID *Object);

// 自定义设备扩展结构
typedef struct _C2P_DEV_EXT
{
	ULONG NodeSize;			// 这个结构大小
	PDEVICE_OBJECT pFilterDeviceObject;		// 过滤设备对象
	KSPIN_LOCK IoRequestSpinLock;			// 同时调用时的保护锁
	KEVENT IoInProgressEvent;				// 进程间同步处理
	PDEVICE_OBJECT TargetDeviceObject;		// 绑定的设备对象
	PDEVICE_OBJECT LowerDeviceObject;		// 绑定前底层设备对象
}C2P_DEV_EXT, *PC2P_DEV_EXT;

NTSTATUS c2pDevExtInit(PC2P_DEV_EXT devExt,
	PDEVICE_OBJECT pFilterOb,
	PDEVICE_OBJECT pTargetOb,
	PDEVICE_OBJECT pLowerOb)
{
	memset(devExt, 0, sizeof(C2P_DEV_EXT));
	devExt->NodeSize = sizeof(C2P_DEV_EXT);
	devExt->pFilterDeviceObject = pFilterOb;
	devExt->TargetDeviceObject = pTargetOb;
	devExt->LowerDeviceObject = pLowerOb;
	KeInitializeSpinLock(&(devExt->IoRequestSpinLock));
	KeInitializeEvent(&(devExt->IoInProgressEvent), NotificationEvent, FALSE);

	return STATUS_SUCCESS;
}

NTSTATUS c2pAttachDevices(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
	UNREFERENCED_PARAMETER(reg_path);

	DbgPrint("Enter c2pAttachDevices.\n");

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
		return status;
	}
	else
	{
		// 会导致驱动对象引用计数+1，需要解引用
		ObReferenceObject(kbdDriverObject);
	}

	// 找到设备链中第一个设备，开始遍历
	PDEVICE_OBJECT pFilterDeviceObject = NULL;
	PDEVICE_OBJECT pTargetDeviceObject = kbdDriverObject->DeviceObject;
	while (pTargetDeviceObject)
	{
		// 生成虚拟过滤设备
		status = IoCreateDevice(driver, sizeof(C2P_DEV_EXT), NULL, pTargetDeviceObject->DeviceType, 0, FALSE, &pFilterDeviceObject);
		if (!NT_SUCCESS(status))
		{
			DbgPrint("IoCreateDevice error.");
			return status;
		}

		PDEVICE_OBJECT pLowerDeviceObject = IoAttachDeviceToDeviceStack(pFilterDeviceObject, pTargetDeviceObject);
		if (!pLowerDeviceObject)
		{
			DbgPrint("IoAttachDeviceToDeviceStack error.");
			IoDeleteDevice(pFilterDeviceObject);
			pFilterDeviceObject = NULL;
			return status;
		}

		// 设备扩展
		PC2P_DEV_EXT devExt = (PC2P_DEV_EXT)(pFilterDeviceObject->DeviceExtension);
		c2pDevExtInit(devExt, pFilterDeviceObject, pTargetDeviceObject, pLowerDeviceObject);

		pFilterDeviceObject->DeviceType = pLowerDeviceObject->DeviceType;
		pFilterDeviceObject->Characteristics = pLowerDeviceObject->Characteristics;
		pFilterDeviceObject->StackSize = pLowerDeviceObject->StackSize + 1;
		pFilterDeviceObject->Flags |= pLowerDeviceObject->Flags & (DO_BUFFERED_IO | DO_DIRECT_IO | DO_POWER_PAGABLE);

		pTargetDeviceObject = pTargetDeviceObject->NextDevice;
	}

	DbgPrint("Leave c2pAttachDevices.\n");

	return status;
}


VOID c2pUnload(PDRIVER_OBJECT driver)
{
	UNREFERENCED_PARAMETER(driver);

	DbgPrint("Enter c2pUnload.\n");

	// 当前线程设置为低实模式，以便让它的运行尽量少影响其他程序
	PRKTHREAD CurrentThread = KeGetCurrentThread();
	KeSetPriorityThread(CurrentThread, LOW_REALTIME_PRIORITY);

	PDEVICE_OBJECT pOb = driver->DeviceObject;
	while (pOb)
	{
		IoDetachDevice(((PC2P_DEV_EXT)(pOb->DeviceExtension))->TargetDeviceObject);
		IoDeleteDevice(pOb);
		pOb = pOb->NextDevice;
	}

	ASSERT(NULL == driver->DeviceObject);
	LARGE_INTEGER lDelay = RtlConvertLongToLargeInteger(100 * DELAY_ONE_MILLISECOND);
	while (gc2pKeyCount)
	{
		KeDelayExecutionThread(KernelMode, FALSE, &lDelay);
	}

	DbgPrint("Leave c2pUnload.\n");
}

NTSTATUS c2pDispatchGeneral(PDEVICE_OBJECT device, PIRP irp)
{
	UNREFERENCED_PARAMETER(device);

	DbgPrint("Enter c2pDispatchGeneral.\n");

	// 跳过当前过滤设备，直接交付给真实设备
	IoSkipCurrentIrpStackLocation(irp);
	return IoCallDriver(((PC2P_DEV_EXT)(device->DeviceExtension))->LowerDeviceObject, irp);
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
			DbgPrint("ctrl2cap: %2x, keyaction: %s", 
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

	// write是已知源数据，过滤驱动是知道该数据是什么，只需要针对该数据做处理
	// read是不知道源数据，过滤驱动必须把请求交给真实设备驱动，等真实设备驱动完成请求后，方能拿到对应数据
	KEVENT waitEvent;
	KeInitializeEvent(&waitEvent, NotificationEvent, FALSE);

	if (irp->CurrentLocation == 1)
	{
		NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
		irp->IoStatus.Status = status;
		irp->IoStatus.Information = 0;

		IoCompleteRequest(irp, IO_NO_INCREMENT);
		return status;
	}

	gc2pKeyCount++;

	//PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(irp);
	IoCopyCurrentIrpStackLocationToNext(irp);		// 针对当前irp，做了修改，所以需要使用Copy而不是Skip
	IoSetCompletionRoutine(irp, c2pReadComplete, device, TRUE, TRUE, TRUE);

	return IoCallDriver(((PC2P_DEV_EXT)(device->DeviceExtension))->LowerDeviceObject, irp);
}

NTSTATUS c2pPower(PDEVICE_OBJECT device, PIRP irp)
{
	DbgPrint("Enter c2pPower.\n");

	PoStartNextPowerIrp(irp);
	IoSkipCurrentIrpStackLocation(irp);
	return PoCallDriver(((PC2P_DEV_EXT)(device->DeviceExtension))->LowerDeviceObject, irp);
}

NTSTATUS c2pPnp(PDEVICE_OBJECT device, PIRP irp)
{
	DbgPrint("Enter c2pPnp.\n");

	NTSTATUS status;

	// 即插即用，当设备拔出时，需要解除绑定
	PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);
	switch (irpStack->MinorFunction)
	{
	case IRP_MN_REMOVE_DEVICE:
		DbgPrint("IRP_MN_REMOVE_DEVICE.\n");

		// 首先把请求传递下去，然后解除绑定
		IoSkipCurrentIrpStackLocation(irp);
		status = IoCallDriver(((PC2P_DEV_EXT)(device->DeviceExtension))->LowerDeviceObject, irp);
		IoDetachDevice(((PC2P_DEV_EXT)(device->DeviceExtension))->LowerDeviceObject);
		IoDeleteDevice(device);
		break;
	default:
		IoSkipCurrentIrpStackLocation(irp);
		status = IoCallDriver(((PC2P_DEV_EXT)(device->DeviceExtension))->LowerDeviceObject, irp);
		break;
	}

	DbgPrint("Leave c2pPnp.\n");

	return status;
}


NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
	UNREFERENCED_PARAMETER(reg_path);

	DbgPrint("Enter comcap.\n");

	for (size_t i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		driver->MajorFunction[i] = c2pDispatchGeneral;
	}

	driver->MajorFunction[IRP_MJ_READ] = c2pDispatchRead;

	driver->MajorFunction[IRP_MJ_POWER] = c2pPower;

	driver->MajorFunction[IRP_MJ_PNP] = c2pPnp;

	driver->DriverUnload = c2pUnload;

	c2pAttachDevices(driver, reg_path);

	return STATUS_SUCCESS;
}