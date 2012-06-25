#ifndef _NES_FME7_H_
#define _NES_FME7_H_
#include "../device.h"
#include "legacy/emu2149.h"

namespace xgm
{

  class NES_FME7:public ISoundChip
  {
  protected:
    INT32 sm[2][3]; // BS stereo mix
    INT16 buf[2];
    PSG *psg;
    double clock, rate;
    TrackInfoBasic trkinfo[5];
  public:
      NES_FME7 ();
     ~NES_FME7 ();
    void Reset ();
    UINT32 Render (INT32 b[2]);
    bool Read (UINT32 adr, UINT32 & val, UINT32 id=0);
    bool Write (UINT32 adr, UINT32 val, UINT32 id=0);
    void SetClock (double);
    void SetRate (double);
    void SetMask (int m){ if(psg) PSG_setMask(psg,m); }
    void SetStereoMix (int trk, xgm::INT16 mixl, xgm::INT16 mixr);
    ITrackInfo *GetTrackInfo(int trk);
  };

}                               // namespace

#endif
