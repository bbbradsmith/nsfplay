#ifndef _NES_MMC5_H_
#define _NES_MMC5_H_
#include "../device.h"
#include "counter.h"
#include "../CPU/nes_cpu.h"

namespace xgm
{
  class NES_MMC5:public ISoundChip
  {
  public:
    enum
    { OPT_NONLINEAR_MIXER=0, OPT_PHASE_REFRESH, OPT_END };

  protected:
    int option[OPT_END];
    int mask;
    INT32 sm[2][3]; // BS stereo panning
    UINT8 ram[0x6000 - 0x5c00];
    UINT8 reg[8];
    UINT8 mreg[2];
    UINT8 pcm; // BS PCM channel
    bool pcm_mode; // BS PCM channel
    NES_CPU* cpu; // BS PCM channel reads need CPU access

    Counter pcounter[2];        // 位相カウンタ
    UINT32 duty[2];
    UINT32 volume[2];
    UINT32 freq[2];
    INT32 out[3];
    bool enable[2];

    bool envelope_disable[2];   // エンベロープ有効フラグ
    bool envelope_loop[2];      // エンベロープループ
    bool envelope_write[2];
    int envelope_div_period[2];
    int envelope_div[2];
    int envelope_counter[2];

    int length_counter[2];

    double clock, rate;
    INT32 calc_sqr (int i);
    INT32 square_table[32];
    TrackInfoBasic trkinfo[3];
  public:
      NES_MMC5 ();
     ~NES_MMC5 ();

    virtual void Reset ();
    virtual UINT32 Render (INT32 b[2]);
    virtual bool Write (UINT32 adr, UINT32 val, UINT32 id=0);
    virtual bool Read (UINT32 adr, UINT32 & val, UINT32 id=0);
    virtual void FrameSequence (int s);
    virtual void SetOption (int id, int b);
    virtual void SetClock (double);
    virtual void SetRate (double);
    virtual void SetMask (int m){ mask = m; }
    void SetStereoMix (int trk, xgm::INT16 mixl, xgm::INT16 mixr);
    ITrackInfo *GetTrackInfo(int trk);

    void SetCPU(NES_CPU* cpu_);
  };

}

#endif
