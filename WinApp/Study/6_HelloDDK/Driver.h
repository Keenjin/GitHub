#pragma once

// 因为这里采用C++编写，而ntddk的实现，是C编写。
// 这里为了防止ntddk的函数符号表导入错误
#ifdef __cplusplus
extern "C"
{
#endif
#include <ntddk.h>
#ifdef __cplusplus
}
#endif

#define PAGECODE code_seq("PAGE")		// 定义分页标记
#define LOCKEDCODE code_seq()			// 定义非分页标记
#define INITCODE code_seq("INIT")		// 定义初始化内存块。
										// 函数只是在加载的时候载入内存，当驱动成功加载后，就可以从内存中卸载掉
#define PAGEDATA data_seq("PAGE")
#define LOCKEDDATA data_seq()
#define INITDATA data_seq("INIT")

#define arraysize(p) (sizeof(p) / sizeof((p)[0]))

// 自定义设备扩展
typedef struct _DEVICE_EXTENSION {
	PDEVICE_OBJECT pDevice;
	UNICODE_STRING ustrDeviceName;		// 设备名，可以在设备管理器中显示的名字
	UNICODE_STRING ustrSymLinkName;		// 符号链接名，用于ring3层访问设备对象的
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

// 函数声明
NTSTATUS CreateDevice(IN PDRIVER_OBJECT pDriverObject);
VOID HelloDDKUnload(IN PDRIVER_OBJECT pDriverObject);
NTSTATUS HelloDDKDispatchRoutine(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);