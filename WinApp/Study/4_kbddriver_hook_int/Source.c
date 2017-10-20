#include <ntddk.h>
#include <ntstrsafe.h>
#include <ntddkbd.h>

#pragma warning(disable: 4054)

#define DELAY_ONE_MICROSECOND (-10)
#define DELAY_ONE_MILLISECOND (DELAY_ONE_MICROSECOND * 1000)
#define DELAY_ONE_SECOND (DELAY_ONE_MILLISECOND * 1000)

typedef unsigned char P2C_U8;
typedef unsigned short P2C_U16;
typedef unsigned long P2C_U32;

// 从sidt指令获得如下一个结构
#pragma pack(push, 1)
typedef struct _P2C_IDTR {
	P2C_U16 limit;
	P2C_U32	base;
}P2C_IDTR, *PP2C_IDTR;
#pragma pack(pop)

void *p2cGetIdt()
{
	P2C_IDTR idtr;
	_asm sidt idtr
	return (void*)idtr.base;
}

// IDT表单元
// 跳转地址保存在offset_low和offset_high中
#pragma pack(push, 1)
typedef struct _P2C_IDT_ENTRY {
	P2C_U16 offset_low;
	P2C_U16 selector;
	P2C_U8 reserved;
	P2C_U8 type : 4;
	P2C_U8 always0 : 1;
	P2C_U8 dpl : 2;
	P2C_U8 present : 1;
	P2C_U16 offset_high;
}P2C_IDTENTRY, *PP2C_IDTENTRY;
#pragma pack(pop)

#define P2C_MAKELONG(low, high)		( ((P2C_U32)(low) & 0xffff) | (((P2C_U32)(high) & 0xffff) << 16) )
#define P2C_LOW16_OF_32(data)		( (P2C_U16)((P2C_U32)(data) & 0xffff) )
#define P2C_HIGH16_OF_32(data)		( (P2C_U16)((P2C_U32)(data) >> 16) )

void* g_p2c_old = NULL;

#define OBUFFER_FULL	0x02
#define IBUFFER_FULL	0x01

ULONG p2cWaitForKdRead()
{
	int i = 100;
	P2C_U8 mychar;
	do
	{
		_asm in al,0x64
		_asm mov mychar,al
		KeStallExecutionProcessor(50);
		if (!(mychar & OBUFFER_FULL))
		{
			break;
		}
	} while (i--);

	if (i) return TRUE;
	return FALSE;
}

ULONG p2cWaitForKdWrite()
{
	int i = 100;
	P2C_U8 mychar;
	do
	{
		_asm in al, 0x64
		_asm mov mychar, al
		KeStallExecutionProcessor(50);
		if (!(mychar & IBUFFER_FULL))
		{
			break;
		}
	} while (i--);

	if (i) return TRUE;
	return FALSE;
}

void p2cUserFilter()
{
	DbgPrint("Enter p2cUserFilter\r\n");
	static P2C_U8 sch_pre = 0;
	P2C_U8 sch;
	p2cWaitForKdRead();
	_asm in al,0x60
	_asm mov sch,al

	// 端口的数据一旦被读出来，就没有了，这个时候，需要强制回写
	if (sch_pre != sch)
	{
		DbgPrint("p2c: scan code = %2x\r\n", sch);

		sch_pre = sch;
		_asm mov al, 0xd2;
		_asm out 0x64,al
		p2cWaitForKdWrite();
		_asm mov al, sch
		_asm out 0x60,al
	}
}

__declspec(naked) p2cInterruptProc()
{
	__asm
	{
		pushad			// 保存所有的通用寄存器
		pushfd			// 保存标志寄存器
		call p2cUserFilter		// 调用我们自己的一个函数，这个函数将实现我们自己的一个功能
		popfd
		popad
		jmp g_p2c_old	// 跳转到原来的中断服务程序
	}
}

void p2cHookInt93(BOOLEAN hook_or_unhook)
{
	PP2C_IDTENTRY idt_addr = (PP2C_IDTENTRY)p2cGetIdt();
	idt_addr += 0x93;		// 第0x93个中断索引
	DbgPrint("p2c: the pre address = %x.\r\n", (void*)P2C_MAKELONG(idt_addr->offset_low, idt_addr->offset_high));

	if (hook_or_unhook)
	{
		DbgPrint("p2c: try to hook interrupt.\r\n");
		g_p2c_old = (void*)P2C_MAKELONG(idt_addr->offset_low, idt_addr->offset_high);
		idt_addr->offset_low = P2C_LOW16_OF_32(p2cInterruptProc);
		idt_addr->offset_high = P2C_HIGH16_OF_32(p2cInterruptProc);
	}
	else
	{
		DbgPrint("p2c: try to recovery interrupt.\r\n");
		idt_addr->offset_low = P2C_LOW16_OF_32(g_p2c_old);
		idt_addr->offset_high = P2C_HIGH16_OF_32(g_p2c_old);
		g_p2c_old = NULL;
	}

	DbgPrint("p2c: the current address = %x.\r\n", (void*)P2C_MAKELONG(idt_addr->offset_low, idt_addr->offset_high));
}

VOID c2pUnload(PDRIVER_OBJECT driver)
{
	UNREFERENCED_PARAMETER(driver);

	p2cHookInt93(FALSE);

	LARGE_INTEGER interval;
	interval.QuadPart = 5 * 1000 * DELAY_ONE_MILLISECOND;
	KeDelayExecutionThread(KernelMode, FALSE, &interval);

	DbgPrint("Leave c2pUnload.\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
	UNREFERENCED_PARAMETER(reg_path);

	DbgPrint("Enter kbddriver hook class.\n");

	driver->DriverUnload = c2pUnload;

	p2cHookInt93(TRUE);

	return STATUS_SUCCESS;
}