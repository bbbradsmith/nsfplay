#ifndef _SOUNDDATA_H_
#define _SOUNDDATA_H_

#include "../xtypes.h"
#include "../debugout.h"

namespace xgm
{
  /**
   * V[PX^tf[^
   */
  class SoundData
  {
  public:
    SoundData(){};
    virtual ~SoundData(){};
    /**
     * oCiC[W©çÌÇÝÝ
     *
     * @param image oCiC[WÖÌ|C^
     * @param size  C[WÌTCY
     * @return ¬÷ true ¸s false
     */
    virtual bool Load(UINT8 *data, UINT32 size){ return false; }

    /**
     * t@C©çÌÇÝÝ
     * @param fn t@C¼iÜ½Ít@Cîñð¦·¶ñjÖÌ|C^
     */
    virtual bool LoadFile(const char *fn){ return false; }

    /**
     * ^CgÌæ¾
     * @return ^Cgîñ
     */
    virtual char *GetTitleString(const char *format = nullptr){ return ""; }

    /**
     * ^CgÌÝè
     * @param title Vµ¢^CgÖÌ|C^(255¶ÜÅ)
     */
    virtual void SetTitleString(char *title){}

    /**
     * tÔ(ms)Ìæ¾
     */
    virtual int GetLength(){ return 3*60*1000; }

    /**
     * tÔ(ms)ÌÝè
     */
    virtual void SetLength(int time_in_ms){}
  };

  /**
   * ¡Èüètf[^
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