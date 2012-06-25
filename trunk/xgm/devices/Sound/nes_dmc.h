#ifndef _NES_DMC_H_
#define _NES_DMC_H_

#include "../device.h"
#include "counter.h"
#include "../audio/MedianFilter.h"

namespace xgm
{
  /** Bottom Half of APU **/
  class NES_DMC:public ISoundChip
  {
  public:
    enum
    { 
      OPT_ENABLE_4011=0,
      OPT_ENABLE_PNOISE,
      OPT_UNMUTE_ON_RESET,
      OPT_DPCM_ANTI_NOISE,
      OPT_NONLINEAR_MIXER,
      OPT_RANDOMIZE_NOISE,
      OPT_END 
    };
  protected:
    enum { BEFORE_NOISE, PRESENT_NOISE, AFTER_NOISE };
    const int GETA_BITS;
    static const UINT32 freq_table[2][16];
    static const UINT32 wavlen_table[2][16];
    UINT32 tnd_table[2][16][16][128];

    int option[OPT_END];
    int mask;
    INT32 sm[2][3]; // BS stereo mix
    UINT8 reg[0x10];
    UINT32 len_reg;
    UINT32 adr_reg;
    IDevice *memory;
    UINT32 out[3];
    UINT32 daddress;
    UINT32 length;
    UINT32 data;
    INT16 damp;
    int dac_lsb;
    UINT32 clock;
    UINT32 rate;
    int pal;
    int mode;
    bool irq;
    bool active;
    int anti_noise_mode;
    INT16 dcoff;

    Counter pcounter[3]; // 位相カウンタ

    UINT32 tri_freq;
    int linear_counter;
    int linear_counter_reload;
    bool linear_counter_halt;
    bool linear_counter_control;

    int noise_volume;
    UINT32 noise, noise_tap;
    UINT32 now, syn_interval, out_interval; // ノイズチャンネルのサンプリングレート変換

    // noise envelope
    bool envelope_loop;
    bool envelope_disable;
    bool envelope_write;
    int envelope_div_period;
    int envelope_div;
    int envelope_counter;

    bool enable[3];
    int length_counter[2]; // 0=tri, 1=noise

    TrackInfoBasic trkinfo[3];

    inline UINT32 calc_tri ();
    inline UINT32 calc_dmc ();
    inline UINT32 calc_noise ();

  public:
      NES_DMC ();
     ~NES_DMC ();
    void Reset ();
    void InitializeTNDTable(double wt, double wn, double wd);
    void SetRate (double rate);
    void SetPal (bool is_pal);
    UINT32 Render (INT32 b[2]);
    void SetMemory (IDevice * r);
    bool Write (UINT32 adr, UINT32 val, UINT32 id=0);
    bool Read (UINT32 adr, UINT32 & val, UINT32 id=0);
    void FrameSequence(int s);
    void SetClock (double rate);
    void SetOption (int, int);
    int GetDamp(){ return (damp<<1)|dac_lsb ; }
    void SetMask(int m){ mask = m; }
    void SetStereoMix (int trk, xgm::INT16 mixl, xgm::INT16 mixr);
    ITrackInfo *GetTrackInfo(int trk);
  };

}

#endif
