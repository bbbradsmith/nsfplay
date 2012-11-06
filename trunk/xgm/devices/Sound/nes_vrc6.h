#ifndef _NES_VRC6_H_
#define _NES_VRC6_H_
#include "../device.h"
#include "counter.h"

namespace xgm
{

  class NES_VRC6:public ISoundChip
  {
  protected:
    Counter pcounter[3];
    int mask;
    INT32 sm[2][3]; // BS stereo mix
    int duty[2];
    int volume[3];
    int enable[3];
    int gate[3];
    UINT32 freq[3];
    INT16 calc_sqr (int i);
    INT16 calc_saw ();
    int accum, ct7;
    UINT8 saw_phase;
    bool halt;
    int freq_shift;
    double clock, rate;
    TrackInfoBasic trkinfo[3];

  public:
      NES_VRC6 ();
     ~NES_VRC6 ();

    virtual void Reset ();
    virtual void Tick (UINT32 clocks);
    virtual UINT32 Render (INT32 b[2]);
    virtual bool Read (UINT32 adr, UINT32 & val, UINT32 id=0);
    virtual bool Write (UINT32 adr, UINT32 val, UINT32 id=0);
    virtual void SetClock (double);
    virtual void SetRate (double);
    virtual void SetMask (int m){ mask = m; }
    virtual void SetStereoMix (int trk, xgm::INT16 mixl, xgm::INT16 mixr);
    virtual ITrackInfo *GetTrackInfo(int trk);
  };

}                               // namespace

#endif
