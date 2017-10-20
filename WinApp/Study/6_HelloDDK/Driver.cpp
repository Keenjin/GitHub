#include "Driver.h"

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
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = HelloDDKDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_READ] = HelloDDKDispatchRoutine;

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

	pDevObj->Flags |= DO_BUFFERED_IO;		// 内存操作方式。DO_BUFFERED_IO表示读写操作使用系统复制缓冲区访问用户模式数据；
											// 如果是DO_DIRECT_IO则表示读写操作使用内存描述符表访问用户模式数据
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	pDevExt->pDevice = pDevObj;				// 使用设备扩展的目的是，避免使用全局变量（有各种同步问题）
	pDevExt->ustrDeviceName = devName;
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

	NTSTATUS status = STATUS_SUCCESS;

	// 完成IRP
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	KdPrint(("Leave HelloDDKDispatchRoutine\n"));

	return status;
}