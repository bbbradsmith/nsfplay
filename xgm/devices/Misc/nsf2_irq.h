#ifndef _NSF2_IRQ_H_
#define _NSF2_IRQ_H_
#include "../device.h"

namespace xgm
{

class NES_CPU; // forward declaration

class NSF2_IRQ : public IDevice
{
protected:
	UINT16 reload;
	UINT32 count;
	bool active;
	bool irq;
	NES_CPU* cpu;
public:
	NSF2_IRQ();
	~NSF2_IRQ();
	void Reset();
	bool Read(UINT32 adr, UINT32 & val, UINT32 id=0);
	bool Write(UINT32 adr, UINT32 val, UINT32 id=0);

	void Clock(UINT32 clocks);
	void SetCPU(NES_CPU* cpu_);
};

} // namespace xgm
#endif