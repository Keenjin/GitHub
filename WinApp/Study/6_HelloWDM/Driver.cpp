#include "Driver.h"

/****************************************************************
* �������ƣ�DriverEntry
* ������������ʼ���������򣬶�λ������Ӳ����Դ�������ں˶���
* �����б�
pDriverObject����I/O�������д�����������������ִ�������-��������������͹���
pRegistryPath������������ע����е�·������ִ�������-���ù����������͹���
* ����ֵ�����س�ʼ������״̬
****************************************************************/
#pragma INITCODE		// ָ��DriverEntry�Ǽ��ص�INIT�ڴ������У������ɹ�ж�غ󣬿����˳��ڴ�
extern "C" NTSTATUS DriverEntry(		// �ض��Ļص�����������Ҫ�����C�ĺ�������
	IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath)
{
	UNREFERENCED_PARAMETER(pRegistryPath);
	KdPrint(("Enter DriverEntry\n"));		// ʹ��KdPrint�꣬�����ڵ��԰汾(Checked/Debug)����־�����а汾(Free/Release)������־

											// ע�������������ú������
	pDriverObject->DriverExtension->AddDevice = HelloWDMAddDevice;		// ���弴�õ�֧��
	pDriverObject->MajorFunction[IRP_MJ_PNP] = HelloWDMPnp;

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = HelloWDMDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = HelloWDMDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = HelloWDMDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_READ] = HelloWDMDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_CLEANUP] = HelloWDMDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = HelloWDMDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_SET_INFORMATION] = HelloWDMDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_SHUTDOWN] = HelloWDMDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = HelloWDMDispatchRoutine;

	pDriverObject->DriverUnload = HelloWDMUnload;

	KdPrint(("DriverEntry end\n"));
	return STATUS_SUCCESS;
}

/****************************************************************
* �������ƣ�HelloWDMAddDevice
* ����������������豸����PNP�������������
* �����б�
pDriverObject����I/O�������д�����������������ִ�������-��������������͹���
* ����ֵ�����س�ʼ��״̬
****************************************************************/
#pragma PAGEDCODE		// ��ҳ�ڴ�
NTSTATUS HelloWDMAddDevice(IN PDRIVER_OBJECT pDriverObject,
	IN PDEVICE_OBJECT pPhysicalDeviceObject)
{
	PAGED_CODE();		// �ж����󼶱��У�飬����APC_LEVELʱ�������һ������
	KdPrint(("Enter HelloWDMAddDevice\n"));

	// �����豸����
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\KeenWDMDevice");

	// �����豸��һ����������һ��Ĭ�ϵ��豸���󣬴���������һ����׼��
	PDEVICE_OBJECT fdo = NULL;
	NTSTATUS status = IoCreateDevice(pDriverObject,
		sizeof(DEVICE_EXTENSION),
		&devName,
		FILE_DEVICE_UNKNOWN,		// �����豸����������ΪFILE_DEVICE_UNKNOWN
		0,
		TRUE,				// ָ�����豸Ϊ��ռ�豸�����ܱ�����Ӧ�ó�����ʹ��
		&fdo);				// ���������������ģ�����ͬʱҲ����ӵ���������pDriverObject��Ӧ�Ĳ�����

	if (!NT_SUCCESS(status))
	{
		return status;
	}

	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)fdo->DeviceExtension;
	pDevExt->fdo = fdo;				// ʹ���豸��չ��Ŀ���ǣ�����ʹ��ȫ�ֱ������и���ͬ�����⣩
	pDevExt->NextStackDevice = IoAttachDeviceToDeviceStack(fdo, pPhysicalDeviceObject);		// �����豸�ҽӵ��豸ջ�������������豸��������ֵ����һ���ջ���豸����
	pDevExt->ustrDeviceName = devName;

	// ��������������
	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&symLinkName, L"\\DosDevices\\SymKeenHelloWDM");		// ����ȿ���ʹ��\\DosDevices\\��Ҳ����ʹ��\\??\\  
	pDevExt->ustrSymLinkName = symLinkName;	// ʹ�÷������ӵ�Ŀ���ǣ������û�ģʽӦ�ó�����Է����豸����Ϊ�豸��ֻ�������ں˷���
	status = IoCreateSymbolicLink(&symLinkName, &devName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(fdo);
		return status;
	}

	fdo->Flags |= DO_BUFFERED_IO | DO_POWER_PAGABLE;		// �ڴ������ʽ��DO_BUFFERED_IO��ʾ��д����ʹ��ϵͳ���ƻ����������û�ģʽ���ݣ�
															// �����DO_DIRECT_IO���ʾ��д����ʹ���ڴ�������������û�ģʽ����
	fdo->Flags &= ~DO_DEVICE_INITIALIZING;

	KdPrint(("Leave HelloWDMAddDevice\n"));
	return STATUS_SUCCESS;
}

/****************************************************************
* �������ƣ�HelloWDMPnp
* �����������Լ��弴��IRP���д���
* �����б�
fdo�������豸����
pIrp����I/O�����
* ����ֵ������״̬
****************************************************************/
#pragma PAGEDCODE
NTSTATUS HelloWDMPnp(IN PDEVICE_OBJECT fdo, IN PIRP pIrp)
{
	PAGED_CODE();		// ȷ���ú��������ڵ���APC_LEVEL���ж����ȼ�

	KdPrint(("Enter HelloWDMPnp\n"));
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION)fdo->DeviceExtension;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);

	// ��ͬIRP�Ĵ�������
	static NTSTATUS(*fcntab[])(PDEVICE_EXTENSION pdx, PIRP pIrp) = {
		DefaultPnpHandler,			// IRP_MN_START_DEVICE
		DefaultPnpHandler,			// IRP_MN_QUERY_REMOVE_DEVICE
		HandleRemoveDevice,			// IRP_MN_REMOVE_DEVICE
		DefaultPnpHandler,			// IRP_MN_CANCEL_REMOVE_DEVICE
		DefaultPnpHandler,			// IRP_MN_STOP_DEVICE
		DefaultPnpHandler,			// IRP_MN_QUERY_STOP_DEVICE
		DefaultPnpHandler,			// IRP_MN_CANCEL_STOP_DEVICE
		DefaultPnpHandler,			// IRP_MN_QUERY_DEVICE_RELATIONS
		DefaultPnpHandler,			// IRP_MN_QUERY_INTERFACE
		DefaultPnpHandler,			// IRP_MN_QUERY_CAPABILITIES
		DefaultPnpHandler,			// IRP_MN_QUERY_RESOURCES
		DefaultPnpHandler,			// IRP_MN_QUERY_RESOURCE_REQUIREMENTS
		DefaultPnpHandler,			// IRP_MN_QUERY_DEVICE_TEXT
		DefaultPnpHandler,			// IRP_MN_FILTER_RESOURCE_REQUIREMENTS
		DefaultPnpHandler,
		DefaultPnpHandler,			// IRP_MN_READ_CONFIG
		DefaultPnpHandler,			// IRP_MN_WRITE_CONFIG
		DefaultPnpHandler,			// IRP_MN_EJECT
		DefaultPnpHandler,			// IRP_MN_SET_LOCK
		DefaultPnpHandler,			// IRP_MN_QUERY_ID
		DefaultPnpHandler,			// IRP_MN_QUERY_PNP_DEVICE_STATE
		DefaultPnpHandler,			// IRP_MN_QUERY_BUS_INFORMATION
		DefaultPnpHandler,			// IRP_MN_DEVICE_USAGE_NOTIFICATION
		DefaultPnpHandler,			// IRP_MN_SURPRISE_REMOVAL
	};
	ULONG fcn = stack->MinorFunction;		// ��ǰIRP�����ͺ�
											// δ֪IRP�Ĵ���
	if (fcn >= arraysize(fcntab))
	{
		status = DefaultPnpHandler(pdx, pIrp);
		return status;
	}

	status = (*fcntab[fcn])(pdx, pIrp);
	KdPrint(("Leave HelloWDMPnp\n"));

	return status;
}

/****************************************************************
* �������ƣ�DefaultPnpHandler
* ������������PNP IRP����Ĭ�ϴ���
* �����б�
pdx���豸�������չ
pIrp����I/O�����
* ����ֵ������״̬
****************************************************************/
#pragma PAGEDCODE
NTSTATUS DefaultPnpHandler(IN PDEVICE_EXTENSION pdx, IN PIRP pIrp)
{
	PAGED_CODE();
	KdPrint(("Enter DefaultPnpHandler\n"));
	IoSkipCurrentIrpStackLocation(pIrp);		// �Թ���ǰIRPջ��ֱ����ת����һ�㴦��
	KdPrint(("Leave DefaultPnpHandler\n"));
	return IoCallDriver(pdx->NextStackDevice, pIrp);
}

/****************************************************************
* �������ƣ�HandleRemoveDevice
* ������������IRP_MN_REMOVE_DEVICE���д���Ҳ�����豸ж��
* �����б�
pdx���豸�������չ
pIrp����I/O�����
* ����ֵ������״̬
****************************************************************/
#pragma PAGEDCODE
NTSTATUS HandleRemoveDevice(IN PDEVICE_EXTENSION pdx, IN PIRP pIrp)
{
	PAGED_CODE();
	KdPrint(("Enter HandleRemoveDevice\n"));

	pIrp->IoStatus.Status = STATUS_SUCCESS;			// ������IRP��״̬Ϊ˳�����
	NTSTATUS status = DefaultPnpHandler(pdx, pIrp);
	IoDeleteSymbolicLink(&pdx->ustrSymLinkName);

	// ��fdo���豸ջ������
	if (pdx->NextStackDevice)
	{
		IoDetachDevice(pdx->NextStackDevice);		// ж���豸����
	}

	// ɾ��fdo
	IoDeleteDevice(pdx->fdo);
	KdPrint(("Leave HandleRemoveDevice\n"));
	return status;
}

/****************************************************************
* �������ƣ�HelloWDMDispatchRoutine
* ������������IRP���д���
* �����б�
fdo�������豸����
pIrp:��I/0�����
* ����ֵ������״̬
****************************************************************/
#pragma PAGEDCODE
NTSTATUS HelloWDMDispatchRoutine(IN PDEVICE_OBJECT fdo,
	IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(fdo);
	PAGED_CODE();

	KdPrint(("Enter HelloWDMDispatchRoutine\n"));

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);

	static char* irpname[] = {
		"IRP_MJ_CREATE",
		"IRP_MJ_CREATE_NAMED_PIPE",
		"IRP_MJ_CLOSE",
		"IRP_MJ_READ",
		"IRP_MJ_WRITE",
		"IRP_MJ_QUERY_INFORMATION",
		"IRP_MJ_SET_INFORMATION",
		"IRP_MJ_QUERY_EA",
		"IRP_MJ_SET_EA",
		"IRP_MJ_FLUSH_BUFFERS",
		"IRP_MJ_QUERY_VOLUME_INFORMATION",
		"IRP_MJ_SET_VOLUME_INFORMATION",
		"IRP_MJ_DIRECTORY_CONTROL",
		"IRP_MJ_FILE_SYSTEM_CONTROL",
		"IRP_MJ_DEVICE_CONTROL",
		"IRP_MJ_INTERNAL_DEVICE_CONTROL",
		"IRP_MJ_SHUTDOWN",
		"IRP_MJ_LOCK_CONTROL",
		"IRP_MJ_CLEANUP",
		"IRP_MJ_CREATE_MAILSLOT",
		"IRP_MJ_QUERY_SECURITY",
		"IRP_MJ_SET_SECURITY",
		"IRP_MJ_POWER",
		"IRP_MJ_SYSTEM_CONTROL",
		"IRP_MJ_DEVICE_CHANGE",
		"IRP_MJ_QUERY_QUOTA",
		"IRP_MJ_SET_QUOTA",
		"IRP_MJ_PNP"
	};

	UCHAR type = stack->MajorFunction;
	if (type >= arraysize(irpname))
	{
		KdPrint(("Unknown irp, major type %x\n", type));
	}
	else
	{
		KdPrint(("irpname: %s\n", irpname[type]));
	}

	NTSTATUS status = STATUS_SUCCESS;

	// ���IRP
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	KdPrint(("Leave HelloWDMDispatchRoutine\n"));

	return status;
}

/****************************************************************
* �������ƣ�HelloWDMUnload
* �����������������������ж�ز�������ж�������豸����ɾ���豸����ͷ�������
* �����б�
pDriverObject����I/O�������д�����������������ִ�������-��������������͹���
* ����ֵ������״̬
****************************************************************/
#pragma PAGEDCODE
VOID HelloWDMUnload(IN PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);
	PAGED_CODE();
	KdPrint(("Enter HelloWDMUnload\n"));
	KdPrint(("Leave HelloWDMUnload\n"));
}