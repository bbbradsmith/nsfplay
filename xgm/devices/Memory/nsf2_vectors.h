#ifndef _NSF2_VECTORS_H_
#define _NSF2_VECTORS_H_
#include "../device.h"

namespace xgm
{
	class NES_CPU; // forward declaration

	// vector table overlay for NSF2
	class NSF2_Vectors : public IDevice
	{
	protected:
		UINT8 vectors[6];
		NES_CPU* cpu;

	public:
		NSF2_Vectors();
		~NSF2_Vectors();
		void Reset ();
		bool Read (UINT32 adr, UINT32 & val, UINT32 id=0); // overlays all 3 vectors
		bool Write (UINT32 adr, UINT32 val, UINT32 id=0); // can write IRQ vector only
		void ForceVector(int vector, UINT32 adr); // write directly to the 3 vectors (0=NMI,1=Reset,2=IRQ)
		void SetCPU(NES_CPU* cpu_);
	};
}

#endif
