#ifndef _RCONV_H_
#define _RCONV_H_
#include "../device.h"
#include "filter.h"

namespace xgm
{

class NES_CPU; // forward
class NES_DMC;
class NES_MMC5;

// RateConverter
//
// Subdivides clocks of Tick and passes to each attached IRenderable.
// Also responsible for clocking the CPU (and related devices)
// before the IRenderables.

class RateConverter : public IRenderable
{
protected:
	IRenderable * target;
	double clock ,rate;
	int mult;
	INT32  tap[2][128];
	double hr[128]; // H(z)
	INT64  hri[128];
	UINT32 clocks; // render clocks pending Tick
	NES_CPU* cpu;
	NES_DMC* dmc;
	NES_MMC5* mmc5;
	int cpu_clocks; // CPU clocks pending Tick
	int cpu_rest; // extra clock accumulator (instructions will get ahead by a few clocks)
	bool fast_skip;

	void ClockCPU(int c);

public:
	RateConverter ();
	virtual ~ RateConverter ();
	void Attach (IRenderable * t);
	void Reset ();
	void SetClock (double clock);
	void SetRate (double rate);
	virtual void Tick (UINT32 clocks_); // ticks get executed during Render
	virtual UINT32 Render (INT32 b[2]);
	virtual void Skip(); // Does ticks in lieu of Render
	inline UINT32 FastRender(INT32 b[2]);

	// call TickCPU before each Tick
	void TickCPU(int t) { cpu_clocks+=t; }

	void SetCPU(NES_CPU* c) { cpu=c; }
	void SetDMC(NES_DMC* d) { dmc=d; }
	void SetMMC5(NES_MMC5* m) { mmc5=m; }
	void SetFastSkip(bool s) { fast_skip=s; }
};

} // namespace
#endif
