#include "nes_n106.h"

namespace xgm
{
#define GETA_BITS 24
  NES_N106::NES_N106 ()
  {
    SetClock (DEFAULT_CLOCK);
    SetRate (DEFAULT_RATE);

    for(int c=0;c<2;++c)
        for(int t=0;t<8;++t)
            sm[c][t] = 128;
  }

  NES_N106::~NES_N106 ()
  {
  }

  void NES_N106::SetStereoMix(int trk, xgm::INT16 mixl, xgm::INT16 mixr)
  {
      if (trk < 0) return;
      if (trk > 7) return;
      sm[0][trk] = mixl;
      sm[1][trk] = mixr;
  }

  ITrackInfo *NES_N106::GetTrackInfo(int trk)
  {
    if(trk<this->chnum)
    {
      trk = 7-trk;
      trkinfo[trk].max_volume = 15;
      trkinfo[trk].volume = volume[trk];
      trkinfo[trk].key = volume[trk]>0;
      trkinfo[trk]._freq = freq[trk];
      trkinfo[trk].freq = freq[trk]?clock/45/length[trk]/chnum*freq[trk]/(0x40000):0.0;
      trkinfo[trk].tone = offset[trk];
      trkinfo[trk].output = out[trk];
      trkinfo[trk].wavelen = length[trk];
      for(UINT32 i=0;i<length[trk];i++)
        trkinfo[trk].wave[i] = wave[(offset[trk]+i)&0xff];
      return &trkinfo[trk];
    }
    else
      return NULL;
  }

  inline INT32 NES_N106::calc (int i)
  {
    return wave[(offset[i]+((pcounter[i]>>18)%length[i]))&0xff] * volume[i];
  }

  void NES_N106::Tick (int clocks)
  {
  }

  UINT32 NES_N106::Render (INT32 b[2])
  {
    int i, c;

    b[0] = 0;
    b[1] = 0;
    for (i = 0; i < chnum; ++i)
    {
        INT32 m = 0;
        if (0 == ((mask>>i)&1))
        {
            m = calc(7-i);
            //m = (i&1) ? m : -m;

            b[0] += m * sm[0][i];
            b[1] += m * sm[1][i];
        }
    }
    //b[0] >>= (7 - 3);
    //b[1] >>= (7 - 3);
    const INT32 CHAN_ADJUST[9] = { 8, 8, 8, 8, 8, 7, 6, 5, 4 }; // this is just a guess
    b[0] = (b[0] * CHAN_ADJUST[chnum]) >> (7 - 1);
    b[1] = (b[1] * CHAN_ADJUST[chnum]) >> (7 - 1);

    wait_counter += wait_incr;
    c = wait_counter>>GETA_BITS;
    wait_counter &= (1<<GETA_BITS)-1;

    while(c--)
    {
      chidx = (chidx + 1)%chnum;
      pcounter[7-chidx] += freq[7-chidx];
    }

    return 2;
  }

  void NES_N106::writeReg (UINT32 adr, UINT32 val)
  {
    int ch;

    if (adr < 0x80)
    {
      wave[adr * 2] = (val & 0xf) - 8;
      wave[adr * 2 + 1] = ((val >> 4) & 0xf) - 8;
      if (adr<0x40) return;
    }

    ch = (adr>>3)&7;

    switch (adr & 7)
    {
    case 0:
      freq[ch] = (freq[ch] & 0x3FF00) | val;
      break;

    case 2:
      freq[ch] = (freq[ch] & 0x300FF) | (val << 8);
      break;

    case 4:
      freq[ch] = (freq[ch] & 0x0FFFF) | ((val & 3) << 16);
      //length[ch] = 0x20 - ( val&0x1c );
      length[ch] = 0x100 - ( val&0xFC ); // BS all 6 bits can be used for length
      fflag[ch] = (val>>4)&1;
      break;

    case 6:
      offset[ch] = val;
      break;

    case 7:
      volume[ch] = (val & 15);
      if (adr == 0x7F)
      {
        chnum = ((val >> 4) & 7) + 1;
        chidx %= chnum;
        wait_incr = (UINT32)( clock / (rate * 45) * (1 << GETA_BITS) );
      }
      break;

    default:
      break;
    }
    reg[adr] = val;
  }

  void NES_N106::Reset ()
  {
    int i;
    for (i = 0; i < 0x100; i++)
      writeReg (i, 0);
    mask = 0;
    writeReg(0x7f,0x70);
    for (i = 0; i < 8; i++) 
      pcounter[i] = 0;
    wait_counter = 0;
  }

  void NES_N106::SetClock (double c)
  {
    clock = c;
  }

  void NES_N106::SetRate (double r)
  {
    if(chnum<=0) chnum = 8;
    rate = r ? r : DEFAULT_RATE;
    wait_incr = (UINT32)( clock / (rate * 45) * (1 << GETA_BITS) );
    chidx = 0;
  }

  bool NES_N106::Read (UINT32 adr, UINT32 & val, UINT32 id)
  {

    switch (adr)
    {
    case 0x4800:
      val = reg[addr & 0x7f];
      if (addr & 0x80) addr++;
      break;
    case 0xF800:
      //val = addr; // this register is not readable
      return false;
    default:
      return false;
    }
    return true;
    
  }

  bool NES_N106::Write (UINT32 adr, UINT32 val, UINT32 id)
  {
    switch (adr)
    {
    case 0x4800:
      writeReg (addr & 0x7f, val);
      if (addr & 0x80)
        addr++;
      break;
    case 0xF800:
      addr = val;
      break;
    default:
      return false;
    }
    return true;
  }

}                               //namespace
