#ifndef _KBM_PLUGIN_H_
#define _KBM_PLUGIN_H_
#include <windows.h>
#include "sdk/kmp_pi.h"
#include "../../player.h"

namespace xgm
{

class KbMediaPlugin
{
protected:
  Player *pl;
  PlayerConfig *cf;
  SoundData *sdat;
  int sample_size;

public:
  KbMediaPlugin(){}
  KbMediaPlugin(Player *p, PlayerConfig *c, SoundData *s) : pl(p), cf(c), sdat(s) 
  {
    pl->SetConfig(c);
  }
  virtual ~KbMediaPlugin()
  {
    cf->DetachObserver(pl);
  }
  
  virtual bool Open(const char *cszFileName, SOUNDINFO *pInfo)
  {
    int rate;
    int nch;

    // 読み込みとリセット
    if(sdat->LoadFile(cszFileName)==false)
    {
      return false;
    }

    if(pl->Load(sdat)==false)
      return false;
    
    // KbMediaPlayerの再生レート要求に応じる
    rate = pInfo->dwSamplesPerSec?pInfo->dwSamplesPerSec:(*cf)["RATE"];
    nch = (*cf)["NCH"];
    pl->SetPlayFreq(rate);
    pl->SetChannels(pInfo->dwChannels);
    pl->Reset();

    // SOUNDINFO構造体の構築
    memset(pInfo,0,sizeof(SOUNDINFO));
    pInfo->dwSamplesPerSec = (*cf)["RATE"];
    pInfo->dwChannels = nch; 
    pInfo->dwBitsPerSample = (*cf)["BPS"];
    pInfo->dwLength = 0xFFFFFFFF;
    pInfo->dwSeekable = 0;
    pInfo->dwUnitRender = 4096;
    pInfo->dwReserved1 = 1; //放っておくと無限ループする        zz

    sample_size = pInfo->dwChannels * pInfo->dwBitsPerSample / 8;

    return true;
  }

  virtual void Close()
  {
  }

  virtual DWORD Render(BYTE * Buffer, DWORD dwSize)
  {
    if(pl->IsStopped())
      return 0;

    if(Buffer)
      pl->Render((INT16 *)Buffer, dwSize/sample_size);
    else
      pl->Skip(dwSize/sample_size);

    return dwSize;
  }

  virtual DWORD SetPosition(DWORD dwPos)=0;
};

class KbMediaPluginMSP : public KbMediaPlugin
{
protected:
  PlayerMSP *pl;
  SoundDataMSP *sdat;

public:
  KbMediaPluginMSP()
  {
  }

  KbMediaPluginMSP(PlayerMSP *p, PlayerConfig *c, SoundDataMSP *s) : pl(p), sdat(s), KbMediaPlugin(p,c,s)
  {
  }

  virtual ~KbMediaPluginMSP()
  {
  }

  virtual bool Open(const char *cszFileName, SOUNDINFO *pInfo)
  {
    if(KbMediaPlugin::Open(cszFileName, pInfo))
    {
      /** multi track mode を有効にする */
	    pInfo->dwLength = sdat->GetSongNum() * 1000;
      pInfo->dwSeekable = 1 ; 
      pInfo->dwReserved2 = 1 ; 
      return true;
    }
    else
      return false;
  }

  virtual DWORD SetPosition(DWORD dwPos)
  {
    if(dwPos)
    {
      pl->SetSong(dwPos/1000 - 1);
    }
    pl->Reset();
    return 0;
  }
 
};

}

#endif