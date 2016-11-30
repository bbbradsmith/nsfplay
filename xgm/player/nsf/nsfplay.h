#ifndef _LIBNSF_H_
#define _LIBNSF_H_
#include "../player.h"
#include "nsfconfig.h"
#include "nsf.h"

#include "../../devices/CPU/nes_cpu.h"
#include "../../devices/Memory/nes_bank.h"
#include "../../devices/Memory/nes_mem.h"
#include "../../devices/Sound/nes_apu.h"
#include "../../devices/Sound/nes_vrc7.h"
#include "../../devices/Sound/nes_fme7.h"
#include "../../devices/Sound/nes_vrc6.h"
#include "../../devices/Sound/nes_dmc.h"
#include "../../devices/Sound/nes_mmc5.h"
#include "../../devices/Sound/nes_n106.h"
#include "../../devices/Sound/nes_fds.h"
#include "../../devices/Audio/filter.h"
#include "../../devices/Audio/mixer.h"
#include "../../devices/Audio/amplifier.h"
#include "../../devices/Audio/rconv.h"
#include "../../devices/Audio/echo.h"
#include "../../devices/Audio/MedianFilter.h"
#include "../../devices/Misc/nes_detect.h"
#include "../../devices/Misc/log_cpu.h"

namespace xgm
{

  class NSFPlayer : public PlayerMSP
  {
	protected:
    NSFPlayerConfig *config;

    enum { PRE_CLICK, CLICKING, POST_CLICK };

    int click_mode;
    double rate;
    int nch; // number of channels
    int song;

    INT32 last_out;
    int silent_length;

    double cpu_clock_rest;
    double apu_clock_rest;

    int time_in_ms;             // tµ½Ô(ms)
    bool playtime_detected;     // tÔªo³ê½çtrue

    void Reload ();
    void DetectLoop ();
    void DetectSilent ();
    void CheckTerminal ();
    void UpdateInfo();

  public:
    Bus apu_bus;
    Layer stack;
    Layer layer;
    Mixer mixer;

    NES_CPU cpu;
    NES_MEM mem;
    NES_BANK bank;

    ISoundChip *sc[NES_DEVICE_MAX];      // TEh`bvÌCX^X
    RateConverter rconv[NES_DEVICE_MAX]; // TvO[gRo[^
    Filter filter[NES_DEVICE_MAX];       // [pXtB^[
    Amplifier amp[NES_DEVICE_MAX];       // Av
    DCFilter dcf;                        // ÅIoÍiÉ|¯é¼¬tB^
    Filter lpf;                          // ÅIoÍÉ|¯é[pXtB^
    Compressor cmp;                      // ÅIoÍiÉ|¯éRvbT
    ILoopDetector *ld;                   // [voí
    CPULogger *logcpu;                   // Logs CPU to file
    EchoUnit echo;
    MedianFilter *mfilter;               // v`mCYÎôÌfBAtB^

    // gbNÔÌñ
    enum {
      APU1_TRK0=0, APU1_TRK1, 
      APU2_TRK0, APU2_TRK1, APU2_TRK2,
      FDS_TRK0,
      MMC5_TRK0, MMC5_TRK1, MMC5_TRK2,
      FME7_TRK0, FME7_TRK1, FME7_TRK2, FME7_TRK3, FME7_TRK4,
      VRC6_TRK0, VRC6_TRK1, VRC6_TRK2,
      VRC7_TRK0, VRC7_TRK1, VRC7_TRK2, VRC7_TRK3, VRC7_TRK4, VRC7_TRK5,
      N106_TRK0, N106_TRK1, N106_TRK2, N106_TRK3, N106_TRK4, N106_TRK5, N106_TRK6, N106_TRK7,
      NES_TRACK_MAX
    };
    InfoBuffer infobuf[NES_TRACK_MAX];   // egbNÌîñðÛ¶
    
    int total_render; // ±êÜÅÉ¶¬µ½g`ÌoCg
    int frame_render; // Pt[ªÌoCg
    int frame_in_ms;  // Pt[Ì·³(ms)

    // eTEh`bvÌGCAXQÆ
    NES_APU *apu;
    NES_DMC *dmc;
    NES_VRC6 *vrc6;
    NES_VRC7 *vrc7;
    NES_FME7 *fme7;
    NES_MMC5 *mmc5;
    NES_N106 *n106;
    NES_FDS *fds;


  public:
    NSF *nsf;
    NSFPlayer ();
    ~NSFPlayer ();

    /** RtBOîñÌZbg */
    virtual void SetConfig(PlayerConfig *pc) ;

    /** f[^ð[h·é */
    virtual bool Load (SoundData * sdat);

    /** Ä¶ügðÝè·é */
    virtual void SetPlayFreq (double);

    /**
     * Number of channels to output.
     */
    virtual void SetChannels(int);

    /** Zbg·éDOÌtÅf[^Ì©È«·¦ª­¶µÄ¢ÄàCµÈ¢D */
    virtual void Reset ();

    /** »ÝtÌÈÔðÔ· */
    virtual int GetSong ();

    /** tF[hAEgðJn·é */
    virtual void FadeOut (int fade_in_ms);

    /** t·éÈÔðÝè·é */
    virtual bool SetSong (int s);
    virtual bool PrevSong (int s);
    virtual bool NextSong (int s);

    /** _Oðs¤ */
    virtual UINT32 Render (INT16 * b, UINT32 length);

    /** _OðXLbv·é */
    virtual UINT32 Skip (UINT32 length);

    /** È¼ðæ¾·é */
    virtual char *GetTitleString ();

    /** tÔðæ¾·é */
    virtual int GetLength ();

    /** tÔª©®o³ê½©Ç¤©ð`FbN·é */
    virtual bool IsDetected ();

    /** tªâ~µ½©Ç¤©ð`FbN·é */
    virtual bool IsStopped ();

    /** »ÝÌNESóµð¶ñÆµÄl¾·é */
    virtual void GetMemoryString (char *buf);   // Memory Dump as String

    /** »ÝÌNESóµðl¾·é */
    virtual void GetMemory (UINT8 * buf);       // Memory Dump

    /** RtBO[VÌXVîñÊmðó¯æéR[obN */
    virtual void Notify (int id);

    // Notify for panning
    virtual void NotifyPan (int id);

    /** time_in_ms_ÅÌfoCXîñðæ¾·é */
    virtual IDeviceInfo *GetInfo(int time_in_ms, int device_id);

    /** Whether to use PAL/NTSC/Dendy based on flags and REGION config */
    virtual int GetRegion(UINT8 flags);
    enum {
        REGION_NTSC = 0,
        REGION_PAL,
        REGION_DENDY
    };
  };

}// namespace

#endif
