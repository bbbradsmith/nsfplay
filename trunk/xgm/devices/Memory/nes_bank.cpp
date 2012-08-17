#include <stdio.h>
#include <stdlib.h>
#include "nes_bank.h"

// this workaround solves a problem with mirrored FDS RAM writes
// when the same bank is used twice; some NSF rips reuse bank 00
// in "unused" banks that actually get written to.
//
// eventually want to fix the NSFs and undo this solution
#define FDS_MEMCPY 1

#if FDS_MEMCPY
static UINT8* fds_image = NULL;
#endif

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

    #if FDS_MEMCPY
    if (fds_image)
      delete[] fds_image;
    #endif
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
      bankdefault[i] = -1; // -1 is special empty bank

    bankmax = (((offset & 0xfff) + size) / 0x1000) + 1;
    if (bankmax > 256)
      return false;

    if (image)
      delete[]image;
    image = new UINT8[0x1000 * bankmax];
    memset (image, 0, 0x1000 * bankmax);
    memcpy (image + (offset & 0xfff), data, size);

    #if FDS_MEMCPY
      if (fds_image)
        delete[] fds_image;
      fds_image = new UINT8[0x10000];
      memset(fds_image, 0, 0x10000);
      for (i = 0; i < 16; i++)
        bank[i] = fds_image + 0x1000 * i;
    #else
    for (i = 0; i < bankmax; i++)
      bank[i] = image + 0x1000 * i;
    for (i = bankmax; i < 256; i++)
      bank[i] = null_bank;
    #endif

    return true;
  }

  void NES_BANK::Reset ()
  {
    memset (null_bank, 0, 0x1000);
    for (int i = 0; i < 16; i++)
    {
      bankswitch[i] = bankdefault[i];

      #if FDS_MEMCPY
        bankswitch[i] = i;
        if (bankdefault[i] == -1 || bankdefault[i] >= bankmax)
          memset(bank[i], 0, 0x1000);
        else
          memcpy(bank[i], image + (bankdefault[i] * 0x1000), 0x1000);
      #endif
    }
  }

  bool NES_BANK::Write (UINT32 adr, UINT32 val, UINT32 id)
  {
    #if FDS_MEMCPY
    if (!fds_enable)
    #endif

    if (0x5ff8 <= adr && adr < 0x6000)
    {
      bankswitch[(adr & 7) + 8] = val & 0xff;
      return true;
    }

    if (fds_enable)
    {
      #if FDS_MEMCPY
      if (0x5ff6 <= adr && adr < 0x6000)
      {
        int b = adr - 0x5ff0;
        if (int(val) >= bankmax)
          memset(bank[b], 0, 0x1000);
        else
          memcpy(bank[b], image + (val * 0x1000), 0x1000);
        return true;
      }
      #else
      if (0x5ff6 <= adr && adr < 0x5ff8)
      {
        bankswitch[adr & 7] = val & 0xff;
        return true;
      }
      #endif

      if (0 <= bankswitch[adr >> 12] && 0x6000 <= adr && adr < 0xe000)
      {
        // for detecting FDS ROMs with improper mirrored writes
        //
        //for (int i=0; i < 14; ++i)
        //{
        //  int b = adr >> 12;
        //  if (i != b && bankswitch[i] == bankswitch[b])
        //  {
        //    DEBUG_OUT("[%04X] write mirrored to [%04X] = %02X\n",
        //      adr, (i * 0x1000) | (adr & 0x0fff), val);
        //  }
        // }

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
