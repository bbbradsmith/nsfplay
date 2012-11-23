#ifndef _IN_NSF_H_
#define _IN_NSF_H_
#include "xgm.h"
#include "utils/nsf_tag.h"
#include "nsfplug_ui.h"

class WA2NSF : public WA2InputModuleMSP
{
protected:
  xgm::NSFPlayer *pl;
  xgm::NSFPlayerConfig *cf;
  xgm::NSF *sdat;
  NSFplug_UI *ui;
  NSF_TAG *ntag;

public:
  WA2NSF(xgm::NSFPlayer *p, xgm::NSFPlayerConfig *c, xgm::NSF *s) 
    : pl(p), cf(c), sdat(s), WA2InputModuleMSP(p,c,s)
  {
    ntag = new NSF_TAG(s);
  }

  virtual ~WA2NSF()
  {
    delete ntag;
  }

  virtual void SetUserInterface(NSFplug_UI *u)
  {
    ui = u; 
    if (ui) ui->SetWA2InputModule(this);
  }

  virtual int IsOurFile(char *fn)
  {
    xgm::NSF nsf;
    return nsf.LoadFile(fn);
  }

  virtual void GetFileInfo(char *file, char *title, int *length_in_ms)
  {
    xgm::NSF nsf;
    
    nsf.SetDefaults((*cf)["PLAY_TIME"], (*cf)["FADE_TIME"], (*cf)["LOOP_NUM"]);

    if(file==NULL||file[0]=='\0')
    {
      strcpy(title,pl->GetTitleString());
      *length_in_ms = pl->GetLength();
      return;
    }
    else
    {
      if(nsf.LoadFile(file))
      {
        strcpy(title,nsf.GetTitleString((*cf)["TITLE_FORMAT"]));
        *length_in_ms = nsf.GetLength();
      }
      else
        strcpy(title,"Not a NSF file.");
    }
  }

  virtual int Play(char *fn)
  {
    sdat->SetDefaults((*cf)["PLAY_TIME"], (*cf)["FADE_TIME"], (*cf)["LOOP_NUM"]);

    if(WA2InputModuleMSP::Play(fn))
      return 1;

    ntag->Sync();

    // ローカルタグもtaginfo.tagも無ければ生成する
    if((int)(*cf)["WRITE_TAGINFO"]&&!ntag->IsExistSection(true)&&!ntag->IsExistSection(false))
      ntag->CreateTag((*cf)["TITLE_FORMAT"]);

    if(!sdat->playlist_mode&&(int)(*cf)["READ_TAGINFO"])
      ntag->ReadTagItem(sdat->song);

    if(ui) ui->SetInfoData(sdat);
      
    return 0;
  }

  virtual void Config(HWND hParent)
  {
    if(ui) {
      ui->SetPlayerWindow(pMod->hMainWindow);
      ui->OpenDialog(NSFplug_UI::DLG_CONFIG);
    }
  }

  virtual int InfoBox(char *fn, HWND hParent)
  {
    if(ui) {
      ui->SetPlayerWindow(pMod->hMainWindow);
      ui->SetInfoData(fn);
      ui->OpenDialog(NSFplug_UI::DLG_INFO);
    }
    return 1;
  }

  virtual void PreAutoStop()
  {
    if((int)(*cf)["UPDATE_PLAYLIST"]&&pl->IsDetected())
    {
      if(sdat->playlist_mode)
        SendMessage(pMod->hMainWindow,WM_WA_IPC,(WPARAM)sdat->GetPlaylistString((*cf)["TITLE_FORMAT"],true),IPC_CHANGECURRENTFILE);
      else if((int)(*cf)["WRITE_TAGINFO"]&&ntag->IsWriteEnable())
        ntag->UpdateTagItem(sdat->song,(*cf)["TITLE_FORMAT"],(*cf)["LOOP_NUM"]);
    }
    WA2InputModuleMSP::PreAutoStop();
  }

};

#endif

