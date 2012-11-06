#include "nes_fds.h"

namespace xgm
{

  NES_FDS::NES_FDS ()
  {
    SetClock (DEFAULT_CLOCK);
    SetRate (DEFAULT_RATE);
    option[OPT_MOD_PHASE_REFRESH] = true;
    option[OPT_CAR_PHASE_REFRESH] = false;
    option[OPT_USE_PWM] = false;

    sm[0] = 128;
    sm[1] = 128;
  }

  NES_FDS::~NES_FDS ()
  {
  }

  void NES_FDS::SetStereoMix(int trk, xgm::INT16 mixl, xgm::INT16 mixr)
  {
      if (trk < 0) return;
      if (trk > 1) return;
      sm[0] = mixl;
      sm[1] = mixr;
  }

  ITrackInfo *NES_FDS::GetTrackInfo(int trk)
  {
    trkinfo.max_volume = 0x21;
    trkinfo.volume = envelope_output[0];
    trkinfo.key = (envelope_output[0]>0);
    trkinfo._freq = freq[0];
    trkinfo.freq = freq[0]?clock/64*freq[0]/0x10000*2:0.0;
    trkinfo.tone = 0;
    for(int i=0;i<64;i++)
      trkinfo.wave[i] = wave[0][i];
    
    return &trkinfo;
  }

  void NES_FDS::SetClock (double c)
  {
    clock = c;
  }

  void NES_FDS::SetRate (double r)
  {
    rate = r ? r : DEFAULT_RATE;
  }

  void NES_FDS::SetOption (int id, int val)
  {
    if(id<OPT_END) option[id] = val;
  }

  void NES_FDS::Reset ()
  {
    int i;

    wavidx = 0;
    geta = clock / rate;
    mask = 0;

    for (i = 0x4040; i < 0x4100; i++)
      Write (i, 0);

    Write (0x408A, 232);

    for (i = 0; i < 0x40; i++)
    {
      wave[0][i] = 0;
      wave[1][i] = 0;
    }

    for (i = 0; i < 2; i++)
    {
      pcounter[i] = 0;
      phase[i] = 0;
      opout[i] = 0;
      opval[i] = 0;
      envelope_output[i] = 0;
      ecounter[i] = 0;
    }

    ccounter.init(clock,rate,36);

  }

  void NES_FDS::update_envelope (int ch)
  {
    while (ecounter[ch] >= 1.0)
    {
      if (envelope_mode[ch] == 0)
      {
        if (envelope_amount[ch])
          envelope_amount[ch]--;
      }
      else if (envelope_mode[ch] == 1)
      {
        //if (envelope_amount[ch] < 0x21)
        if (envelope_amount[ch] < 0x20) // BS envelope should not push above $20
          envelope_amount[ch]++;
      }
      ecounter[ch] -= 1.0;
    }

    if (!envelope_disable)
      ecounter[ch] += ecounter_incr[ch];
  }

  INT32 NES_FDS::calc ()
  {
    static const int vtable[4] = { 48, 32, 24, 16 };

    static const int ctable[4] = { 36, 24, 18, 16 };

    /* Modulator */
    //opout[1] =
    //  ((opval[1] & 0x40) ? (opval[1] - 128) : opval[1]) * envelope_output[1];
    // BS setting modulator output after updating it

    //if (!envelope_disable)
    //  envelope_output[1] =
    //    envelope_amount[1] < 0x21 ? envelope_amount[1] : 0x20;
    // BS again, doing this after update, also removing cap ($4085 can set it higher than 0x20)

    while (pcounter[1] >= 1.0)
    {
      if (wave[1][phase[1]] == -1)
        opval[1] = 0;
      else
        opval[1] = (opval[1] + wave[1][phase[1]]) & 0x7f;
      phase[1] = (phase[1] + 1) & 0x3f;
      pcounter[1] -= 1.0;
    }
    pcounter[1] += incr[1];
    update_envelope (1);

    // BS setting envelope_output after updating now
    if (!envelope_disable) envelope_output[1] = envelope_amount[1];

    /* Carrier */
    opout[0] = wave[0][phase[0]];

    while (pcounter[0] >= 1.0)
    {
      phase[0] = (phase[0] + 1) & 0x3f;
      if (!phase[0])
        envelope_output[0] =
          //envelope_amount[0] < 0x21 ? envelope_amount[0] : 0x20;
          // BS should not cap this, $4080 can set it higher
          envelope_amount[0];
      pcounter[0] -= 1.0;
    }

    if (!write_enable[0])
    {
      // ’ˆÓ)fdssound.txt‚Æ‚Í3072`4095‚Ì”ÍˆÍ‚ªˆÙ‚È‚éB
      //INT32 fm =
      //  (((4096 + 1024 + opout[1]) & 4095) / 16) - 64 + ((opout[1] > 0)
      //                                                   && (opout[1] & 0xf) ?
      //                                                   2 : 0);

      // BS rewriting FDS modulation according to disch's FDS.txt
      INT32 sweep_bias = (opval[1] >= 64) ? (opval[1] - 128) : opval[1];
      opout[1] = sweep_bias * envelope_output[1];

      INT32 fm = opout[1]; // temp = sweep bias * sweep gain
      if (fm & 0x0F)
      {
          fm /= 16;
          if (sweep_bias < 0) fm -= 1;
          else                fm += 2;
      }
      else
      {
          fm /= 16;
      }
      if (fm > 193) fm -= 258;
      if (fm < -64) fm += 256;

      if (write_enable[1]) // disable modulator if $4080 bit 7 set
      {
        fm = 0;
      }

      pcounter[0] += incr[0] + (incr[0] * double(fm)) / 64.0;
    }

    update_envelope (0);

    if (option[OPT_USE_PWM])
    {
      ccounter.iup();
      if(ccounter.value() < envelope_output[0])
        return opout[0] << 6;
      else
        return 0;
    }
    else
      opout[0] *= envelope_output[0];
    
    return (opout[0] * vtable[volume & 3]) >> 4;
  }

  void NES_FDS::Tick (UINT32 clocks)
  {
  }

  UINT32 NES_FDS::Render (INT32 b[2])
  {
    INT32 m = calc();
    m = mask ? 0 : m;

    b[0] = (m * sm[0]) >> 7;
    b[1] = (m * sm[1]) >> 7;

    return 2;
  }

  bool NES_FDS::Write (UINT32 adr, UINT32 val, UINT32 id)
  {
    UINT8 btable[] = { 0, 1, 2, 4, 0xff, 128 - 4, 128 - 2, 128 - 1 };

    if (0x4040 <= adr && adr < 0x4080)
    {
      if (write_enable[0])
        wave[0][adr & 0x3f] = (val & 0x3f) - 0x20;
      return true;
    }

    switch (adr)
    {
    case 0x4080:
      envelope_mode[0] = (val >> 6) & 3;
      if (envelope_mode[0] & 2)
        envelope_amount[0] = val & 0x3f;
      else
      {
        if (envelope_speed)
          ecounter_incr[0] =
            geta / ((8 * envelope_speed) * ((val & 0x3f) + 1));
        else
          ecounter_incr[0] = 0.0;
      }
      break;

    case 0x4082:
      freq[0] = (freq[0] & 0xf00) | (val & 0xff);
      incr[0] = geta * freq[0] / 65536.0;
      break;

    case 0x4083:
      freq[0] = ((val & 0x0f) << 8) | (freq[0] & 0xff);
      incr[0] = geta * freq[0] / 65536.0;
      sound_disable = (val >> 7) & 1;
      if (sound_disable)
        pcounter[0] = 0;
      envelope_disable = (val >> 6) & 1;
      if (option[OPT_CAR_PHASE_REFRESH])
      {
        phase[0] = 0;
        pcounter[0] = 0;
      }
      break;

    case 0x4084:
      envelope_mode[1] = (val >> 6) & 3;
      if (envelope_mode[1] & 2)
        envelope_amount[1] = val & 0x3f;
      else
      {
        if (envelope_speed)
          ecounter_incr[1] =
            geta / ((8 * envelope_speed) * ((val & 0x3f) + 1));
        else
          ecounter_incr[1] = 0.0;
      }
      break;

    case 0x4085:
      opval[1] = val & 0x7f;
      if (option[OPT_MOD_PHASE_REFRESH])
      {
        phase[1] = 0;
        pcounter[1] = 0;
      }
      break;

    case 0x4086:
      freq[1] = (freq[1] & 0xf00) | (val & 0xff);
      incr[1] = geta * freq[1] / 65536.0;
      break;

    case 0x4087:
      freq[1] = ((val & 0x0f) << 8) | (freq[1] & 0xff);
      incr[1] = geta * freq[1] / 65536.0;
      write_enable[1] = (val >> 7) & 1;
      break;

    case 0x4088:
      if (write_enable[1])
      {
        wave[1][(wavidx++) & 0x3f] = btable[val & 7];
        wave[1][(wavidx++) & 0x3f] = btable[val & 7];
      }
      break;

    case 0x4089:
      write_enable[0] = (val >> 7) & 1;
      volume = (val & 3);
      break;

    case 0x408A:
      envelope_speed = val & 0xff;
      break;

    case 0x4023:
      break;

    default:
      return false;
    }

    reg[adr & 0xFF] = val;
    return true;

  }

  bool NES_FDS::Read (UINT32 adr, UINT32 & val, UINT32 id)
  {
    if (0x4040 <= adr && adr < 0x4080)
    {
      val = write_enable ? wave[0][phase[0] & 0x3f] : wave[0][adr & 0x3f];
      return true;
    }
    else if ( 0x4080 <= adr && adr < 0x408B)
    {
      val = reg[adr&0xFF];
      return true;
    }

    switch (adr)
    {
    case 0x4090:
      val = envelope_output[0] | 0x40;
      break;

    case 0x4092:
      val = envelope_output[1] | 0x40;
      break;

    default:
      return false;
    }

    return true;
  }

} // namespace
