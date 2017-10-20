#include <ntddk.h>
#include <ntstrsafe.h>
#include <ntddkbd.h>

#pragma warning(disable: 4054)

extern POBJECT_TYPE*		IoDriverObjectType;

#define KBD_DRIVER_NAME		L"\\Driver\\Kbdclass"
#define USBKBD_DRIVER_NAME	L"\\Driver\\kbdhid"
#define PS2KBD_DRIVER_NAME	L"\\Driver\\i8042prt"

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

// 要搜索是回调函数
typedef VOID(_stdcall *KEYBOARDCLASSSERVICECALLBACK)(
	IN PDEVICE_OBJECT DeviceObject,
	IN PKEYBOARD_INPUT_DATA InputDataStart,
	IN PKEYBOARD_INPUT_DATA InputDataEnd, 
	IN OUT PULONG InputDataConsumed);

// 自定义结构保存回调
typedef struct _KBD_CALLBACK
{
	PDEVICE_OBJECT classDeviceObject;
	KEYBOARDCLASSSERVICECALLBACK serviceCallBack;
}KBD_CALLBACK, *PKBD_CALLBACK;

KBD_CALLBACK gKbdCallBack = { 0 };

void MyKeyboardClassServiceCallBack(
	IN PDEVICE_OBJECT DeviceObject,
	IN PKEYBOARD_INPUT_DATA InputDataStart,
	IN PKEYBOARD_INPUT_DATA InputDataEnd,
	IN OUT PULONG InputDataConsumed)
{
	DbgPrint("Enter MyKeyboardClassServiceCallBack");

	for (PKEYBOARD_INPUT_DATA keyData = InputDataStart; keyData <= InputDataEnd; keyData++)
	{
		DbgPrint("hook kbdport: %2x, keyaction: %s",
			keyData->MakeCode, keyData->Flags ? "up" : "down");
	}

	gKbdCallBack.serviceCallBack(DeviceObject, InputDataStart, InputDataEnd, InputDataConsumed);
}

PDRIVER_OBJECT OpenKbdDriver(PCWSTR driverName)
{
	// 找到kbdclass类驱动对象地址
	UNICODE_STRING uniNtNameString;

	PDRIVER_OBJECT kbdDriverObject = NULL;

	// 初始化kdbClass驱动的名字为一个字符串
	// 打开驱动对象

	RtlInitUnicodeString(&uniNtNameString, driverName);

	DbgPrint("%wZ\n", uniNtNameString);

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
		DbgPrint("OpenKbdDriver ObReferenceObjectByName error. %d\n", status);
		return NULL;
	}
	else
	{
		// 会导致驱动对象引用计数+1，需要解引用
		ObReferenceObject(kbdDriverObject);
	}

	return kbdDriverObject;
}

NTSTATUS SearchServiceCallBack(IN PDRIVER_OBJECT DriverObject)
{
	DbgPrint("Enter SearchServiceCallBack.\n");

	UNREFERENCED_PARAMETER(DriverObject);

	NTSTATUS status = STATUS_UNSUCCESSFUL;

	// 分别打开USB键盘端口驱动和PS2键盘驱动，看看哪个能打开，说明当前正在使用的驱动对象就是谁
	PDRIVER_OBJECT KbdhidDriverObject = OpenKbdDriver(USBKBD_DRIVER_NAME);
	PDRIVER_OBJECT Kbdi8042DriverObject = OpenKbdDriver(PS2KBD_DRIVER_NAME);

	// 两种键盘同时存在，这种极端情况不考虑
	if (KbdhidDriverObject && Kbdi8042DriverObject)
	{
		DbgPrint("SearchServiceCallBack two kbd exist.\n");
		return STATUS_UNSUCCESSFUL;
	}

	// 两种键盘同时不存在，直接返回失败
	if (!KbdhidDriverObject && !Kbdi8042DriverObject)
	{
		DbgPrint("SearchServiceCallBack two kbd not exist.\n");
		return STATUS_UNSUCCESSFUL;
	}

	PDRIVER_OBJECT UsingKbdDriverObject = KbdhidDriverObject ? KbdhidDriverObject : Kbdi8042DriverObject;
	PDEVICE_OBJECT UsingDeviceObject = UsingKbdDriverObject->DeviceObject;

	// 上述UsingDeviceExt设备扩展里，应该有一个函数指针，是存在于KbdClass中的
	// KbdClass的一个设备对象的指针，也保存在UsingDeviceExt设备扩展里
	PDRIVER_OBJECT KbdClassObject = OpenKbdDriver(KBD_DRIVER_NAME);
	if (!KbdClassObject)
	{
		DbgPrint("SearchServiceCallBack KbdClass not exist.\n");
		return STATUS_UNSUCCESSFUL;
	}

	PVOID KbdClassDriverStart = KbdClassObject->DriverStart;
	ULONG KbdClassDriverSize = KbdClassObject->DriverSize;

	PVOID* AddrServiceCallBack = NULL;

	while (UsingDeviceObject)
	{
		PVOID UsingDeviceExt = UsingDeviceObject->DeviceExtension;

		// 遍历KbdClass的设备对象，确认是否在UsingDeviceExt中存在
		PDEVICE_OBJECT pTargetDeviceObject = KbdClassObject->DeviceObject;
		while (pTargetDeviceObject)
		{
			PUCHAR pDeviceExt = (PUCHAR)UsingDeviceExt;

			for (size_t i = 0; i < 4096; i++, pDeviceExt += sizeof(PUCHAR))
			{
				if (!MmIsAddressValid(pDeviceExt))
				{
					break;
				}

				if (gKbdCallBack.classDeviceObject && gKbdCallBack.serviceCallBack)
				{
					status = STATUS_SUCCESS;
					break;
				}

				PVOID tmp = *(PVOID*)pDeviceExt;
				if (tmp == pTargetDeviceObject)
				{
					// 找到这里，说明在端口驱动的设备扩展里，保存了一个Kbdclass类驱动的设备对象
					gKbdCallBack.classDeviceObject = (PDEVICE_OBJECT)tmp;
					DbgPrint("finded classDeviceObject %8x\n", tmp);
					continue;
				}

				if ((tmp > KbdClassDriverStart) &&
					(tmp < (PVOID)((PUCHAR)KbdClassDriverStart + KbdClassDriverSize)) &&
					MmIsAddressValid(tmp))
				{
					gKbdCallBack.serviceCallBack = (KEYBOARDCLASSSERVICECALLBACK)tmp;
					AddrServiceCallBack = (PVOID*)pDeviceExt;
					DbgPrint("SearchServiceCallBack servicecallback : %8x\naddrServiceCallBack : %8x\n", tmp, AddrServiceCallBack);
				}
			}

			pTargetDeviceObject = pTargetDeviceObject->NextDevice;
		}

		UsingDeviceObject = UsingDeviceObject->NextDevice;
	}

	// 成功找到了回调地址，就替换成我们自己的
	if (AddrServiceCallBack && gKbdCallBack.serviceCallBack)
	{
		DbgPrint("SearchServiceCallBack Hook KeyboardClassServiceCallBack\n");
		//*AddrServiceCallBack = MyKeyboardClassServiceCallBack;
	}
	else
	{
		DbgPrint("SearchServiceCallBack error search.\n");
	}

	return status;
}

VOID c2pUnload(PDRIVER_OBJECT driver)
{
	UNREFERENCED_PARAMETER(driver);

	DbgPrint("Leave c2pUnload.\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
	UNREFERENCED_PARAMETER(reg_path);

	DbgPrint("Enter kbddriver hook class.\n");

	driver->DriverUnload = c2pUnload;

	SearchServiceCallBack(driver);

	return STATUS_SUCCESS;
}