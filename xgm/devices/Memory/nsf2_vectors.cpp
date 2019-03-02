#include <assert.h>
#include "nsf2_vectors.h"

namespace xgm {

NSF2_Vectors::NSF2_Vectors()
{
}

NSF2_Vectors::~NSF2_Vectors()
{
}

void NSF2_Vectors::Reset()
{
}

bool NSF2_Vectors::Read(UINT32 adr, UINT32& val, UINT32 id)
{
	if (adr < 0xFFFA) return false;
	if (adr > 0xFFFF) return false;
	val = vectors[adr-0xFFFA];
	return true;
}

bool NSF2_Vectors::Write(UINT32 adr, UINT32 val, UINT32 id)
{
	if (adr < 0xFFFE) return false;
	if (adr > 0xFFFF) return false;
	vectors[adr-0xFFFA] = val;
	return true;
}

void NSF2_Vectors::ForceVector(int vector, UINT32 adr)
{
	if (vector < 0 || vector > 2) return;
	vectors[(vector*2)+0] = adr & 0xFF;
	vectors[(vector*2)+1] = (adr >> 8) & 0xFF;
}

}                               // namespace
