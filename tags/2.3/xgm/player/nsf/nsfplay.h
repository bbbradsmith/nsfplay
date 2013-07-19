#ifndef _LIBNSF_H_
#define _LIBNSF_H_
#include "../player.h"
#include "nsfconfig.h"
#include "nsf.h"

#include "../../devices/cpu/nes_cpu.h"
#include "../../devices/memory/nes_bank.h"
#include "../../devices/memory/nes_mem.h"
#include "../../devices/sound/nes_apu.h"
#include "../../devices/sound/nes_vrc7.h"
#include "../../devices/sound/nes_fme7.h"
#include "../../devices/sound/nes_vrc6.h"
#include "../../devices/sound/nes_dmc.h"
#include "../../devices/sound/nes_mmc5.h"
#include "../../devices/sound/nes_n106.h"
#include "../../devices/sound/nes_fds.h"
#include "../../devices/audio/filter.h"
#include "../../devices/audio/mixer.h"
#include "../../devices/audio/amplifier.h"
#include "../../devices/audio/rconv.h"
#include "../../devices/audio/echo.h"
#include "../../devices/audio/MedianFilter.h"
#include "../../devices/misc/nes_detect.h"
#include "../../devices/misc/log_cpu.h"

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

    int time_in_ms;             // 演奏した時間(ms)
    bool playtime_detected;     // 演奏時間が検出されたらtrue

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

    ISoundChip *sc[NES_DEVICE_MAX];      // サウンドチップのインスタンス
    RateConverter rconv[NES_DEVICE_MAX]; // サンプリングレートコンバータ
    Filter filter[NES_DEVICE_MAX];       // ローパスフィルター
    Amplifier amp[NES_DEVICE_MAX];       // アンプ
    DCFilter dcf;                        // 最終出力段に掛ける直流フィルタ
    Filter lpf;                          // 最終出力に掛けるローパスフィルタ
    Compressor cmp;                      // 最終出力段に掛けるコンプレッサ
    ILoopDetector *ld;                   // ループ検出器
    CPULogger *logcpu;                   // Logs CPU to file
    EchoUnit echo;
    MedianFilter *mfilter;               // プチノイズ対策のメディアンフィルタ

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
    virtual char *GetTitleString ();

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

    // Notify for panning
    virtual void NotifyPan (int id);

    /** time_in_ms時点でのデバイス情報を取得する */
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
