#include "nes_vrc6.h"

namespace xgm
{

  NES_VRC6::NES_VRC6 ()
  {
    SetClock (DEFAULT_CLOCK);
    SetRate (DEFAULT_RATE);

    halt = false;
    freq_shift = 0;

    for(int c=0;c<2;++c)
        for(int t=0;t<3;++t)
            sm[c][t] = 128;
  }

  NES_VRC6::~NES_VRC6 ()
  {
  }

  void NES_VRC6::SetStereoMix(int trk, xgm::INT16 mixl, xgm::INT16 mixr)
  {
      if (trk < 0) return;
      if (trk > 2) return;
      sm[0][trk] = mixl;
      sm[1][trk] = mixr;
  }

  ITrackInfo *NES_VRC6::GetTrackInfo(int trk)
  {
    if(trk<2)
    {
      trkinfo[trk].max_volume = 15;
      trkinfo[trk].volume = volume[trk];
      trkinfo[trk]._freq = freq[trk];
      trkinfo[trk].freq = freq[trk]?clock/16/freq[trk]:0;
      trkinfo[trk].tone = duty[trk];
      trkinfo[trk].key = (volume[trk]>0)&&enable[trk]&&!gate[trk];
      return &trkinfo[trk];
    }
    else if(trk==2)
    {
      trkinfo[2].max_volume = 255;
      trkinfo[2].volume = accum;
      trkinfo[2]._freq = freq[2];
      trkinfo[2].freq = freq[2]?clock/14/freq[2]:0;
      trkinfo[2].tone = -1;
      trkinfo[2].key = (enable[2]>0);
      return &trkinfo[2];
    }
    else
      return NULL;
  }

  void NES_VRC6::SetClock (double c)
  {
    clock = c;
  }

  void NES_VRC6::SetRate (double r)
  {
    rate = r ? r : DEFAULT_RATE;
    for (int i = 0; i < 3; i++)
      pcounter[i].init (clock, rate, i < 2 ? 16 : 2);
  }

  void NES_VRC6::Reset ()
  {
    Write (0x9003, 0);
    for (int i = 0; i < 3; i++)
    {
      Write (0x9000 + i, 0);
      Write (0xa000 + i, 0);
      Write (0xb000 + i, 0);
    }
    ct7 = 0;
    mask = 0;
  }

  INT16 NES_VRC6::calc_sqr (int i)
  {
    static const INT16 sqrtbl[8][16] = {
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
      {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    if (!enable[i])
      return 0;

    if (!halt)
      pcounter[i].iup ();

    return (gate[i]
      || sqrtbl[duty[i]][pcounter[i].value ()])? volume[i] : 0;
  }

  INT16 NES_VRC6::calc_saw ()
  {
    if (!enable[2])
      return 0;

    if (!halt && pcounter[2].iup ())
    {
      if ((++ct7) == 7)
        ct7 = saw_phase = 0;
      else
        saw_phase += accum; // note 8-bit wrapping behaviour
    }

    // only top 5 bits of saw are output
    return saw_phase >> 3;
  }

  void NES_VRC6::Tick (int clocks)
  {
  }

  UINT32 NES_VRC6::Render (INT32 b[2])
  {
    INT32 m[3];
    m[0] = calc_sqr(0);
    m[1] = calc_sqr(1);
    m[2] = calc_saw();

    // note: signal is inverted compared to 2A03

    m[0] = (mask & 1) ? 0 : -m[0];
    m[1] = (mask & 2) ? 0 : -m[1];
    m[2] = (mask & 4) ? 0 : -m[2];

    b[0]  = m[0] * sm[0][0];
    b[0] += m[1] * sm[0][1];
    b[0] += m[2] * sm[0][2];
    //b[0] >>= (7 - 7);

    b[1]  = m[0] * sm[1][0];
    b[1] += m[1] * sm[1][1];
    b[1] += m[2] * sm[1][2];
    //b[1] >>= (7 - 7);

    return 2;
  }

  bool NES_VRC6::Write (UINT32 adr, UINT32 val, UINT32 id)
  {
    int ch, cmap[4] = { 0, 0, 1, 2 };

    switch (adr)
    {
    case 0x9000:
    case 0xa000:
      ch = cmap[(adr >> 12) & 3];
      volume[ch] = val & 15;
      duty[ch] = (val >> 4) & 7;
      gate[ch] = (val >> 7) & 1;
      break;

    case 0x9001:
    case 0xa001:
    case 0xb001:
      ch = cmap[(adr >> 12) & 3];
      freq[ch] = (freq[ch] & 0xf00) | val;
      pcounter[ch].setcycle (freq[ch] >> freq_shift);
      break;

    case 0x9002:
    case 0xa002:
    case 0xb002:
      ch = cmap[(adr >> 12) & 3];
      freq[ch] = ((val & 0xf) << 8) + (freq[ch] & 0xff);
      pcounter[ch].setcycle (freq[ch] >> freq_shift);
      if (!enable[ch]) // if enable is being turned on, phase should be reset
      {
        if (adr == 0xb002)
        {
          ct7 = 0; // reset
          saw_phase = 0;
        }
        else
        {
          pcounter[ch].reset(0);
          // TODO
          // really I need to reset the duty cycle without
          // resetting the divider; the pcounter implementation
          // implements both together, making this impossible.
        }
      }
      enable[ch] = (val >> 7) & 1;
      break;

    case 0xb000:
      accum = val & 63;
      break;

    case 0x9003:
      halt = val & 1;
      freq_shift =
          (val & 4) ? 8 :
          (val & 2) ? 4 :
          0;
      pcounter[0].setcycle(freq[0] >> freq_shift);
      pcounter[1].setcycle(freq[1] >> freq_shift);
      pcounter[2].setcycle(freq[2] >> freq_shift);
      break;

    default:
      return false;

    }

    return true;
  }

  bool NES_VRC6::Read (UINT32 adr, UINT32 & val, UINT32 id)
  {
    return false;
  }


}                               // namespace
