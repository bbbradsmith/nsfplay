#ifndef _PLAYER_H_
#define _PLAYER_H_
#include <set>
#include "../xtypes.h"
#include "../debugout.h"
#include "../../vcm/vcm.h"
#include "soundData.h"
#include "../devices/device.h"

namespace xgm {

  /**
   * 音声合成系の設定を管理するクラス
   * @see Player
   */
  class PlayerConfig : public vcm::Configuration
  {
  public:
    virtual bool Load(const char *file, const char *sect)=0;
    virtual bool Save(const char *file, const char *sect)=0;
    virtual bool Load(const char *file, const char *sect, const char *name)=0;
    virtual bool Save(const char *file, const char *sect, const char *name)=0;
  };

  /**
   * 音声合成系
   */
  class Player : public vcm::ObserverI
  {
  protected:
    PlayerConfig *config;

  public:
    /**
     * GetLoopCountメソッドの特別な戻り値
     * @see GetLoopCount
     */
    enum { NEVER_LOOP=-1, INFINITE_LOOP=0 };

    Player()
    {
    }

    virtual ~Player(){ 
    }

    /**
     * PlayerConfigオブジェクトをアタッチする
     * <P>
     * PlayerConfigオブジェクトのオブザーバに，このオブジェクトが自動的に登録される．
     * </P>
     * @param pc アタッチするPlayerConfigオブジェクト
     */
    virtual void SetConfig(PlayerConfig *pc)
    { 
      config = pc;
      config->AttachObserver(this);
    }
    
    virtual PlayerConfig *GetConfig()
    {
      return config;
    }

    /**
     * PlayerConfigオブジェクトが更新された時にコールされるエントリ
     * @param param 更新ステータス
     */
    virtual void Notify(int param){};

    /**
     * 演奏データをロードする
     * <P>
     * Playerオブジェクトは、内部に演奏データのコピーを格納しない。
     * したがって、Playerオブジェクトの演奏中に演奏データを消滅させてはいけない。
     * 演奏データの生成と消滅はPlayerオブジェクトの外部で管理する。
     * </P>
     * @param data 演奏データへのポインタ
     * @return 成功時 ture 失敗時 false
     */
    virtual bool Load(SoundData *sdat)=0;

    /**
     * プレイヤを初期化する
     */
    virtual void Reset()=0;

    /**
     * 再生レートを設定する
     */
    virtual void SetPlayFreq(double rate)=0;

    /**
     * Number of channels to output.
     */
    virtual void SetChannels(int channels)=0;

    /**
     * 音声データのレンダリングを行う
     * <P>
     * バッファサイズは samples * sizeof(INT16)必要
     * </P>
     * @param buf レンダリングデータを格納するバッファ
     * @param samples サンプルの個数
     *                0を与えられてもハングアップしてはならない
     * @return 実際に生成されたサンプル数
     */
    virtual UINT32 Render(INT16 *buf, UINT32 samples)=0;

    /** フェードアウト */
    virtual void FadeOut(int fade_in_ms)=0;

    /**
     * 音声データのレンダリングをスキップする
     * @param samples スキップするサンプル数
     *                0を与えられてもハングアップしてはならない．
     * @return 実際にスキップしたサンプル数
     */
    virtual UINT32 Skip(UINT32 samples)=0;

    /**
     * 演奏が停止したかどうかを調べる．
     * @return 停止中ならtrue．演奏中ならfalse．
     */
    virtual bool IsStopped()=0;

    /**
     * 演奏がループした回数をチェックする．
     * <P>
     * 初回の演奏を1ループ目と数える．
     * </P>
     * @return 演奏がループした回数．
     NEVER_LOOPの場合はループしないデータ，INFINITE_LOOPの場合は無限ループするデータである．
     */
    virtual int  GetLoopCount(){ return NEVER_LOOP; }
    virtual char* GetTitleString(){ return "UNKNOWN"; }
    virtual int GetLength(){ return 5*60*1000; }
    
    /* 番号idの時間timeでのデバイス情報を取得 time==-1の時は現在のデバイス情報を返す */
    virtual IDeviceInfo *GetInfo(int time_in_ms, int device_id){ return NULL; }
  };

  class PlayerMSP : public Player
  {
  public:
    PlayerMSP() : Player(){};
    virtual ~PlayerMSP(){};
    /**
     * 次の曲に進む
     * @param step 進める曲数
     * @return 成功時 true 失敗時 false
     */
    virtual bool NextSong(int s){ return false; }

    /**
     * 前の曲に戻る
     * @param step 戻る曲数
     * @return 成功時 true 失敗時 false
     */
    virtual bool PrevSong(int s){ return false; }

    /**
     * 曲番号を直接設定する
     * @param song 曲番号
     * @return 成功時 true 失敗時 false
     */
    virtual bool SetSong(int song){ return false; }

    /**
     * 現在選択中の曲番号を獲得する
     * @return 曲番号
     */
    virtual int GetSong(){ return -1; }
  };

}// xgm
#endif
