#ifndef _NES_RAM_H_
#define _NES_RAM_H_
#include "../device.h"

namespace xgm
{
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
    void SetFDSMode (bool);
  };

}                               // namespace

#endif
