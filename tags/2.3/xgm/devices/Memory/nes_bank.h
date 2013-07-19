#ifndef _NES_BANK_H_
#define _NES_BANK_H_
#include "../device.h"
namespace xgm
{

  /**
   * 4KB*16バンクのバンク空間
   **/
  class NES_BANK : public IDevice
  {
  protected:
    UINT8 null_bank[0x1000];
    UINT8 *bank[256];
    UINT8 *image;
    int bankswitch[16];
    int bankdefault[16];
    bool fds_enable;
    int bankmax;

  public:
      NES_BANK ();
     ~NES_BANK ();
    void Reset ();
    bool Read (UINT32 adr, UINT32 & val, UINT32 id=0);
    bool Write (UINT32 adr, UINT32 val, UINT32 id=0);
    void SetBankDefault (UINT8 bank, int value);
    bool SetImage (UINT8 * data, UINT32 offset, UINT32 size);
    void SetFDSMode (bool);
  };

}







#endif
