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
    double clock, rate;
    TrackInfoBasic trkinfo[3];

  public:
      NES_VRC6 ();
     ~NES_VRC6 ();
    void Reset ();
    UINT32 Render (INT32 b[2]);
    bool Read (UINT32 adr, UINT32 & val, UINT32 id=0);
    bool Write (UINT32 adr, UINT32 val, UINT32 id=0);
    void SetClock (double);
    void SetRate (double);
    void SetMask (int m){ mask = m; }
    void SetStereoMix (int trk, xgm::INT16 mixl, xgm::INT16 mixr);
    ITrackInfo *GetTrackInfo(int trk);
  };

}                               // namespace

#endif
