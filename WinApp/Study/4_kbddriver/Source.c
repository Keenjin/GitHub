#include <ntddk.h>
#include <ntstrsafe.h>
#include <ntddkbd.h>

extern POBJECT_TYPE*		IoDriverObjectType;

#define KBD_DRIVER_NAME		L"\\Driver\\Kbdclass"
#define DELAY_ONE_MICROSECOND (-10)
#define DELAY_ONE_MILLISECOND (DELAY_ONE_MICROSECOND * 1000)
#define DELAY_ONE_SECOND (DELAY_ONE_MILLISECOND * 1000)

ULONG	gc2pKeyCount;

// �����ǹ�������
NTSTATUS ObReferenceObjectByName(PUNICODE_STRING ObjectName,
	ULONG Attributes,
	PACCESS_STATE AccessState,
	ACCESS_MASK DesiredAccess,
	POBJECT_TYPE ObjectType,
	KPROCESSOR_MODE AccessMode,
	PVOID ParseContext,
	PVOID *Object);

// �Զ����豸��չ�ṹ
typedef struct _C2P_DEV_EXT
{
	ULONG NodeSize;			// ����ṹ��С
	PDEVICE_OBJECT pFilterDeviceObject;		// �����豸����
	KSPIN_LOCK IoRequestSpinLock;			// ͬʱ����ʱ�ı�����
	KEVENT IoInProgressEvent;				// ���̼�ͬ������
	PDEVICE_OBJECT TargetDeviceObject;		// �󶨵��豸����
	PDEVICE_OBJECT LowerDeviceObject;		// ��ǰ�ײ��豸����
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

	// ��ʼ��kdbClass����������Ϊһ���ַ���
	// ����������

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
		// �ᵼ�������������ü���+1����Ҫ������
		ObReferenceObject(kbdDriverObject);
	}

	// �ҵ��豸���е�һ���豸����ʼ����
	PDEVICE_OBJECT pFilterDeviceObject = NULL;
	PDEVICE_OBJECT pTargetDeviceObject = kbdDriverObject->DeviceObject;
	while (pTargetDeviceObject)
	{
		// ������������豸
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

		// �豸��չ
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

	// ��ǰ�߳�����Ϊ��ʵģʽ���Ա����������о�����Ӱ����������
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

	// ������ǰ�����豸��ֱ�ӽ�������ʵ�豸
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

	// write����֪Դ���ݣ�����������֪����������ʲô��ֻ��Ҫ��Ը�����������
	// read�ǲ�֪��Դ���ݣ�����������������󽻸���ʵ�豸����������ʵ�豸�����������󣬷����õ���Ӧ����
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
	IoCopyCurrentIrpStackLocationToNext(irp);		// ��Ե�ǰirp�������޸ģ�������Ҫʹ��Copy������Skip
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

	// ���弴�ã����豸�γ�ʱ����Ҫ�����
	PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);
	switch (irpStack->MinorFunction)
	{
	case IRP_MN_REMOVE_DEVICE:
		DbgPrint("IRP_MN_REMOVE_DEVICE.\n");

		// ���Ȱ����󴫵���ȥ��Ȼ������
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