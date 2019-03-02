#include "nsf2_irq.h"
#include "../CPU/nes_cpu.h"

namespace xgm {

NSF2_IRQ::NSF2_IRQ()
{
	Reset();
}

NSF2_IRQ::~NSF2_IRQ()
{
}

void NSF2_IRQ::Reset()
{
	active = false;
	irq = false;
	reload = 0xFFFF;
	count = reload;
}

bool NSF2_IRQ::Read(UINT32 adr, UINT32 & val, UINT32 id)
{
	if (0x401B > adr || adr > 0x401D) return false;
	switch (adr)
	{
	case 0x401B:
		val = reload & 0xFF;
		break;
	case 0x401C:
		val = (reload >> 8) & 0xFF;
		break;
	case 0x401D:
		val = irq ? 0x80 : 0x00; // return IRQ state
		irq = false; // acknowledge IRQ
		cpu->UpdateIRQ(NES_CPU::IRQD_NSF2, false);
		break;
	}
	return true;
}

bool NSF2_IRQ::Write(UINT32 adr, UINT32 val, UINT32 id)
{
	if (0x401B > adr || adr > 0x401D) return false;
	switch (adr)
	{
	case 0x401B:
		reload = (reload & 0xFF00) | (val & 0x00FF);
		break;
	case 0x401C:
		reload = (reload & 0x00FF) | ((val & 0x00FF) << 8);
		break;
	case 0x401D:
		active = (val & 0x01) != 0;
		count = reload;
		irq = false;
		cpu->UpdateIRQ(NES_CPU::IRQD_NSF2, false);
		break;
	}
	return true;
}

void NSF2_IRQ::Clock(UINT32 clocks)
{
	if (!active)
	{
		count = reload;
		return;
	}

	if (clocks > count)
	{
		irq = true;
		cpu->UpdateIRQ(NES_CPU::IRQD_NSF2, true);
		while (clocks > 0)
		{
			if (clocks > count)
			{
				clocks -= count;
				count = reload;
			}
			else
			{
				count -= clocks;
				clocks = 0;
			}
		}
	}
	else
	{
		count -= clocks;
	}
}

void NSF2_IRQ::SetCPU(NES_CPU* cpu_)
{
	cpu = cpu_;
}

} // namespace xgm
