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

// ��sidtָ��������һ���ṹ
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

// IDT��Ԫ
// ��ת��ַ������offset_low��offset_high��
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
		_asm in al, 0x64
		_asm mov mychar, al
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
	_asm in al, 0x60
	_asm mov sch, al

	// �˿ڵ�����һ��������������û���ˣ����ʱ����Ҫǿ�ƻ�д
	if (sch_pre != sch)
	{
		DbgPrint("p2c: scan code = %2x\r\n", sch);

		sch_pre = sch;
		_asm mov al, 0xd2;
		_asm out 0x64, al
		p2cWaitForKdWrite();
		_asm mov al, sch
		_asm out 0x60, al
	}
}

__declspec(naked) p2cInterruptProc()
{
	__asm
	{
		pushad			// �������е�ͨ�üĴ���
		pushfd			// �����־�Ĵ���
		call p2cUserFilter		// ���������Լ���һ�����������������ʵ�������Լ���һ������
		popfd
		popad
		jmp g_p2c_old	// ��ת��ԭ�����жϷ������
	}
}

// �޸�IOAPIC�ض����
P2C_U8 p2cSearchOrSetIrql(P2C_U8 new_ch)
{
	DbgPrint("Enter p2cSearchOrSetIrql");

	// ����һ�������ַ0xfec00000������IOAPIC�Ĵ�������Windows�ϵĿ�ʼ��ַ
	PHYSICAL_ADDRESS phys;
	RtlZeroMemory(&phys, sizeof(PHYSICAL_ADDRESS));
	phys.u.LowPart = 0xfec00000;

	// �����ַ�ǲ���ֱ�Ӷ�д����Ҫӳ��Ϊ�����ַ
	PVOID paddr = MmMapIoSpace(phys, 0x14, MmNonCached);
	if (!MmIsAddressValid(paddr))
	{
		return 0;
	}

	// ѡ��Ĵ������ǼĴ�����ƫ��Ϊ0�ļĴ�����ѡ��Ĵ�����Ȼ��32λ�ģ�����ֻ����8λƫ��
	P2C_U8 *io_reg_sel = (P2C_U8 *)paddr;

	// ���ڼĴ���ƫ��Ϊ0x10
	P2C_U32 *io_win = (P2C_U32*)((P2C_U8 *)paddr + 0x10);

	// ѡ��0x12���Ӧ��IRQ1�������ж�
	*io_reg_sel = 0x12;
	P2C_U32 ch = *io_win;
	P2C_U32 ch1;

	// ������ֵ�����ؾ�ֵ
	if (new_ch != 0)
	{
		ch1 = *io_win;
		ch1 &= 0xffffff00;
		ch1 |= (P2C_U32)new_ch;
		*io_win = ch1;

		DbgPrint("p2cSearchOrSetIrql: set %2x to irql.\r\n", new_ch);
	}

	ch &= 0xff;
	MmUnmapIoSpace(paddr, 0x14);

	DbgPrint("p2cSearchOrSetIrql: the old vec of irq1 is %2x to irql.\r\n", (P2C_U8)ch);

	return (P2C_U8)ch;
}

P2C_U8 p2cGetIdleIdtVec()
{
	PP2C_IDTENTRY idt_addr = (PP2C_IDTENTRY)p2cGetIdt();
	for (P2C_U8 i = 0x20; i < 0x2a; i++)
	{
		if (idt_addr[i].type == 0)
		{
			return i;
		}
	}

	return 0;
}

P2C_U8 p2cCopyANewIdt93(P2C_U8 id, void* interrupt_proc)
{
	PP2C_IDTENTRY idt_addr = (PP2C_IDTENTRY)p2cGetIdt();
	idt_addr[id] = idt_addr[0x93];
	idt_addr->offset_low = P2C_LOW16_OF_32(interrupt_proc);
	idt_addr->offset_high = P2C_HIGH16_OF_32(interrupt_proc);

	return id;
}

void p2cResetIoApic(BOOLEAN set_or_recovery)
{
	static P2C_U8 idle_id = 0;
	P2C_U8 old_id = 0;
	PP2C_IDTENTRY idt_addr = (PP2C_IDTENTRY)p2cGetIdt();
	DbgPrint("p2c: the pre address = %x.\r\n", (void*)P2C_MAKELONG(idt_addr->offset_low, idt_addr->offset_high));

	if (set_or_recovery)
	{
		DbgPrint("p2c: try to set IoApic.\r\n");

		idt_addr += 0x93;
		g_p2c_old = (void*)P2C_MAKELONG(idt_addr->offset_low, idt_addr->offset_high);
		idle_id = p2cGetIdleIdtVec();
		if (idle_id != 0)
		{
			p2cCopyANewIdt93(idle_id, p2cInterruptProc);

			// �����¶�λ������ж�
			old_id = p2cSearchOrSetIrql(idle_id);
			// Ĭ�϶�λ��0x93
			ASSERT(old_id == 0x93);
		}
	}
	else
	{
		DbgPrint("p2c: try to recovery IoApic.\r\n");

		old_id = p2cSearchOrSetIrql(0x93);
		ASSERT(old_id == idle_id);
		idt_addr[old_id].type = 0;
	}

	DbgPrint("p2c: the current address = %x.\r\n", (void*)P2C_MAKELONG(idt_addr->offset_low, idt_addr->offset_high));
}

VOID c2pUnload(PDRIVER_OBJECT driver)
{
	UNREFERENCED_PARAMETER(driver);

	p2cResetIoApic(FALSE);

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

	p2cResetIoApic(TRUE);

	return STATUS_SUCCESS;
}