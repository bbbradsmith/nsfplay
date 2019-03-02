#ifndef _NES_RAM_H_
#define _NES_RAM_H_
#include "../device.h"

namespace xgm
{
  const UINT32 PLAYER_RESERVED      = 0x4100;
  const UINT32 PLAYER_RESERVED_SIZE = 0x0020;

  class NES_MEM : public IDevice
  {
  protected:
    UINT8 image[0x10000];
    bool fds_enable;

  public:
      NES_MEM ();
     ~NES_MEM ();
    void Reset ();
    bool Read (UINT32 adr, UINT32 & val, UINT32 id=0);
    bool Write (UINT32 adr, UINT32 val, UINT32 id=0);
    bool SetImage (UINT8 * data, UINT32 offset, UINT32 size);
    void SetFDSMode (bool); // enables writing to $6000-DFFF for FDS
    void SetReserved (const UINT8* data, UINT32 size);
  };

}                               // namespace

#endif
