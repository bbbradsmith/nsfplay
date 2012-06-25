#ifndef _IN_NSF_H_
#define _IN_NSF_H_
#include <windows.h>
#include "xgm.h"
#include "utils/nsf_tag.h"
#include "debugout.h"
#include "nsfplug_ui.h"


class KMP_NSF : public xgm::KbMediaPluginMSP
{
public:
  xgm::NSFPlayer *pl;
  xgm::NSFPlayerConfig *cf;
  xgm::NSF *sdat;    
  NSF_TAG *ntag;
  bool not_write_yet;

  KMP_NSF(xgm::NSFPlayer *p, xgm::NSFPlayerConfig *c, xgm::NSF *s) 
    : pl(p), cf(c), sdat(s), xgm::KbMediaPluginMSP(p,c,s)
  {
    ntag = new NSF_TAG(s);
  }

  ~KMP_NSF()
  {
    delete ntag;
  }

  bool Open(const char *cszFileName, SOUNDINFO *pInfo)
  {
    sdat->SetDefaults((*cf)["PLAY_TIME"], (*cf)["FADE_TIME"], (*cf)["LOOP_NUM"]);
    bool ret = KbMediaPluginMSP::Open(cszFileName, pInfo);
    ntag->Sync(); // sdat‚ÌXV‚Æ“¯Šú

    if((int)(*cf)["WRITE_TAGINFO"]&&!ntag->IsExistSection(true)&&!ntag->IsExistSection(false))
      ntag->CreateTag((*cf)["TITLE_FORMAT"]);
  
    if((int)(*cf)["AUTO_DETECT"]&&(int)(*cf)["UPDATE_PLAYLIST"]&&(int)(*cf)["WRITE_TAGINFO"])
    {
      not_write_yet=true;
    }
    else
      not_write_yet=false;

    return ret;
  }

  DWORD Render(BYTE *Buffer, DWORD dwSize)
  {
    DWORD ret = KbMediaPluginMSP::Render(Buffer, dwSize);

    if(not_write_yet&&pl->IsDetected()&&(int)(*cf)["UPDATE_PLAYLIST"])
    {
      if((int)(*cf)["WRITE_TAGINFO"]&&ntag->IsWriteEnable())
      {
        ntag->UpdateTagItem( pl->GetSong(), (*cf)["TITLE_FORMAT"], (*cf)["LOOP_NUM"] );
        not_write_yet=false;
      }
    }  
    return ret;
  }

  void Close()
  {
  }

};

extern HINSTANCE ghPlugin;
extern CRITICAL_SECTION cso;
extern std::set<KMP_NSF *>kmp_set;
extern void kmp_hack(NSFplug_UI *ui);
extern void kmp_unhack();
extern void kmp_update_menu(KMP_NSF *kmp);

#endif