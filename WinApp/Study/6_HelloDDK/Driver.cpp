#include "Driver.h"
#include "define.h"

/****************************************************************
* 函数名称：DriverEntry
* 功能描述：初始化驱动程序，定位和申请硬件资源，创建内核对象
* 参数列表：
	pDriverObject：从I/O管理器中传进来的驱动对象，由执行体组件-对象管理器创建和管理
	pRegistryPath：驱动程序在注册表中的路径，由执行体组件-配置管理器创建和管理
* 返回值：返回初始化驱动状态
****************************************************************/
#pragma INITCODE		// 指明DriverEntry是加载到INIT内存区域中，驱动成功卸载后，可以退出内存
extern "C" NTSTATUS DriverEntry(		// 特定的回调函数，都需要编译成C的函数命名
	IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath) 
{
	UNREFERENCED_PARAMETER(pRegistryPath);
	KdPrint(("Enter DriverEntry\n"));		// 使用KdPrint宏，可以在调试版本(Checked/Debug)打日志，发行版本(Free/Release)不打日志

	// 注册其他驱动调用函数入口
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

	// 创建驱动设备对象
	NTSTATUS status = CreateDevice(pDriverObject);

	KdPrint(("DriverEntry end\n"));

	return status;
}

/****************************************************************
* 函数名称：CreateDevice
* 功能描述：初始化设备对象
* 参数列表：
	pDriverObject：从I/O管理器中传进来的驱动对象，由执行体组件-对象管理器创建和管理
* 返回值：返回初始化状态
****************************************************************/
#pragma INITCODE
NTSTATUS CreateDevice(
	IN PDRIVER_OBJECT pDriverObject)
{
	// 创建设备名称
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\KeenDDKDevice");

	// 创建设备。一个驱动，有一个默认的设备对象，大概这个就是一个标准吧
	PDEVICE_OBJECT pDevObj = NULL;
	NTSTATUS status = IoCreateDevice(pDriverObject,
		sizeof(DEVICE_EXTENSION),
		&devName,
		FILE_DEVICE_UNKNOWN,		// 虚拟设备，必须设置为FILE_DEVICE_UNKNOWN
		0,
		TRUE,				// 指定此设备为独占设备，不能被其他应用程序所使用
		&pDevObj);			// 对象管理器来管理的，这里同时也会添加到驱动对象pDriverObject相应的参数中

	if (!NT_SUCCESS(status))
	{
		return status;
	}

	pDevObj->Flags |= DO_DIRECT_IO;		// 内存操作方式。DO_BUFFERED_IO表示读写操作使用系统复制缓冲区访问用户模式数据；
											// 如果是DO_DIRECT_IO则表示读写操作使用内存描述符表访问用户模式数据
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	pDevExt->pDevice = pDevObj;				// 使用设备扩展的目的是，避免使用全局变量（有各种同步问题）
	pDevExt->ustrDeviceName = devName;

	//申请模拟文件的缓冲区
	pDevExt->buffer = (PUCHAR)ExAllocatePool(PagedPool, MAX_FILE_LENGTH);
	//设置模拟文件大小
	pDevExt->file_length = 0;

	// 创建符号链接名
	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&symLinkName, L"\\??\\SymKeenHelloDDK");
	pDevExt->ustrSymLinkName = symLinkName;	// 使用符号链接的目的是，方便用户模式应用程序可以访问设备。因为设备名只能用于内核访问
	status = IoCreateSymbolicLink(&symLinkName, &devName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(pDevObj);
		return status;
	}

	return STATUS_SUCCESS;
}

/****************************************************************
* 函数名称：HelloDDKUnload
* 功能描述：负责驱动程序的卸载操作。会卸载整个设备链，删除设备对象和符号链接
* 参数列表：
	pDriverObject：从I/O管理器中传进来的驱动对象，由执行体组件-对象管理器创建和管理
* 返回值：返回状态
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

		// 删除符号链接
		UNICODE_STRING linkName = pDevExt->ustrSymLinkName;
		IoDeleteSymbolicLink(&linkName);
		pNextObj = pNextObj->NextDevice;
		IoDeleteDevice(pDevExt->pDevice);
	}
}

/****************************************************************
* 函数名称：HelloDDKDispatchRoutine
* 功能描述：对IRP进行处理
* 参数列表：
	pDevObj：功能设备对象
	pIrp:从I/0请求包
* 返回值：返回状态
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

	// 完成IRP
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;		// 设置IRP操作字节数
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	KdPrint(("Leave HelloDDKDispatchRoutine\n"));

	return status;
}

/****************************************************************
* 函数名称：HelloDDKRead
* 功能描述：对读IRP进行处理
* 参数列表：
	pDevObj：功能设备对象
	pIrp:从I/0请求包
* 返回值：返回状态
****************************************************************/
NTSTATUS HelloDDKRead(IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);
	KdPrint(("Enter HelloDDKRead\n"));

	NTSTATUS status = STATUS_SUCCESS;

	// 获取需要读设备的字节数
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

	// 得到锁定缓冲区信息
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
	//	// 得到内核模式下的映射，将设备里面的数据，读入内核模式地址，相当于映射给了外界设备
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
	//	// 将数据保存在缓冲区，以便后续使用
	//	memcpy(pIrp->AssociatedIrp.SystemBuffer, pDevExt->buffer + ulReadOffset, ulReadLength);
	//	status = STATUS_SUCCESS;
	//}

	// 设置IRP完成状态
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = ulReadLength;		// 设置实际读取的字节数
	
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("Leave HelloDDKRead\n"));

	return status;
}

/****************************************************************
* 函数名称：HelloDDKWrite
* 功能描述：对写IRP进行处理
* 参数列表：
	pDevObj：功能设备对象
	pIrp:从I/0请求包
* 返回值：返回状态
****************************************************************/
NTSTATUS HelloDDKWrite(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	KdPrint(("Enter HelloDDKWrite\n"));

	NTSTATUS status = STATUS_SUCCESS;

	// 获取需要读设备的字节数
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);
	ULONG ulWriteLength = stack->Parameters.Write.Length;
	ULONG ulWriteOffset = (ULONG)stack->Parameters.Write.ByteOffset.QuadPart;
	// 将写入的数据，存储在缓冲区内
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	KdPrint(("HelloDDKWrite: ulWriteLength:%d, ulWriteOffset:%d", ulWriteLength, ulWriteOffset));

	// 得到锁定缓冲区信息
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
	//	// 存储长度大于缓冲区长度
	//	status = STATUS_FILE_INVALID;
	//	ulWriteLength = 0;
	//}
	//else
	//{
	//	// 将写入的数据，存储在缓冲区
	//	memcpy(pDevExt->buffer + ulWriteOffset, pIrp->AssociatedIrp.SystemBuffer, ulWriteLength);
	//	status = STATUS_SUCCESS;
	//	if (ulWriteLength + ulWriteOffset > pDevExt->file_length)
	//	{
	//		pDevExt->file_length = ulWriteLength + ulWriteOffset;
	//	}
	//}	

	// 设置IRP完成状态
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = ulWriteLength;		// 设置实际读取的字节数

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("Leave HelloDDKWrite\n"));

	return status;
}

NTSTATUS HelloDDKQueryInformation(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	KdPrint(("Enter HelloDDKQueryInformation\n"));

	NTSTATUS status = STATUS_SUCCESS;

	// 获取需要读设备的字节数
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);
	FILE_INFORMATION_CLASS info = stack->Parameters.QueryFile.FileInformationClass;

	// 将写入的数据，存储在缓冲区内
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

	if (info == FileStandardInformation)
	{
		// 标准文件
		KdPrint(("FileStandardInformation\n"));
		PFILE_STANDARD_INFORMATION file_info = (PFILE_STANDARD_INFORMATION)pIrp->AssociatedIrp.SystemBuffer;
		file_info->EndOfFile = RtlConvertLongToLargeInteger(pDevExt->file_length);
	}

	// 设置IRP完成状态
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = stack->Parameters.QueryFile.Length;		// 设置实际读取的字节数

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("Leave HelloDDKQueryInformation\n"));

	return status;
}

NTSTATUS HelloDDKDeviceControl(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDevObj);
	KdPrint(("Enter HelloDDKDeviceControl\n"));

	NTSTATUS status = STATUS_SUCCESS;

	// 获取需要读设备的字节数
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);

	// 得到输入缓冲区数据
	ULONG cbin = stack->Parameters.DeviceIoControl.InputBufferLength;
	ULONG cbout = stack->Parameters.DeviceIoControl.OutputBufferLength;
	ULONG code = stack->Parameters.DeviceIoControl.IoControlCode;

	ULONG length = 0;
	switch (code)
	{
	case IOCTL_TEST1:
		{
			KdPrint(("HelloDDKDeviceControl, IOCTL_TEST1\n"));
			// 输入缓冲区
			UCHAR* bufferIn = (UCHAR*)pIrp->AssociatedIrp.SystemBuffer;
			for (size_t i = 0; i < cbin; i++)
			{
				KdPrint(("0x%x ", bufferIn[i]));
			}
			KdPrint(("\n"));
			// 输出缓冲区
			UCHAR* bufferOut = (UCHAR*)pIrp->AssociatedIrp.SystemBuffer;
			memset(bufferOut, 0x11, cbout);
			length = cbout;
		}
		break;
	case IOCTL_TEST2:
		{
			KdPrint(("HelloDDKDeviceControl, IOCTL_TEST2\n"));
			// 输入缓冲区
			UCHAR* bufferIn = (UCHAR*)pIrp->AssociatedIrp.SystemBuffer;
			for (size_t i = 0; i < cbin; i++)
			{
				KdPrint(("0x%x ", bufferIn[i]));
			}
			KdPrint(("\n"));
			// 输出缓冲区
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
			// 输入缓冲区
			UCHAR* bufferIn = (UCHAR*)stack->Parameters.DeviceIoControl.Type3InputBuffer;
			KdPrint(("user address in: 0x%x\n", bufferIn));
			
			// 输出缓冲区
			UCHAR* bufferOut = (UCHAR*)pIrp->UserBuffer;
			KdPrint(("user address out: 0x%x\n", bufferOut));
			__try {
				// 判断指针可读，然后显示输入缓冲区内容
				ProbeForRead(bufferIn, cbin, 4);
				for (size_t i = 0; i < cbin; i++)
				{
					KdPrint(("0x%x ", bufferIn[i]));
				}
				KdPrint(("\n"));

				// 判断指针可写，然后输出内容到输出缓冲区
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

	// 设置IRP完成状态
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = length;		// 设置实际读取的字节数

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	KdPrint(("Leave HelloDDKDeviceControl\n"));

	return status;
}