#include <ntddk.h>
#include <ntstrsafe.h>

#define CCP_MAX_COM_ID	32
#define DELAY_ONE_MICROSECOND (-10)
#define DELAY_ONE_MILLISECOND (DELAY_ONE_MICROSECOND * 1000)
#define DELAY_ONE_SECOND (DELAY_ONE_MILLISECOND * 1000)

static PDEVICE_OBJECT s_fltobj[CCP_MAX_COM_ID] = { 0 };
static PDEVICE_OBJECT s_nextobj[CCP_MAX_COM_ID] = { 0 };

NTSTATUS ccpAttachDevice(PDRIVER_OBJECT driver, PDEVICE_OBJECT oldobj, PDEVICE_OBJECT *fltobj, PDEVICE_OBJECT *next)
{
	DbgPrint("Enter comcap ccpAttachDevice.\n");

	NTSTATUS status = IoCreateDevice(driver, 0, NULL, oldobj->DeviceType, 0, FALSE, fltobj);
	if (status != STATUS_SUCCESS)
	{
		return status;
	}

	if (oldobj->Flags & DO_BUFFERED_IO)
	{
		(*fltobj)->Flags |= DO_BUFFERED_IO;
	}

	if (oldobj->Flags & DO_DIRECT_IO)
	{
		(*fltobj)->Flags |= DO_DIRECT_IO;
	}

	if (oldobj->Characteristics & FILE_DEVICE_SECURE_OPEN)
	{
		(*fltobj)->Characteristics |= FILE_DEVICE_SECURE_OPEN;
	}

	(*fltobj)->Characteristics |= DO_POWER_PAGABLE;

	PDEVICE_OBJECT topdev = IoAttachDeviceToDeviceStack(*fltobj, oldobj);
	if (topdev == NULL)
	{
		// 绑定失败
		IoDeleteDevice(*fltobj);
		*fltobj = NULL;
		status = STATUS_UNSUCCESSFUL;
		return status;
	}

	*next = topdev;
	(*fltobj)->Flags = (*fltobj)->Flags & ~DO_DEVICE_INITIALIZING;

	return STATUS_SUCCESS;
}

PDEVICE_OBJECT ccpOpenCom(ULONG id, NTSTATUS* status)
{
	DbgPrint("Enter comcap ccpOpenCom.\n");

	UNICODE_STRING name_str;
	static WCHAR name[32] = {0};
	PFILE_OBJECT fileobj = NULL;
	PDEVICE_OBJECT devobj = NULL;

	memset(name, 0, sizeof(name));

	RtlStringCchPrintfW(name, 32, L"\\Device\\Serial%d", id);
	RtlInitUnicodeString(&name_str, name);

	*status = IoGetDeviceObjectPointer(&name_str, FILE_ALL_ACCESS, &fileobj, &devobj);

	if (*status == STATUS_SUCCESS)
	{
		ObDereferenceObject(fileobj);
	}

	return devobj;
}

void ccpAttachAllComs(PDRIVER_OBJECT driver)
{
	DbgPrint("Enter comcap ccpAttachAllComs.\n");

	NTSTATUS status;
	for (size_t i = 0; i < CCP_MAX_COM_ID; i++)
	{
		PDEVICE_OBJECT com_ob = ccpOpenCom(i, &status);
		if (com_ob == NULL)
		{
			continue;
		}

		ccpAttachDevice(driver, com_ob, &s_fltobj[i], &s_nextobj[i]);
	}
}

NTSTATUS ccpDispatch(PDEVICE_OBJECT device, PIRP irp)
{
	DbgPrint("Enter comcap ccpDispatch.\n");

	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(irp);

	for (size_t i = 0; i < CCP_MAX_COM_ID; i++)
	{
		if (s_fltobj[i] == device)
		{
			// 电源操作，都放过
			if (irpsp->MajorFunction == IRP_MJ_POWER)
			{
				PoStartNextPowerIrp(irp);
				IoSkipCurrentIrpStackLocation(irp);
				return PoCallDriver(s_nextobj[i], irp);
			}

			// 过滤写请求
			if (irpsp->MajorFunction == IRP_MJ_WRITE)
			{
				ULONG len = irpsp->Parameters.Write.Length;

				// 获取缓冲区
				PUCHAR buf = NULL;
				if (irp->MdlAddress != NULL)
				{
					buf = (PUCHAR)MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);
				}
				else
				{
					buf = (PUCHAR)irp->UserBuffer;
				}

				if (buf == NULL)
				{
					buf = (PUCHAR)irp->AssociatedIrp.SystemBuffer;
				}

				for (size_t j = 0; j < len; j++)
				{
					DbgPrint("Comcap: Send Data: %2x ", buf[j]);
				}
			}

			IoSkipCurrentIrpStackLocation(irp);
			return IoCallDriver(s_nextobj[i], irp);
		}
	}

	return STATUS_SUCCESS;
}

void ccpUnload(PDRIVER_OBJECT drv)
{
	UNREFERENCED_PARAMETER(drv);

	// 先一个个解除绑定
	for (size_t i = 0; i < CCP_MAX_COM_ID; i++)
	{
		if (s_nextobj[i] != NULL)
		{
			IoDetachDevice(s_nextobj[i]);
		}
	}

	// 睡眠5s，等待所有IRP结束
	LARGE_INTEGER interval;
	interval.QuadPart = 5 * 1000 * DELAY_ONE_MILLISECOND;
	KeDelayExecutionThread(KernelMode, FALSE, &interval);

	// 删除这些设备
	for (size_t i = 0; i < CCP_MAX_COM_ID; i++)
	{
		if (s_fltobj[i] != NULL)
		{
			IoDeleteDevice(s_fltobj[i]);
		}
	}
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
	UNREFERENCED_PARAMETER(reg_path);

	DbgPrint("Enter comcap.\n");

	for (size_t i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		driver->MajorFunction[i] = ccpDispatch;
	}

	driver->DriverUnload = ccpUnload;

	ccpAttachAllComs(driver);

	return STATUS_SUCCESS;
}