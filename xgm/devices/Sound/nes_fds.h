#ifndef _NES_FDS_H_
#define _NES_FDS_H_
#include "../device.h"

namespace xgm {

class TrackInfoFDS : public TrackInfoBasic
{
public:
    INT16 wave[64];
    virtual IDeviceInfo *Clone(){ return new TrackInfoFDS(*this); }
};

class NES_FDS : public ISoundChip
{
public:
    enum
    {
        OPT_CUTOFF=0,
        OPT_4085_RESET,
        OPT_WRITE_PROTECT,
        OPT_END
    };

protected:
    double rate, clock;
    int mask;
    INT32 sm[2]; // stereo mix
    UINT32 wout; // current output
    UINT32 vout; // current volume
    TrackInfoFDS trkinfo;
    int option[OPT_END];

    bool master_io;
    UINT32 master_vol;
    UINT32 last_freq; // for trackinfo

    // two wavetables
    enum { TMOD=0, TWAV=1 };
    INT32 wave[2][64];
    UINT32 freq[2];
    UINT32 phase[2];
    bool wav_write;
    bool wav_halt;
    bool env_halt;
    bool mod_halt;
    UINT32 mod_pos;
    UINT32 mod_write_pos;

    // two ramp envelopes
    enum { EMOD=0, EVOL=1 };
    bool env_mode[2];
    bool env_disable[2];
    UINT32 env_timer[2];
    UINT32 env_speed[2];
    UINT32 env_out[2];
    UINT32 master_env_speed;

    // 1-pole RC lowpass filter
    INT32 rc_accum;
    INT32 rc_k;
    INT32 rc_l;

    // nonlinear DAC LUT
    // data from plgDavid, normalized
    // data capture shared from the NESDev server
    const float FDS_LUT_norm[64] = {
        0.0,
        0.011465572752058506,
        0.0233255997300148,
        0.03682375326752663,
        0.04673049971461296,
        0.061058409512043,
        0.07386837154626846,
        0.09098339825868607,
        0.09331251680850983,
        0.10951442271471024,
        0.12239760905504227,
        0.1415075808763504,
        0.1483096331357956,
        0.16800136864185333,
        0.18295270204544067,
        0.20666064321994781,
        0.18730713427066803,
        0.20707780122756958,
        0.2199448049068451,
        0.2434738427400589,
        0.24558208882808685,
        0.26910510659217834,
        0.28437408804893494,
        0.312602698802948,
        0.29783013463020325,
        0.32291364669799805,
        0.33768296241760254,
        0.3677976429462433,
        0.36768317222595215,
        0.3981393277645111,
        0.4163309335708618,
        0.4529191851615906,
        0.3774969279766083,
        0.40523025393486023,
        0.41825342178344727,
        0.45081785321235657,
        0.44415655732154846,
        0.4759543240070343,
        0.4921776056289673,
        0.5303648114204407,
        0.4969009757041931,
        0.5296915769577026,
        0.5450936555862427,
        0.5845786333084106,
        0.576058030128479,
        0.6141541004180908,
        0.6341322660446167,
        0.6813235282897949,
        0.6059473752975464,
        0.6424090266227722,
        0.6577944159507751,
        0.7020274996757507,
        0.6884633898735046,
        0.7309963703155518,
        0.7510766983032227,
        0.802958607673645,
        0.7523477077484131,
        0.7961556911468506,
        0.8153874278068542,
        0.869225800037384,
        0.8552623987197876,
        0.9073793292045593,
        0.9342948198318481,
        1.0
    };

public:
    NES_FDS ();
    virtual ~ NES_FDS ();

    virtual void Reset ();
    virtual void Tick (UINT32 clocks);
    virtual UINT32 Render (INT32 b[2]);
    virtual bool Write (UINT32 adr, UINT32 val, UINT32 id=0);
    virtual bool Read (UINT32 adr, UINT32 & val, UINT32 id=0);
    virtual void SetRate (double);
    virtual void SetClock (double);
    virtual void SetOption (int, int);
    virtual void SetMask(int m){ mask = m&1; }
    virtual void SetStereoMix (int trk, INT16 mixl, INT16 mixr);
    virtual ITrackInfo *GetTrackInfo(int trk);
};

} // namespace xgm

#endif
