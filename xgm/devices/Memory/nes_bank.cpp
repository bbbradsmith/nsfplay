#include <stdio.h>
#include <stdlib.h>
#include "nes_bank.h"

namespace xgm
{

  NES_BANK::NES_BANK ()
  {
    image = NULL;
    fds_enable = false;
  };

  NES_BANK::~NES_BANK ()
  {
    if (image)
      delete[]image;
  }

  void NES_BANK::SetBankDefault (UINT8 bank, int value)
  {
    bankdefault[bank] = value;
  }

  bool NES_BANK::SetImage (UINT8 * data, UINT32 offset, UINT32 size)
  {
    int i;

    // バンクスイッチの初期値は全て「バンク無効」
    for (i = 0; i < 16; i++)
      bankdefault[i] = -1;

    bankmax = (((offset & 0xfff) + size) / 0x1000) + 1;
    if (bankmax > 256)
      return false;

    if (image)
      delete[]image;
    image = new UINT8[0x1000 * bankmax];
    memset (image, 0, 0x1000 * bankmax);
    memcpy (image + (offset & 0xfff), data, size);

    for (i = 0; i < bankmax; i++)
      bank[i] = image + 0x1000 * i;
    for (i = bankmax; i < 256; i++)
      bank[i] = null_bank;

    return true;
  }

  void NES_BANK::Reset ()
  {
    memset (null_bank, 0, 0x1000);
    for (int i = 0; i < 16; i++)
      bankswitch[i] = bankdefault[i];
  }

  bool NES_BANK::Write (UINT32 adr, UINT32 val, UINT32 id)
  {
    if (0x5ff8 <= adr && adr < 0x6000)
    {
      bankswitch[(adr & 7) + 8] = val & 0xff;
      return true;
    }

    if (fds_enable)
    {
      if (0x5ff6 <= adr && adr < 0x5ff8)
      {
        bankswitch[adr & 7] = val & 0xff;
        return true;
      }

      if (0 <= bankswitch[adr >> 12] && 0x6000 <= adr && adr < 0xe000)
      {
        bank[bankswitch[adr >> 12]][adr & 0x0fff] = val;
        return true;
      }
    }

    return false;
  }

  bool NES_BANK::Read (UINT32 adr, UINT32 & val, UINT32 id)
  {
    if (0x5ff8 <= adr && adr < 0x5fff)
    {
      val = bankswitch[(adr & 7) + 8];
      return true;
    }

    if (0 <= bankswitch[adr >> 12] && 0x8000 <= adr && adr < 0x10000)
    {
      val = bank[bankswitch[adr >> 12]][adr & 0xfff];
      return true;
    }

    if (fds_enable)
    {
      if (0x5ff6 <= adr && adr < 0x5ff8)
      {
        val = bankswitch[adr & 7];
        return true;
      }

      if (0 <= bankswitch[adr >> 12] && 0x6000 <= adr && adr < 0x8000)
      {
        val = bank[bankswitch[adr >> 12]][adr & 0xfff];
        return true;
      }
    }

    return false;
  }

  void NES_BANK::SetFDSMode (bool t)
  {
    fds_enable = t;
  }

}                               // namespace
