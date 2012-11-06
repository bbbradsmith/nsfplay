#ifndef _NES_VRC7_H_
#define _NES_VRC7_H_
#include "../device.h"
#include "legacy/emu2413.h"

namespace xgm
{

  class NES_VRC7 : public ISoundChip
  {
  protected:
    int mask;
    unsigned int patch_set;
    INT32 sm[2][6]; // BS stereo mix
    INT16 buf[2];
    OPLL *opll;
    double clock, rate;
    TrackInfoBasic trkinfo[6];
  public:
      NES_VRC7 ();
     ~NES_VRC7 ();

    virtual void Reset ();
    virtual void Tick (int clocks);
    virtual UINT32 Render (INT32 b[2]);
    virtual bool Read (UINT32 adr, UINT32 & val, UINT32 id=0);
    virtual bool Write (UINT32 adr, UINT32 val, UINT32 id=0);
    virtual void SetPatchSet (unsigned int p);
    virtual void SetClock (double);
    virtual void SetRate (double);
    virtual void SetMask (int m){ mask = m; if(opll) OPLL_setMask(opll, m); }
    virtual void SetStereoMix (int trk, xgm::INT16 mixl, xgm::INT16 mixr);
    virtual ITrackInfo *GetTrackInfo(int trk);
  };

}                               // namespace

#endif
