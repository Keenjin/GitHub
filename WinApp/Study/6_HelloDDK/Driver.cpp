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
	pDriverObject->DriverUnload = HelloDDKUnload;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = HelloDDKDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = HelloDDKDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = HelloDDKDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_READ] = HelloDDKDispatchRoutine;

	// ���������豸����
	NTSTATUS status = CreateDevice(pDriverObject);

	KdPrint(("DriverEntry end\n"));

	return status;
}

/****************************************************************
* �������ƣ�CreateDevice
* ������������ʼ���豸����
* �����б�
	pDriverObject����I/O�������д�����������������ִ�������-��������������͹���
* ����ֵ�����س�ʼ��״̬
****************************************************************/
#pragma INITCODE
NTSTATUS CreateDevice(
	IN PDRIVER_OBJECT pDriverObject)
{
	// �����豸����
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\KeenDDKDevice");

	// �����豸��һ����������һ��Ĭ�ϵ��豸���󣬴���������һ����׼��
	PDEVICE_OBJECT pDevObj = NULL;
	NTSTATUS status = IoCreateDevice(pDriverObject,
		sizeof(DEVICE_EXTENSION),
		&devName,
		FILE_DEVICE_UNKNOWN,		// �����豸����������ΪFILE_DEVICE_UNKNOWN
		0,
		TRUE,				// ָ�����豸Ϊ��ռ�豸�����ܱ�����Ӧ�ó�����ʹ��
		&pDevObj);			// ���������������ģ�����ͬʱҲ����ӵ���������pDriverObject��Ӧ�Ĳ�����

	if (!NT_SUCCESS(status))
	{
		return status;
	}

	pDevObj->Flags |= DO_BUFFERED_IO;		// �ڴ������ʽ��DO_BUFFERED_IO��ʾ��д����ʹ��ϵͳ���ƻ����������û�ģʽ���ݣ�
											// �����DO_DIRECT_IO���ʾ��д����ʹ���ڴ�������������û�ģʽ����
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	pDevExt->pDevice = pDevObj;				// ʹ���豸��չ��Ŀ���ǣ�����ʹ��ȫ�ֱ������и���ͬ�����⣩
	pDevExt->ustrDeviceName = devName;
	// ��������������
	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&symLinkName, L"\\??\\SymKeenHelloDDK");
	pDevExt->ustrSymLinkName = symLinkName;	// ʹ�÷������ӵ�Ŀ���ǣ������û�ģʽӦ�ó�����Է����豸����Ϊ�豸��ֻ�������ں˷���
	status = IoCreateSymbolicLink(&symLinkName, &devName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(pDevObj);
		return status;
	}

	return STATUS_SUCCESS;
}

/****************************************************************
* �������ƣ�HelloDDKUnload
* �����������������������ж�ز�������ж�������豸����ɾ���豸����ͷ�������
* �����б�
	pDriverObject����I/O�������д�����������������ִ�������-��������������͹���
* ����ֵ������״̬
****************************************************************/
#pragma PAGECODE
VOID HelloDDKUnload(IN PDRIVER_OBJECT pDriverObject)
{
	KdPrint(("Enter DriverUnload\n"));
	PDEVICE_OBJECT pNextObj = pDriverObject->DeviceObject;
	while (pNextObj != NULL)
	{
		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pNextObj->DeviceExtension;

		// ɾ����������
		UNICODE_STRING linkName = pDevExt->ustrSymLinkName;
		IoDeleteSymbolicLink(&linkName);
		pNextObj = pNextObj->NextDevice;
		IoDeleteDevice(pDevExt->pDevice);
	}
}

/****************************************************************
* �������ƣ�HelloDDKDispatchRoutine
* ������������IRP���д���
* �����б�
	pDevObj�������豸����
	pIrp:��I/0�����
* ����ֵ������״̬
****************************************************************/
#pragma PAGECODE
NTSTATUS HelloDDKDispatchRoutine(IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);
	KdPrint(("Enter HelloDDKDispatchRoutine\n"));

	NTSTATUS status = STATUS_SUCCESS;

	// ���IRP
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	KdPrint(("Leave HelloDDKDispatchRoutine\n"));

	return status;
}