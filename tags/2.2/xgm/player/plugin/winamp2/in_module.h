#ifndef _IN_MODULE_H_
#define _IN_MODULE_H_
#include <windows.h>
#include <crtdbg.h>
#include <stdio.h>
#include <assert.h>
#include "../../player.h"
extern "C"
{
#include "sdk/IN2.H"
#include "sdk/FRONTEND.H"
}

// 再生サンプル数を再生ミリ秒へ変換
#define POS2MS(x,r) ((int)((double)(x)/(r)*1000))
// 再生ミリ秒を再生サンプル数へ変換
#define MS2POS(x,r) ((unsigned int)((double)(x)/1000*(r)))

#define WM_WA_UNKNOWN (WM_USER + 1)
#define WM_WA_MPEG_EOF (WM_USER + 2)

class WA2InputModule
{
public:
  In_Module *pMod;

protected:
  xgm::Player       *pl; // プレイヤ
  xgm::PlayerConfig *cf; // コンフィグレータ
  xgm::SoundData *sdat;  // 音楽データ操作オブジェクト

  int rate, bps, nch;
  int decode_pos, seek_pos; // 現在デコード中の位置
  bool seek_flag;
  bool pause_flag;          // ポーズフラグ
  bool reset_flag;     // リスタートフラグ

  /* 演奏のためのスレッド */
  unsigned long thread_id;
  HANDLE hPlayThread;
  int killPlayThread;

public:
  WA2InputModule(xgm::Player *p, xgm::PlayerConfig *c, xgm::SoundData *s);
  virtual ~WA2InputModule();
  void SetModule(In_Module *);
  static DWORD WINAPI __stdcall PlayThread(WA2InputModule *_t);
  virtual void PreAutoStop();
  virtual void StartPlayThread();
  virtual void StopPlayThread();
  virtual void Config(HWND hParent);
  virtual void About(HWND hParent);
  virtual void Init();
  virtual void Quit();
  virtual void GetFileInfo(char *file, char *title, int *length_in_ms);
  virtual int InfoBox(char *file, HWND hParent);
  virtual int IsOurFile(char *fn);
  virtual bool LoadFile(char *fn);
  virtual int Play(char *fn);
  virtual void Pause();
  virtual void UnPause();
  virtual int IsPaused();
  virtual void Stop();
  virtual int GetLength();
  virtual int GetOutputTime();
  virtual void SetOutputTime(int time_in_ms);
  virtual void SetVolume(int volume);
  virtual void SetPan(int pan);
  virtual void EQSet(int on, char data[10], int preamp);
  virtual void SetPlayer(xgm::Player *p);
  virtual void SetPlayerConfig(xgm::PlayerConfig *p);
  virtual void SetSoundData(xgm::SoundData *s);
  virtual xgm::Player *GetPlayer();
  virtual xgm::PlayerConfig *GetConfig();
  virtual xgm::SoundData *GetSoundData();
  virtual void QueueFile(const char *file);
  virtual void PlayStart();
  virtual void ClearList();

  // BS needed for synchronization with NSFTrackDialog
  bool GetResetFlag() const { return reset_flag; }
};

/**
 * NSF等の複数曲パック型データ(MSP)に対応したWA2InputModuleの派生
 *
 * <P>
 * 単一ファイルに複数曲が格納されている場合，このクラスはWinampをフック
 * し，ユーザーからの選曲操作をブロックする．代わりに，内部プレイヤーの
 * 選曲ポインタを前後させ，再演奏を開始する．
 * </P>
 */
class WA2InputModuleMSP : public WA2InputModule
{
protected:
  /** 前回に読んだデータのファイル名 */
  char last_fn[512];
  /** メッセージフック */
  HHOOK hHookCallWndProc;
  static WA2InputModuleMSP *_this;
  bool grabbed;

  /** 曲番号をリセットしない */
  bool keep_song; 
  xgm::PlayerMSP *pl;
  xgm::SoundDataMSP *sdat;

  /* ウィンドウプロシージャのアドレス */
  HOOKPROC hp;

public:
  virtual void GrabWinamp();
  static LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam);
  virtual void UnGrabWinamp();
  virtual void PreAutoStop();
  virtual void Init();
  virtual void Quit();
  virtual bool LoadFile(char *fn);
  virtual int Play(char *fn);
  virtual void Keep_Song(bool b);
  WA2InputModuleMSP(xgm::PlayerMSP *p, xgm::PlayerConfig *c, xgm::SoundDataMSP *s);
  virtual ~WA2InputModuleMSP();
};

#endif