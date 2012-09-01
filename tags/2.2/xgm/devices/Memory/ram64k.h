#ifndef _RAM64K_H_
#define _RAM64K_H_
#include "../device.h"

namespace xgm
{
  class RAM64K : public IDevice
  {

  public:
    UINT8 image[0x10000];
      RAM64K ();
     ~RAM64K ();
    void Reset ();
    bool Read (UINT32 adr, UINT32 & val, UINT32 id=0);
    bool Write (UINT32 adr, UINT32 val, UINT32 id=0);
    bool SetImage (UINT8 * data, UINT32 offset, UINT32 size);
  };

} // namespace

#endif
