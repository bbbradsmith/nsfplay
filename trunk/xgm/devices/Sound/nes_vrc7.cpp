#include "nes_vrc7.h"

namespace xgm
{
  NES_VRC7::NES_VRC7 ()
  {
    opll = OPLL_new ( 3579545, DEFAULT_RATE);
    OPLL_reset_patch (opll, OPLL_VRC7_FT_TONE);
    SetClock(DEFAULT_CLOCK);

    for(int c=0;c<2;++c)
        for(int t=0;t<6;++t)
            sm[c][t] = 128;
  }

  NES_VRC7::~NES_VRC7 ()
  {
    OPLL_delete (opll);
  }

  void NES_VRC7::SetClock (double c)
  {
    clock = c / 6 / 72;
  }

  void NES_VRC7::SetRate (double r)
  {
    rate = r ? r : DEFAULT_RATE;
    OPLL_set_quality(opll, 1); // BS quality always on (not really a CPU hog)
    OPLL_set_rate(opll,(e_uint32)rate);
  }

  void NES_VRC7::Reset ()
  {
    OPLL_reset (opll);
  }

  void NES_VRC7::SetStereoMix(int trk, xgm::INT16 mixl, xgm::INT16 mixr)
  {
      if (trk < 0) return;
      if (trk > 5) return;
      sm[0][trk] = mixl;
      sm[1][trk] = mixr;
  }

  ITrackInfo *NES_VRC7::GetTrackInfo(int trk)
  {
    if(opll&&trk<6)
    {
      trkinfo[trk].max_volume = 15;
      trkinfo[trk].volume = (opll->reg[0x30+trk])&15;
      trkinfo[trk]._freq = opll->reg[0x10+trk]+((opll->reg[0x20+trk]&1)<<8);
      int blk = (opll->reg[0x20+trk]>>1)&7;
      trkinfo[trk].freq = clock*trkinfo[trk]._freq/(double)(0x80000>>blk);
      trkinfo[trk].tone = (opll->reg[0x30+trk]>>4)&15;
      trkinfo[trk].key = (opll->reg[0x20+trk]&0x10)?true:false;
      return &trkinfo[trk];
    }
    else
      return NULL;
  }

  bool NES_VRC7::Write (UINT32 adr, UINT32 val, UINT32 id)
  {
    if (adr == 0x9010)
    {
      OPLL_writeIO (opll, 0, val);
      return true;
    }
    if (adr == 0x9030)
    {
      OPLL_writeIO (opll, 1, val);
      return true;
    }
    else
      return false;
  }

  bool NES_VRC7::Read (UINT32 adr, UINT32 & val, UINT32 id)
  {
    return false;
  }

  UINT32 NES_VRC7::Render (INT32 b[2])
  {
    b[0] = b[1] = 0;
    OPLL_calc(opll);
    for (int i=0; i < 6; ++i)
    {
        INT32 val = (mask & (1<<i)) ? 0 : opll->slot[(i<<1)|1].output[1];
        b[0] += val * sm[0][i];
        b[1] += val * sm[1][i];
    }
    b[0] >>= (7 - 4);
    b[1] >>= (7 - 4);
    return 2;
  }
}
