#ifndef _BLOCK_H_
#define _BLOCK_H_
#include "../device.h"

namespace xgm
{
  // Layerに挟んで，このデバイス以下のデバイスアクセスを完全にブロックするデバイス
  class BlockingDevice : public IDevice
  {
  public:
    void Reset(){}
    bool Write(UINT32 adr, UINT32 val, UINT32 id = 0)
    {
      return blocking;
    }
    bool Read(UINT32 adr, UINT32 &val, UINT32 id = 0)
    {
      return false;
    }
  };
}// namespace

#endif _BLOCK_H_
