#include "nes_dmc.h"
#include "nes_apu.h"

namespace xgm
{
  const UINT32 NES_DMC::wavlen_table[2][16] = {
  { // NTSC
    4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068
  },
  { // PAL
    4, 8, 14, 30, 60, 88, 118, 148, 188, 236, 354, 472, 708,  944, 1890, 3778
  }};

  const UINT32 NES_DMC::freq_table[2][16] = {
  { // NTSC
    428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106, 84, 72, 54
  },
  { // PAL
    398, 354, 316, 298, 276, 236, 210, 198, 176, 148, 132, 118,  98, 78, 66, 50
  }};

  NES_DMC::NES_DMC () : GETA_BITS (20)
  {
    SetClock (DEFAULT_CLOCK);
    SetRate (DEFAULT_RATE);
    SetPal (false);
    option[OPT_ENABLE_4011] = 1;
    option[OPT_ENABLE_PNOISE] = 1;
    option[OPT_UNMUTE_ON_RESET] = 1;
    option[OPT_DPCM_ANTI_CLICK] = 0;
    option[OPT_NONLINEAR_MIXER] = 1;
    option[OPT_RANDOMIZE_NOISE] = 1;
    tnd_table[0][0][0][0] = 0;
    tnd_table[1][0][0][0] = 0;

    apu = NULL;
    frame_sequence_count = 0;
    frame_sequence_length = 7458;
    frame_sequence_steps = 4;

    for(int c=0;c<2;++c)
        for(int t=0;t<3;++t)
            sm[c][t] = 128;
  }


  NES_DMC::~NES_DMC ()
  {
  }

  void NES_DMC::SetStereoMix(int trk, xgm::INT16 mixl, xgm::INT16 mixr)
  {
      if (trk < 0) return;
      if (trk > 2) return;
      sm[0][trk] = mixl;
      sm[1][trk] = mixr;
  }

  ITrackInfo *NES_DMC::GetTrackInfo(int trk)
  {
    switch(trk)
    {
    case 0:
      trkinfo[trk].max_volume = 255;
      trkinfo[0].key = (linear_counter>0 && length_counter[0]>0 && enable[0]);
      trkinfo[0].volume = 0;
      trkinfo[0]._freq = tri_freq;
      if(trkinfo[0]._freq)
        trkinfo[0].freq = clock/32/(trkinfo[0]._freq + 1);
      else
        trkinfo[0].freq = 0;
      trkinfo[0].tone = -1;
      trkinfo[0].output = out[0];
      break;
    case 1:
      trkinfo[1].max_volume = 15;
      trkinfo[1].volume = noise_volume+(envelope_disable?0:0x10);
      trkinfo[1].key = length_counter[1]>0 && enable[1] &&
                       (envelope_disable ? (noise_volume>0) : (envelope_counter>0));
      trkinfo[1]._freq = reg[0x400e-0x4008]&0xF;
      trkinfo[1].freq = clock/double(wavlen_table[pal][trkinfo[1]._freq] * ((noise_tap&(1<<6)) ? 93 : 1));
      trkinfo[1].tone = noise_tap & (1<<6);
      trkinfo[1].output = out[1];
      break;
    case 2:
      trkinfo[2].max_volume = 127;
      trkinfo[2].volume = reg[0x4011-0x4008]&0x7F;
      trkinfo[2].key = active;
      trkinfo[2]._freq = reg[0x4010-0x4008]&0xF;
      trkinfo[2].freq = clock/double(freq_table[pal][trkinfo[2]._freq]);
      trkinfo[2].tone = (0xc000|(adr_reg<<6));
      trkinfo[2].output = (damp<<1)|dac_lsb;
      break;
    default:
      return NULL;
    }
    return &trkinfo[trk];
  }

  void NES_DMC::FrameSequence(int s)
  {
    //DEBUG_OUT("FrameSequence: %d\n",s);

    if (s > 3) return; // no operation in step 4

    if (apu)
    {
        apu->FrameSequence(s);
    }

    if (s == 0 && (frame_sequence_steps == 4))
    {
        frame_irq = true;
    }

    // 240hz clock
    {
        // triangle linear counter
        if (linear_counter_halt)
        {
            linear_counter = linear_counter_reload;
        }
        else
        {
            if (linear_counter > 0) --linear_counter;
        }
        if (!linear_counter_control)
        {
            linear_counter_halt = false;
        }

        // noise envelope
        bool divider = false;
        if (envelope_write)
        {
            envelope_write = false;
            envelope_counter = 15;
            envelope_div = 0;
        }
        else
        {
            ++envelope_div;
            if (envelope_div > envelope_div_period)
            {
                divider = true;
                envelope_div = 0;
            }
        }
        if (divider)
        {
            if (envelope_loop && envelope_counter == 0)
                envelope_counter = 15;
            else if (envelope_counter > 0)
                --envelope_counter;
        }
    }

    // 120hz clock
    if ((s&1) == 0)
    {
        // triangle length counter
        if (!linear_counter_control && (length_counter[0] > 0))
            --length_counter[0];

        // noise length counter
        if (!envelope_loop && (length_counter[1] > 0))
            --length_counter[1];
    }

  }

  // 三角波チャンネルの計算 戻り値は0-15
  UINT32 NES_DMC::calc_tri (UINT32 clocks)
  {
    static UINT32 tritbl[32] = 
    {
      0, 1, 2, 3, 4, 5, 6, 7,
      8, 9,10,11,12,13,14,15,
     15,14,13,12,11,10, 9, 8,
      7, 6, 5, 4, 3, 2, 1, 0
    };

    if (linear_counter > 0 && length_counter[0] > 0)
    {
      counter[0] += clocks;
      while (counter[0] > tri_freq)
      {
        tphase = (tphase + 1) & 31;
        counter[0] -= (tri_freq + 1);
      }
    }

    UINT32 ret = tritbl[tphase];
    return ret;
  }

  // ノイズチャンネルの計算 戻り値は0-127
  // 低サンプリングレートで合成するとエイリアスノイズが激しいので
  // ノイズだけはこの関数内で高クロック合成し、簡易なサンプリングレート
  // 変換を行っている。
  UINT32 NES_DMC::calc_noise(UINT32 clocks)
  {
    UINT32 env = envelope_disable ? noise_volume : envelope_counter;
    if (length_counter[1] < 1) env = 0;

    UINT32 last = (noise & 0x4000) ? env : 0;
    if (clocks < 1) return last;

    // simple anti-aliasing (noise requires it, even when oversampling is off)
    UINT32 count = 0;
    UINT32 accum = 0;

    counter[1] += clocks;
    assert (nfreq > 0); // prevent infinite loop
    while (counter[1] >= nfreq)
    {
        // tick the noise generator
        UINT32 feedback = (noise&1) ^ ((noise&noise_tap)?1:0);
        noise = (noise>>1) | (feedback<<14);

        ++count;
        accum += last;
        last = (noise & 0x4000) ? env : 0;

        counter[1] -= nfreq;
    }

    if (count < 1) // no change over interval, don't anti-alias
    {
       return last;
    }

    UINT32 clocks_accum = clocks - counter[1];
    // count = number of samples in accum
    // counter[1] = number of clocks since last sample

    accum = (accum * clocks_accum) + (last * counter[1] * count);
    // note accum as an average is already premultiplied by count

    return accum / (clocks * count);
  }

  // DMCチャンネルの計算 戻り値は0-127
  UINT32 NES_DMC::calc_dmc (UINT32 clocks)
  {
    counter[2] += clocks;
    assert (dfreq > 0); // prevent infinite loop
    while (counter[2] >= dfreq)
    {
      if ( data != 0x100 ) // data = 0x100 は EMPTY を意味する。
      {
        if ((data & 1) && (damp < 63))
          damp++;
        else if (!(data & 1) && (0 < damp))
          damp--;
        data >>=1;
      }

      if ( data == 0x100 && active )
      { 
        memory->Read (daddress, data);
        data |= (data&0xFF)|0x10000; // 8bitシフトで 0x100 になる
        if ( length > 0 ) 
        {
          daddress = ((daddress+1)&0xFFFF)|0x8000 ;
          length --;
        }
      }
      
      if ( length == 0 ) // 最後のフェッチが終了したら(再生完了より前に)即座に終端処理
      {
        if (mode & 1)
        {
          daddress = ((adr_reg<<6)|0xC000);
          length = (len_reg<<4)+1;
        }
        else
        {
          irq = (mode==2&&active)?1:0; // 直前がactiveだったときはIRQ発行
          active = false;
        }
      }

      counter[2] -= dfreq;
    }

    return (damp<<1) + dac_lsb;
  }

  void NES_DMC::TickFrameSequence (UINT32 clocks)
  {
      frame_sequence_count += clocks;
      while (frame_sequence_count > frame_sequence_length)
      {
          FrameSequence(frame_sequence_step);

          frame_sequence_count -= frame_sequence_length;
          ++frame_sequence_step;
          if(frame_sequence_step >= frame_sequence_steps)
              frame_sequence_step = 0;
      }
  }

  void NES_DMC::Tick (UINT32 clocks)
  {
    out[0] = calc_tri(clocks);
    out[1] = calc_noise(clocks);
    out[2] = calc_dmc(clocks);
  }

  UINT32 NES_DMC::Render (INT32 b[2])
  {
    out[0] = (mask & 1) ? 0 : out[0];
    out[1] = (mask & 2) ? 0 : out[1];
    out[2] = (mask & 4) ? 0 : out[2];

    INT32 m[3];
    m[0] = tnd_table[0][out[0]][0][0];
    m[1] = tnd_table[0][0][out[1]][0];
    m[2] = tnd_table[0][0][0][out[2]];

    if (option[OPT_NONLINEAR_MIXER])
    {
        INT32 ref = m[0] + m[1] + m[2];
        INT32 voltage = tnd_table[1][out[0]][out[1]][out[2]];
        if (ref)
        {
            for (int i=0; i < 3; ++i)
                m[i] = (m[i] * voltage) / ref;
        }
        else
        {
            for (int i=0; i < 3; ++i)
                m[i] = voltage;
        }
    }

    // anti-click nullifies any 4011 write but preserves nonlinearity
    if (option[OPT_DPCM_ANTI_CLICK])
    {
        if (dmc_pop) // $4011 will cause pop this frame
        {
            // adjust offset to counteract pop
            dmc_pop_offset += dmc_pop_follow - m[2];
            dmc_pop = false;

            // prevent overflow, keep headspace at edges
            const INT32 OFFSET_MAX = (1 << 30) - (4 << 16);
            if (dmc_pop_offset >  OFFSET_MAX) dmc_pop_offset =  OFFSET_MAX;
            if (dmc_pop_offset < -OFFSET_MAX) dmc_pop_offset = -OFFSET_MAX;
        }
        dmc_pop_follow = m[2]; // remember previous position

        m[2] += dmc_pop_offset; // apply offset
    }

    b[0]  = m[0] * sm[0][0];
    b[0] += m[1] * sm[0][1];
    b[0] += m[2] * sm[0][2];
    b[0] >>= 7;

    b[1]  = m[0] * sm[1][0];
    b[1] += m[1] * sm[1][1];
    b[1] += m[2] * sm[1][2];
    b[1] >>= 7;

    return 2;
  }

  void NES_DMC::SetClock (double c)
  {
    clock = (UINT32)(c);
  }

  void NES_DMC::SetRate (double r)
  {
    rate = (UINT32)(r?r:DEFAULT_RATE);
  }

  void NES_DMC::SetPal (bool is_pal)
  {
      pal = (is_pal ? 1 : 0);
      // set CPU cycles in frame_sequence
      frame_sequence_length = is_pal ? 8314 : 7458;
  }

  void NES_DMC::SetAPU (NES_APU* apu_)
  {
      apu = apu_;
  }

  // Initializing TRI, NOISE, DPCM mixing table
  void NES_DMC::InitializeTNDTable(double wt, double wn, double wd) {

    { // Linear Mixer
      for(int t=0; t<16 ; t++) {
        for(int n=0; n<16; n++) {
          for(int d=0; d<128; d++) {
              tnd_table[0][t][n][d] = (UINT32)(8192.0*(3.0*t+2.0*n+d)/208.0);
          }
        }
      }
    }
    { // Non-Linear Mixer
      tnd_table[1][0][0][0] = 0;
      for(int t=0; t<16 ; t++) {
        for(int n=0; n<16; n++) {
          for(int d=0; d<128; d++) {
            if(t!=0||n!=0||d!=0)
              tnd_table[1][t][n][d] = (UINT32)((8192.0*159.79)/(100.0+1.0/((double)t/wt+(double)n/wn+(double)d/wd)));
          }
        }
      }
    }

  }

  void NES_DMC::Reset ()
  {
    int i;
    mask = 0;

    InitializeTNDTable(8227,12241,22638);

    counter[0] = 0;
    counter[1] = 0;
    counter[2] = 0;
    tphase = 0;
    nfreq = wavlen_table[0][0];
    dfreq = freq_table[0][0];

    envelope_div = 0;
    length_counter[0] = 0;
    length_counter[1] = 0;
    envelope_counter = 0;

    frame_irq = false;
    frame_irq_enable = false;
    frame_sequence_count = 0;
    frame_sequence_steps = 4;
    frame_sequence_step = 0;

    for (i = 0; i < 0x10; i++)
      Write (0x4008 + i, 0);

    irq = false;
    Write (0x4015, 0x00);
    if (option[OPT_UNMUTE_ON_RESET])
      Write (0x4015, 0x0f);

    out[0] = out[1] = out[2] = 0;
    tri_freq = 0;
    damp = 0;
    dmc_pop = false;
    dmc_pop_offset = 0;
    dmc_pop_follow = 0;
    dac_lsb = 0;
    data = 0x100;
    adr_reg = 0;
    active = false;
    length = 0;
    len_reg = 0;
    daddress = 0;
    noise = 1;
    noise_tap = (1<<1);
    if (option[OPT_RANDOMIZE_NOISE])
    {
        noise |= ::rand();
    }

    SetRate(rate);
  }

  void NES_DMC::SetMemory (IDevice * r)
  {
    memory = r;
  }

  void NES_DMC::SetOption (int id, int val)
  {
    if(id<OPT_END)
    {
      option[id] = val;
      if(id==OPT_NONLINEAR_MIXER)
        InitializeTNDTable(8227,12241,22638);
    }
  }

  bool NES_DMC::Write (UINT32 adr, UINT32 val, UINT32 id)
  {
    static const UINT8 length_table[32] = {
        0x0A, 0xFE,
        0x14, 0x02,
        0x28, 0x04,
        0x50, 0x06,
        0xA0, 0x08,
        0x3C, 0x0A,
        0x0E, 0x0C,
        0x1A, 0x0E,
        0x0C, 0x10,
        0x18, 0x12,
        0x30, 0x14,
        0x60, 0x16,
        0xC0, 0x18,
        0x48, 0x1A,
        0x10, 0x1C,
        0x20, 0x1E
    };

    if (adr == 0x4015)
    {
      enable[0] = (val & 4) ? true : false;
      enable[1] = (val & 8) ? true : false;

      if (!enable[0])
      {
          length_counter[0] = 0;
      }
      if (!enable[1])
      {
          length_counter[1] = 0;
      }

      if ((val & 16)&&!active)
      {
        enable[2] = active = true;
        daddress = (0xC000 | (adr_reg << 6));
        length = (len_reg << 4) + 1;
        irq = 0;
      }
      else if (!(val & 16))
      {
        enable[2] = active = false;
      }

      reg[adr-0x4008] = val;
      return true;
    }

    if (adr == 0x4017)
    {
      //DEBUG_OUT("4017 = %02X\n", val);

      frame_irq_enable = ((val & 0x40) == 0x40);
      frame_sequence_count = 0;
      frame_sequence_steps = ((val & 0x80) == 0x80) ? 5 : 4;
      frame_sequence_step = (frame_sequence_steps == 5) ? 0 : 1;
      FrameSequence(frame_sequence_step);
      ++frame_sequence_step;
    }

    if (adr<0x4008||0x4013<adr)
      return false;

    reg[adr-0x4008] = val&0xff;

    //DEBUG_OUT("$%04X %02X\n", adr, val);

    switch (adr)
    {

    // tri

    case 0x4008:
      linear_counter_control = (val >> 7) & 1;
      linear_counter_reload = val & 0x7F;
      break;

    case 0x4009:
      break;

    case 0x400a:
      tri_freq = val | (tri_freq & 0x700) ;
      if (counter[0] > tri_freq) counter[0] = tri_freq;
      break;

    case 0x400b:
      tri_freq = (tri_freq & 0xff) | ((val & 0x7) << 8) ;
      if (counter[0] > tri_freq) counter[0] = tri_freq;
      linear_counter_halt = true;
      if (enable[0])
      {
        length_counter[0] = length_table[(val >> 3) & 0x1f];
      }
      break;

    // noise

    case 0x400c:
      noise_volume = val & 15;
      envelope_div_period = val & 15;
      envelope_disable = (val >> 4) & 1;
      envelope_loop = (val >> 5) & 1;
      break;

    case 0x400d:
      break;

    case 0x400e:
      if (option[OPT_ENABLE_PNOISE])
        noise_tap = (val & 0x80) ? (1<<6) : (1<<1);
      else
        noise_tap = (1<<1);
      nfreq = wavlen_table[pal][val&15];
      if (counter[1] > nfreq) counter[1] = nfreq;
      break;

    case 0x400f:
      if (enable[1])
      {
        length_counter[1] = length_table[(val >> 3) & 0x1f];
      }
      envelope_write = true;
      break;

    // dmc

    case 0x4010:
      mode = (val >> 6) & 3;
      dfreq = freq_table[pal][val&15];
      if (counter[2] > dfreq) counter[2] = dfreq;
      break;

    case 0x4011:
      if (option[OPT_ENABLE_4011])
      {
        damp = (val >> 1) & 0x3f;
        dac_lsb = val & 1;
        dmc_pop = true;
      }
      break;

    case 0x4012:
      adr_reg = val&0xff;
      // ここでdaddressは更新されない
      break;

    case 0x4013:
      len_reg = val&0xff;
      // ここでlengthは更新されない
      break;

    default:
      return false;
    }

    return true;
  }

  bool NES_DMC::Read (UINT32 adr, UINT32 & val, UINT32 id)
  {
    if (adr == 0x4015)
    {
      val |= (irq?128:0)
          | (frame_irq ? 0x40 : 0)
          | (active?16:0)
          | (length_counter[1]?8:0)
          | (length_counter[0]?4:0)
          ;

      frame_irq = false;
      return true;
    }
    else if (0x4008<=adr&&adr<=0x4014)
    {
      val |= reg[adr-0x4008];
      return true;
    }
    else
      return false;
  }

}                               // namespace
