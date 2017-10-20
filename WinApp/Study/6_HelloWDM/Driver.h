#pragma once

// ��Ϊ�������C++��д����ntddk��ʵ�֣���C��д��
// ����Ϊ�˷�ֹntddk�ĺ������ű������
#ifdef __cplusplus
extern "C"
{
#endif
#include <wdm.h>
#ifdef __cplusplus
}
#endif

#define PAGEDCODE code_seq("PAGE")		// �����ҳ���
#define LOCKEDCODE code_seq()			// ����Ƿ�ҳ���
#define INITCODE code_seq("INIT")		// �����ʼ���ڴ�顣
// ����ֻ���ڼ��ص�ʱ�������ڴ棬�������ɹ����غ󣬾Ϳ��Դ��ڴ���ж�ص�
#define PAGEDATA data_seq("PAGE")
#define LOCKEDDATA data_seq()
#define INITDATA data_seq("INIT")

#define arraysize(p) (sizeof(p) / sizeof((p)[0]))

// �Զ����豸��չ
typedef struct _DEVICE_EXTENSION {
	PDEVICE_OBJECT fdo;
	PDEVICE_OBJECT NextStackDevice;
	UNICODE_STRING ustrDeviceName;		// �豸�����������豸����������ʾ������
	UNICODE_STRING ustrSymLinkName;		// ����������������ring3������豸�����
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

// ��������
NTSTATUS HelloWDMAddDevice(IN PDRIVER_OBJECT pDriverObject,
	IN PDEVICE_OBJECT pPhysicalDeviceObject);
NTSTATUS HelloWDMPnp(IN PDEVICE_OBJECT fdo, IN PIRP pIrp);
NTSTATUS DefaultPnpHandler(IN PDEVICE_EXTENSION pdx, IN PIRP pIrp);
NTSTATUS HandleRemoveDevice(IN PDEVICE_EXTENSION pdx, IN PIRP pIrp);

VOID HelloWDMUnload(IN PDRIVER_OBJECT pDriverObject);
NTSTATUS HelloWDMDispatchRoutine(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);