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
	pDriverObject->DriverExtension->AddDevice = HelloWDMAddDevice;		// 即插即用的支持
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
* 函数名称：HelloWDMAddDevice
* 功能描述：添加新设备。由PNP管理器负责调用
* 参数列表：
pDriverObject：从I/O管理器中传进来的驱动对象，由执行体组件-对象管理器创建和管理
* 返回值：返回初始化状态
****************************************************************/
#pragma PAGEDCODE		// 分页内存
NTSTATUS HelloWDMAddDevice(IN PDRIVER_OBJECT pDriverObject,
	IN PDEVICE_OBJECT pPhysicalDeviceObject)
{
	PAGED_CODE();		// 中断请求级别的校验，超过APC_LEVEL时，会产生一个断言
	KdPrint(("Enter HelloWDMAddDevice\n"));

	// 创建设备名称
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\KeenWDMDevice");

	// 创建设备。一个驱动，有一个默认的设备对象，大概这个就是一个标准吧
	PDEVICE_OBJECT fdo = NULL;
	NTSTATUS status = IoCreateDevice(pDriverObject,
		sizeof(DEVICE_EXTENSION),
		&devName,
		FILE_DEVICE_UNKNOWN,		// 虚拟设备，必须设置为FILE_DEVICE_UNKNOWN
		0,
		TRUE,				// 指定此设备为独占设备，不能被其他应用程序所使用
		&fdo);				// 对象管理器来管理的，这里同时也会添加到驱动对象pDriverObject相应的参数中

	if (!NT_SUCCESS(status))
	{
		return status;
	}

	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)fdo->DeviceExtension;
	pDevExt->fdo = fdo;				// 使用设备扩展的目的是，避免使用全局变量（有各种同步问题）
	pDevExt->NextStackDevice = IoAttachDeviceToDeviceStack(fdo, pPhysicalDeviceObject);		// 将此设备挂接到设备栈（这里是物理设备），返回值是下一层堆栈的设备对象
	pDevExt->ustrDeviceName = devName;

	// 创建符号链接名
	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&symLinkName, L"\\DosDevices\\SymKeenHelloWDM");		// 这里既可以使用\\DosDevices\\，也可以使用\\??\\  
	pDevExt->ustrSymLinkName = symLinkName;	// 使用符号链接的目的是，方便用户模式应用程序可以访问设备。因为设备名只能用于内核访问
	status = IoCreateSymbolicLink(&symLinkName, &devName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(fdo);
		return status;
	}

	fdo->Flags |= DO_BUFFERED_IO | DO_POWER_PAGABLE;		// 内存操作方式。DO_BUFFERED_IO表示读写操作使用系统复制缓冲区访问用户模式数据；
															// 如果是DO_DIRECT_IO则表示读写操作使用内存描述符表访问用户模式数据
	fdo->Flags &= ~DO_DEVICE_INITIALIZING;

	KdPrint(("Leave HelloWDMAddDevice\n"));
	return STATUS_SUCCESS;
}

/****************************************************************
* 函数名称：HelloWDMPnp
* 功能描述：对即插即用IRP进行处理
* 参数列表：
fdo：功能设备对象
pIrp：从I/O请求包
* 返回值：返回状态
****************************************************************/
#pragma PAGEDCODE
NTSTATUS HelloWDMPnp(IN PDEVICE_OBJECT fdo, IN PIRP pIrp)
{
	PAGED_CODE();		// 确保该函数运行在低于APC_LEVEL的中断优先级

	KdPrint(("Enter HelloWDMPnp\n"));
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION)fdo->DeviceExtension;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);

	// 不同IRP的处理例程
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
	ULONG fcn = stack->MinorFunction;		// 当前IRP辅类型号
											// 未知IRP的处理
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
* 函数名称：DefaultPnpHandler
* 功能描述：对PNP IRP进行默认处理
* 参数列表：
pdx：设备对象的扩展
pIrp：从I/O请求包
* 返回值：返回状态
****************************************************************/
#pragma PAGEDCODE
NTSTATUS DefaultPnpHandler(IN PDEVICE_EXTENSION pdx, IN PIRP pIrp)
{
	PAGED_CODE();
	KdPrint(("Enter DefaultPnpHandler\n"));
	IoSkipCurrentIrpStackLocation(pIrp);		// 略过当前IRP栈，直接跳转到下一层处理
	KdPrint(("Leave DefaultPnpHandler\n"));
	return IoCallDriver(pdx->NextStackDevice, pIrp);
}

/****************************************************************
* 函数名称：HandleRemoveDevice
* 功能描述：对IRP_MN_REMOVE_DEVICE进行处理，也就是设备卸载
* 参数列表：
pdx：设备对象的扩展
pIrp：从I/O请求包
* 返回值：返回状态
****************************************************************/
#pragma PAGEDCODE
NTSTATUS HandleRemoveDevice(IN PDEVICE_EXTENSION pdx, IN PIRP pIrp)
{
	PAGED_CODE();
	KdPrint(("Enter HandleRemoveDevice\n"));

	pIrp->IoStatus.Status = STATUS_SUCCESS;			// 表明此IRP的状态为顺利完成
	NTSTATUS status = DefaultPnpHandler(pdx, pIrp);
	IoDeleteSymbolicLink(&pdx->ustrSymLinkName);

	// 把fdo从设备栈中脱离
	if (pdx->NextStackDevice)
	{
		IoDetachDevice(pdx->NextStackDevice);		// 卸载设备对象
	}

	// 删除fdo
	IoDeleteDevice(pdx->fdo);
	KdPrint(("Leave HandleRemoveDevice\n"));
	return status;
}

/****************************************************************
* 函数名称：HelloWDMDispatchRoutine
* 功能描述：对IRP进行处理
* 参数列表：
fdo：功能设备对象
pIrp:从I/0请求包
* 返回值：返回状态
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

	// 完成IRP
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	KdPrint(("Leave HelloWDMDispatchRoutine\n"));

	return status;
}

/****************************************************************
* 函数名称：HelloWDMUnload
* 功能描述：负责驱动程序的卸载操作。会卸载整个设备链，删除设备对象和符号链接
* 参数列表：
pDriverObject：从I/O管理器中传进来的驱动对象，由执行体组件-对象管理器创建和管理
* 返回值：返回状态
****************************************************************/
#pragma PAGEDCODE
VOID HelloWDMUnload(IN PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);
	PAGED_CODE();
	KdPrint(("Enter HelloWDMUnload\n"));
	KdPrint(("Leave HelloWDMUnload\n"));
}