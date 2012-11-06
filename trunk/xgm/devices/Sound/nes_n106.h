#ifndef _NES_N106_H_
#define _NES_N106_H_
#include "../device.h"
#include "counter.h"

namespace xgm
{
  class TrackInfoN106 : public TrackInfoBasic
  {
  public:   
    int wavelen;
    //INT16 wave[32];
    // BS allowing longer waves
    INT16 wave[256];
    virtual IDeviceInfo *Clone(){ return new TrackInfoN106(*this); }
  };

  class NES_N106:public ISoundChip
  {
  protected:
    double clock, rate;
    int mask;
    INT32 sm[2][8]; // BS stereo mix
    UINT32 pcounter[8];
    UINT32 wait_counter;
    UINT32 wait_incr;
    INT16 wave[0x80 * 2];
    UINT8 reg[0x100];
    UINT32 volume[8];
    UINT32 offset[8];
    UINT32 freq[8];
    UINT32 addr;
    UINT32 length[8];
    UINT32 fflag[8];
    UINT32 phase[8];
    INT32 out[8];
    INT32 calc (int i);
    void writeReg (UINT32 adr, UINT32 val);
    int chnum, chidx;
    TrackInfoN106 trkinfo[8];

  public:
      NES_N106 ();
     ~NES_N106 ();

    virtual void Reset ();
    virtual void Tick (UINT32 clocks);
    virtual UINT32 Render (INT32 b[2]);
    virtual bool Write (UINT32 adr, UINT32 val, UINT32 id=0);
    virtual bool Read (UINT32 adr, UINT32 & val, UINT32 id=0);
    virtual void SetClock (double);
    virtual void SetRate (double);
    virtual void SetMask (int m) { mask = m; }
    virtual void SetStereoMix (int trk, xgm::INT16 mixl, xgm::INT16 mixr);
    virtual ITrackInfo *GetTrackInfo(int trk);
  };
}                               // namespace

#endif
