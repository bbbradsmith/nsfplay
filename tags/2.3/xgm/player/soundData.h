#ifndef _SOUNDDATA_H_
#define _SOUNDDATA_H_

#include "../xtypes.h"
#include "../debugout.h"

namespace xgm
{
  /**
   * シーケンス型演奏データ
   */
  class SoundData
  {
  public:
    SoundData(){};
    virtual ~SoundData(){};
    /**
     * バイナリイメージからの読み込み
     *
     * @param image バイナリイメージへのポインタ
     * @param size  イメージのサイズ
     * @return 成功時 true 失敗時 false
     */
    virtual bool Load(UINT8 *data, UINT32 size){ return false; }

    /**
     * ファイルからの読み込み
     * @param fn ファイル名（またはファイル情報を示す文字列）へのポインタ
     */
    virtual bool LoadFile(const char *fn){ return false; }

    /**
     * タイトルの取得
     * @return タイトル情報
     */
    virtual char *GetTitleString(const char *format=NULL){ return ""; }

    /**
     * タイトルの設定
     * @param title 新しいタイトルへのポインタ(255文字まで)
     */
    virtual void SetTitleString(char *title){}

    /**
     * 演奏時間(ms)の取得
     */
    virtual int GetLength(){ return 3*60*1000; }

    /**
     * 演奏時間(ms)の設定
     */
    virtual void SetLength(int time_in_ms){}
  };

  /**
   * 複数曲入り演奏データ
   */
  class SoundDataMSP : public SoundData
  {
  public:
    bool enable_multi_tracks;
    SoundDataMSP() : enable_multi_tracks(false){}
    virtual ~SoundDataMSP(){}
    virtual int GetSong()=0;
    virtual void SetSong(int)=0;
    virtual int GetSongNum()=0;
  };
}

#endif