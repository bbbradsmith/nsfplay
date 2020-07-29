#ifndef _LIBNSF_H_
#define _LIBNSF_H_
#include "../player.h"
#include "nsfconfig.h"
#include "nsf.h"

#include "../../devices/CPU/nes_cpu.h"
#include "../../devices/Memory/nes_bank.h"
#include "../../devices/Memory/nes_mem.h"
#include "../../devices/Memory/nsf2_vectors.h"
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
#include "../../devices/Audio/fader.h"
#include "../../devices/Audio/amplifier.h"
#include "../../devices/Audio/rconv.h"
#include "../../devices/Audio/echo.h"
#include "../../devices/Audio/MedianFilter.h"
#include "../../devices/Misc/nsf2_irq.h"
#include "../../devices/Misc/nes_detect.h"
#include "../../devices/Misc/log_cpu.h"

namespace xgm
{

  class NSFPlayer : public PlayerMSP
  {
  protected:
    NSFPlayerConfig *config;

    double rate;
    int nch; // number of channels
    int song;

    INT32 last_out;
    int silent_length;

    double cpu_clock_rest;
    double apu_clock_rest;

    int time_in_ms;             // 演奏した時間(ms)
    bool playtime_detected;     // 演奏時間が検出されたらtrue
    bool infinite;               // never fade out

    void Reload ();
    void DetectLoop ();
    void DetectSilent ();
    void CheckTerminal ();

  public:
    void UpdateInfo();

    Bus apu_bus;
    Layer stack;
    Layer layer;
    Mixer mixer;
    Fader fader;

    NES_CPU cpu;
    NES_MEM mem;
    NES_BANK bank;
    NSF2_Vectors nsf2_vectors;
    NSF2_IRQ nsf2_irq;

    ISoundChip *sc[NES_DEVICE_MAX];      // サウンドチップのインスタンス
    Amplifier amp[NES_DEVICE_MAX];       // アンプ
    RateConverter rconv;
    DCFilter dcf;                        // 最終出力段に掛ける直流フィルタ
    Filter lpf;                          // 最終出力に掛けるローパスフィルタ
    ILoopDetector *ld;                   // ループ検出器
    CPULogger *logcpu;                   // Logs CPU to file

    // トラック番号の列挙
    enum {
      APU1_TRK0=0, APU1_TRK1, 
      APU2_TRK0, APU2_TRK1, APU2_TRK2,
      FDS_TRK0,
      MMC5_TRK0, MMC5_TRK1, MMC5_TRK2,
      FME7_TRK0, FME7_TRK1, FME7_TRK2, FME7_TRK3, FME7_TRK4,
      VRC6_TRK0, VRC6_TRK1, VRC6_TRK2,
      VRC7_TRK0, VRC7_TRK1, VRC7_TRK2, VRC7_TRK3, VRC7_TRK4, VRC7_TRK5,
      N106_TRK0, N106_TRK1, N106_TRK2, N106_TRK3, N106_TRK4, N106_TRK5, N106_TRK6, N106_TRK7,
      VRC7_TRK6, VRC7_TRK7, VRC7_TRK8,
      NES_TRACK_MAX
    };
    InfoBuffer infobuf[NES_TRACK_MAX];   // 各トラックの情報を保存
    
    int total_render; // これまでに生成した波形のバイト数
    int frame_render; // １フレーム分のバイト数
    int frame_in_ms;  // １フレームの長さ(ms)

    // 各サウンドチップのエイリアス参照
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

    /** コンフィグ情報のセット */
    virtual void SetConfig(PlayerConfig *pc) ;

    /** データをロードする */
    virtual bool Load (SoundData * sdat);

    /** 再生周波数を設定する */
    virtual void SetPlayFreq (double);

    /**
     * Number of channels to output.
     */
    virtual void SetChannels(int);

    /** リセットする．前の演奏でデータの自己書き換えが発生していても修復しない． */
    virtual void Reset ();

    /** 現在演奏中の曲番号を返す */
    virtual int GetSong ();

    /** フェードアウトを開始する */
    virtual void FadeOut (int fade_in_ms);

    /** 演奏する曲番号を設定する */
    virtual bool SetSong (int s);
    virtual bool PrevSong (int s);
    virtual bool NextSong (int s);

    /** レンダリングを行う */
    virtual UINT32 Render (INT16 * b, UINT32 length);

    /** レンダリングをスキップする */
    virtual UINT32 Skip (UINT32 length);

    /** 曲名を取得する */
    virtual const char *GetTitleString ();

    /** 演奏時間を取得する */
    virtual int GetLength ();

    /** 演奏時間が自動検出されたかどうかをチェックする */
    virtual bool IsDetected ();

    /** 演奏が停止したかどうかをチェックする */
    virtual bool IsStopped ();

    /** 現在のNESメモリ状況を文字列として獲得する */
    virtual void GetMemoryString (char *buf);   // Memory Dump as String

    /** 現在のNESメモリ状況を獲得する */
    virtual void GetMemory (UINT8 * buf);       // Memory Dump

    /** コンフィグレーションの更新情報通知を受け取るコールバック */
    virtual void Notify (int id);

    /** Notify for panning */
    virtual void NotifyPan (int id);

    /** time_in_ms時点でのデバイス情報を取得する */
    virtual IDeviceInfo *GetInfo(int time_in_ms, int device_id);

    /** Whether to use PAL/NTSC/Dendy based on flags and REGION config */
    virtual int GetRegion(UINT8 flags, int pref);

    enum {
        REGION_NTSC = 0,
        REGION_PAL,
        REGION_DENDY
    };

    /** Refresh infinite playback setting from PLAY_ADVANCE config */
    virtual void UpdateInfinite();
  };

}// namespace

#endif
