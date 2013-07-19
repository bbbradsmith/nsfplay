#include <assert.h>
#include "nes_mem.h"

namespace xgm
{
  NES_MEM::NES_MEM ()
  {
    fds_enable = false;
  }

  NES_MEM::~NES_MEM ()
  {
  }

  void NES_MEM::Reset ()
  {
    memset (image, 0, 0x800);
    //memset (image + 0x6000, 0, 0x2000); // •ª‚©‚Á‚Ä‚Ä‚ ‚¦‚Ä‰Šú‰»‚µ‚Ä‚Ü‚¹‚ñB
  }

  bool NES_MEM::SetImage (UINT8 * data, UINT32 offset, UINT32 size)
  {
    memset (image, 0, 0x10000);
    if( offset + size < 0x10000 )
      memcpy (image + offset, data, size );
    else 
      memcpy (image + offset, data, 0x10000 - offset);
    return true;
  }

  bool NES_MEM::Write (UINT32 adr, UINT32 val, UINT32 id)
  {
    if (0x0000 <= adr && adr < 0x2000)
    {
      image[adr & 0x7ff] = val;
      return true;
    }
    if (0x6000 <= adr && adr < 0x8000)
    {
      image[adr] = val;
      return true;
    }
    if (0x4100 <= adr && adr < 0x4110)
    {
      image[adr] = val;
      return true;
    }
    if (fds_enable && 0x8000 <= adr && adr < 0xe000)
    {
      image[adr] = val;
    }
    return false;
  }

  bool NES_MEM::Read (UINT32 adr, UINT32 & val, UINT32 id)
  {
    if (0x0000 <= adr && adr < 0x2000)
    {
      val = image[adr & 0x7ff];
      return true;
    }
    if (0x4100 <= adr && adr < 0x4110)
    {
      val = image[adr];
      return true;
    }
    if (0x6000 <= adr && adr < 0x10000)
    {
      val = image[adr];
      return true;
    }
    val = 0;
    return false;
  }

  void NES_MEM::SetFDSMode (bool t)
  {
    fds_enable = t;
  }

}                               // namespace
