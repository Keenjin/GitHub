#include "Driver.h"
#include "define.h"

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
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = HelloDDKWrite;
	pDriverObject->MajorFunction[IRP_MJ_READ] = HelloDDKRead;
	pDriverObject->MajorFunction[IRP_MJ_QUERY_INFORMATION] = HelloDDKQueryInformation;
	pDriverObject->MajorFunction[IRP_MJ_CLEANUP] = HelloDDKDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = HelloDDKDeviceControl;
	pDriverObject->MajorFunction[IRP_MJ_SET_INFORMATION] = HelloDDKDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_SHUTDOWN] = HelloDDKDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = HelloDDKDispatchRoutine;

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

	pDevObj->Flags |= DO_DIRECT_IO;		// �ڴ������ʽ��DO_BUFFERED_IO��ʾ��д����ʹ��ϵͳ���ƻ����������û�ģʽ���ݣ�
											// �����DO_DIRECT_IO���ʾ��д����ʹ���ڴ�������������û�ģʽ����
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	pDevExt->pDevice = pDevObj;				// ʹ���豸��չ��Ŀ���ǣ�����ʹ��ȫ�ֱ������и���ͬ�����⣩
	pDevExt->ustrDeviceName = devName;

	//����ģ���ļ��Ļ�����
	pDevExt->buffer = (PUCHAR)ExAllocatePool(PagedPool, MAX_FILE_LENGTH);
	//����ģ���ļ���С
	pDevExt->file_length = 0;

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
		if (pDevExt->buffer)
		{
			ExFreePool(pDevExt->buffer);
			pDevExt->buffer = NULL;
		}

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
		KdPrint(("Unknown irp, major type 0x%02x\n", type));
	}
	else
	{
		KdPrint(("irpname: 0x%02x, %s\n", type, irpname[type]));
	}

	NTSTATUS status = STATUS_SUCCESS;

	// ���IRP
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;		// ����IRP�����ֽ���
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	KdPrint(("Leave HelloDDKDispatchRoutine\n"));

	return status;
}

/****************************************************************
* �������ƣ�HelloDDKRead
* �����������Զ�IRP���д���
* �����б�
	pDevObj�������豸����
	pIrp:��I/0�����
* ����ֵ������״̬
****************************************************************/
NTSTATUS HelloDDKRead(IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);
	KdPrint(("Enter HelloDDKRead\n"));

	NTSTATUS status = STATUS_SUCCESS;

	// ��ȡ��Ҫ���豸���ֽ���
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);
	ULONG ulReadLength = stack->Parameters.Read.Length;
	ULONG ulReadOffset = (ULONG)stack->Parameters.Read.ByteOffset.QuadPart;
	KdPrint(("HelloDDKRead: ulReadLength:%d, ulReadOffset:%d", ulReadLength, ulReadOffset));

	//PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

	PVOID user_address = pIrp->UserBuffer;
	__try
	{
		ProbeForWrite(user_address, ulReadLength, 4);
		memset(user_address, 0xAA, ulReadLength);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = STATUS_UNSUCCESSFUL;
	}

	// �õ�������������Ϣ
	//ULONG mdl_length = MmGetMdlByteCount(pIrp->MdlAddress);
	//PVOID mdl_address = MmGetMdlVirtualAddress(pIrp->MdlAddress);
	//ULONG mdl_offset = MmGetMdlByteOffset(pIrp->MdlAddress);
	//KdPrint(("HelloDDKRead: mdl_length:%d, mdl_address:0x%x, mdl_offset:%d", mdl_length, mdl_address, mdl_offset));

	//if ((mdl_length != ulReadLength) || (ulReadLength + ulReadOffset > MAX_FILE_LENGTH))
	//{
	//	status = STATUS_FILE_INVALID;
	//	ulReadLength = 0;
	//}
	//else
	//{
	//	// �õ��ں�ģʽ�µ�ӳ�䣬���豸��������ݣ������ں�ģʽ��ַ���൱��ӳ���������豸
	//	PVOID kernel_address = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
	//	KdPrint(("HelloDDKRead: kernel_address:0x%x", kernel_address));
	//	memcpy(kernel_address, pDevExt->buffer + ulReadOffset, ulReadLength);
	//	status = STATUS_SUCCESS;
	//}

	//if (ulReadLength + ulReadOffset > MAX_FILE_LENGTH)
	//{
	//	status = STATUS_FILE_INVALID;
	//	ulReadLength = 0;
	//}
	//else
	//{
	//	// �����ݱ����ڻ��������Ա����ʹ��
	//	memcpy(pIrp->AssociatedIrp.SystemBuffer, pDevExt->buffer + ulReadOffset, ulReadLength);
	//	status = STATUS_SUCCESS;
	//}

	// ����IRP���״̬
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = ulReadLength;		// ����ʵ�ʶ�ȡ���ֽ���
	
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("Leave HelloDDKRead\n"));

	return status;
}

/****************************************************************
* �������ƣ�HelloDDKWrite
* ������������дIRP���д���
* �����б�
	pDevObj�������豸����
	pIrp:��I/0�����
* ����ֵ������״̬
****************************************************************/
NTSTATUS HelloDDKWrite(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	KdPrint(("Enter HelloDDKWrite\n"));

	NTSTATUS status = STATUS_SUCCESS;

	// ��ȡ��Ҫ���豸���ֽ���
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);
	ULONG ulWriteLength = stack->Parameters.Write.Length;
	ULONG ulWriteOffset = (ULONG)stack->Parameters.Write.ByteOffset.QuadPart;
	// ��д������ݣ��洢�ڻ�������
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	KdPrint(("HelloDDKWrite: ulWriteLength:%d, ulWriteOffset:%d", ulWriteLength, ulWriteOffset));

	// �õ�������������Ϣ
	ULONG mdl_length = MmGetMdlByteCount(pIrp->MdlAddress);
	PVOID mdl_address = MmGetMdlVirtualAddress(pIrp->MdlAddress);
	ULONG mdl_offset = MmGetMdlByteOffset(pIrp->MdlAddress);
	KdPrint(("HelloDDKRead: mdl_length:%d, mdl_address:0x%x, mdl_offset:%d", mdl_length, mdl_address, mdl_offset));

	if ((mdl_length != ulWriteLength) || (ulWriteOffset + ulWriteLength > MAX_FILE_LENGTH))
	{
		status = STATUS_FILE_INVALID;
		ulWriteLength = 0;
	}
	else
	{
		PVOID kernel_address = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
		KdPrint(("HelloDDKRead: kernel_address:0x%x", kernel_address));
		memcpy(pDevExt->buffer + ulWriteOffset, kernel_address, ulWriteLength);
		status = STATUS_SUCCESS;
		if (ulWriteLength + ulWriteOffset > pDevExt->file_length)
		{
			pDevExt->file_length = ulWriteLength + ulWriteOffset;
		}
	}

	//if (ulWriteOffset+ ulWriteLength > MAX_FILE_LENGTH)
	//{
	//	// �洢���ȴ��ڻ���������
	//	status = STATUS_FILE_INVALID;
	//	ulWriteLength = 0;
	//}
	//else
	//{
	//	// ��д������ݣ��洢�ڻ�����
	//	memcpy(pDevExt->buffer + ulWriteOffset, pIrp->AssociatedIrp.SystemBuffer, ulWriteLength);
	//	status = STATUS_SUCCESS;
	//	if (ulWriteLength + ulWriteOffset > pDevExt->file_length)
	//	{
	//		pDevExt->file_length = ulWriteLength + ulWriteOffset;
	//	}
	//}	

	// ����IRP���״̬
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = ulWriteLength;		// ����ʵ�ʶ�ȡ���ֽ���

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("Leave HelloDDKWrite\n"));

	return status;
}

NTSTATUS HelloDDKQueryInformation(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	KdPrint(("Enter HelloDDKQueryInformation\n"));

	NTSTATUS status = STATUS_SUCCESS;

	// ��ȡ��Ҫ���豸���ֽ���
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);
	FILE_INFORMATION_CLASS info = stack->Parameters.QueryFile.FileInformationClass;

	// ��д������ݣ��洢�ڻ�������
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

	if (info == FileStandardInformation)
	{
		// ��׼�ļ�
		KdPrint(("FileStandardInformation\n"));
		PFILE_STANDARD_INFORMATION file_info = (PFILE_STANDARD_INFORMATION)pIrp->AssociatedIrp.SystemBuffer;
		file_info->EndOfFile = RtlConvertLongToLargeInteger(pDevExt->file_length);
	}

	// ����IRP���״̬
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = stack->Parameters.QueryFile.Length;		// ����ʵ�ʶ�ȡ���ֽ���

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("Leave HelloDDKQueryInformation\n"));

	return status;
}

NTSTATUS HelloDDKDeviceControl(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);
	KdPrint(("Enter HelloDDKDeviceControl\n"));

	NTSTATUS status = STATUS_SUCCESS;

	// ��ȡ��Ҫ���豸���ֽ���
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);

	// �õ����뻺��������
	ULONG cbin = stack->Parameters.DeviceIoControl.InputBufferLength;
	ULONG cbout = stack->Parameters.DeviceIoControl.OutputBufferLength;
	ULONG code = stack->Parameters.DeviceIoControl.IoControlCode;

	ULONG length = 0;
	switch (code)
	{
	case IOCTL_TEST1:
		{
			KdPrint(("HelloDDKDeviceControl, IOCTL_TEST1\n"));
			// ���뻺����
			UCHAR* bufferIn = (UCHAR*)pIrp->AssociatedIrp.SystemBuffer;
			for (size_t i = 0; i < cbin; i++)
			{
				KdPrint(("0x%x ", bufferIn[i]));
			}
			KdPrint(("\n"));
			// ���������
			UCHAR* bufferOut = (UCHAR*)pIrp->AssociatedIrp.SystemBuffer;
			memset(bufferOut, 0x11, cbout);
			length = cbout;
		}
		break;
	case IOCTL_TEST2:
		{
			KdPrint(("HelloDDKDeviceControl, IOCTL_TEST2\n"));
			// ���뻺����
			UCHAR* bufferIn = (UCHAR*)pIrp->AssociatedIrp.SystemBuffer;
			for (size_t i = 0; i < cbin; i++)
			{
				KdPrint(("0x%x ", bufferIn[i]));
			}
			KdPrint(("\n"));
			// ���������
			KdPrint(("user address: 0x%x\n", MmGetMdlVirtualAddress(pIrp->MdlAddress)));
			UCHAR* bufferOut = (UCHAR*)MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
			KdPrint(("kernel address: 0x%x\n", bufferOut));
			memset(bufferOut, 0x22, cbout);
			length = cbout;
		}
		break;
	case IOCTL_TEST3:
		{
			KdPrint(("HelloDDKDeviceControl, IOCTL_TEST3\n"));
			// ���뻺����
			UCHAR* bufferIn = (UCHAR*)stack->Parameters.DeviceIoControl.Type3InputBuffer;
			KdPrint(("user address in: 0x%x\n", bufferIn));
			
			// ���������
			UCHAR* bufferOut = (UCHAR*)pIrp->UserBuffer;
			KdPrint(("user address out: 0x%x\n", bufferOut));
			__try {
				// �ж�ָ��ɶ���Ȼ����ʾ���뻺��������
				ProbeForRead(bufferIn, cbin, 4);
				for (size_t i = 0; i < cbin; i++)
				{
					KdPrint(("0x%x ", bufferIn[i]));
				}
				KdPrint(("\n"));

				// �ж�ָ���д��Ȼ��������ݵ����������
				ProbeForWrite(bufferOut, cbout, 4);
				memset(bufferOut, 0x33, cbout);
				length = cbout;
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
				status = STATUS_UNSUCCESSFUL;
			}
		}
		break;
	default:
		status = STATUS_INVALID_VARIANT;
		break;
	}

	// ����IRP���״̬
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = length;		// ����ʵ�ʶ�ȡ���ֽ���

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("Leave HelloDDKDeviceControl\n"));

	return status;
}