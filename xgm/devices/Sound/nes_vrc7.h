#ifndef _NES_VRC7_H_
#define _NES_VRC7_H_
#include "../device.h"
#include "../../../../VRC7-sound/VRC7-sound/vrc7_sound.h"

namespace xgm
{

  class NES_VRC7 : public ISoundChip
  {
  protected:
    int mask;
    int patch_set;
    const UINT8* patch_custom;
    //INT32 sm[2][6]; // stereo mix
    INT32 sm[2][9]; // stereo mix temporary HACK to support YM2413
    INT16 buf[2];
    struct vrc7_sound *vrc7_s;
    UINT32 divider; // clock divider
    double clock, rate;
    //TrackInfoBasic trkinfo[6];
    TrackInfoBasic trkinfo[9]; // HACK to support YM2413
    bool use_all_channels;
  public:
      NES_VRC7 ();
     ~NES_VRC7 ();

    virtual void Reset ();
    virtual void Tick (UINT32 clocks);
    virtual UINT32 Render (INT32 b[2]);
    virtual bool Read (UINT32 adr, UINT32 & val, UINT32 id=0);
    virtual bool Write (UINT32 adr, UINT32 val, UINT32 id=0);
    virtual void UseAllChannels (bool b);
    virtual void SetPatchSet (int p);
    virtual void SetPatchSetCustom (const UINT8* pset);
    virtual void SetClock (double);
    virtual void SetRate (double);
    virtual void SetMask (int m){ mask = m; if(vrc7_s) vrc7_s->channel_mask=m; }
    virtual void SetStereoMix (int trk, xgm::INT16 mixl, xgm::INT16 mixr);
    virtual ITrackInfo *GetTrackInfo(int trk);
  };

}                               // namespace

#endif
