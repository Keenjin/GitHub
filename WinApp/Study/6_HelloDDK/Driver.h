#pragma once

// ��Ϊ�������C++��д����ntddk��ʵ�֣���C��д��
// ����Ϊ�˷�ֹntddk�ĺ������ű������
#ifdef __cplusplus
extern "C"
{
#endif
#include <ntddk.h>
#ifdef __cplusplus
}
#endif

#define PAGECODE code_seq("PAGE")		// �����ҳ���
#define LOCKEDCODE code_seq()			// ����Ƿ�ҳ���
#define INITCODE code_seq("INIT")		// �����ʼ���ڴ�顣
										// ����ֻ���ڼ��ص�ʱ�������ڴ棬�������ɹ����غ󣬾Ϳ��Դ��ڴ���ж�ص�
#define PAGEDATA data_seq("PAGE")
#define LOCKEDDATA data_seq()
#define INITDATA data_seq("INIT")

#define arraysize(p) (sizeof(p) / sizeof((p)[0]))

// �Զ����豸��չ
typedef struct _DEVICE_EXTENSION {
	PDEVICE_OBJECT pDevice;
	UNICODE_STRING ustrDeviceName;		// �豸�����������豸����������ʾ������
	UNICODE_STRING ustrSymLinkName;		// ����������������ring3������豸�����
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

// ��������
NTSTATUS CreateDevice(IN PDRIVER_OBJECT pDriverObject);
VOID HelloDDKUnload(IN PDRIVER_OBJECT pDriverObject);
NTSTATUS HelloDDKDispatchRoutine(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);