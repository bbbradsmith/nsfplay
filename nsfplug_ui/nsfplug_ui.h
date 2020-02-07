#ifndef _NSFPLUG_UI_
#define _NSFPLUG_UI_
#include "xgm.h"
#include "plugin/winamp2/in_module.h"
#include "nsfconfig_ini.h"

class NSFplug_Model
{
public:
  xgm::NSF *sdat;           // �f�[�^���b�p�ւ̃|�C���^
  xgm::NSFPlayer *pl;       // �v���C���[�ւ̃|�C���^
  xgm::NSFPlayerConfigIni *cf; // �v���C���[�̃R���t�B�O���
};

class NSFplug_UI
{
public:
  enum { 
    DLG_CONFIG, DLG_INFO, DLG_ABOUT, DLG_MIXER, 
    DLG_MASK, DLG_EASY, DLG_MEMORY, DLG_TRACK,
    DLG_PRESET
  };
  virtual ~NSFplug_UI(){}
  virtual xgm::NSFPlayer* GetPlayer()=0;
  virtual xgm::NSFPlayerConfigIni* GetConfig()=0;
  virtual void SetPlayerWindow(HWND)=0;
  virtual void SetWA2InputModule(WA2InputModule *p)=0;
  virtual void OpenDialog(int id)=0;
  virtual void SetInfoData(xgm::NSF *)=0;
  virtual void SetInfoData(char *)=0;
  // ���A���^�C���X�V�𒆒f
  virtual void StopUpdate()=0;
  // ���A���^�C���X�V���J�n
  virtual void StartUpdate()=0;
};

typedef NSFplug_UI * (*CreateNSFplug_UI_t) (NSFplug_Model *cf, int mode);
typedef void (*DeleteNSFplug_UI_t) (NSFplug_UI *p);
typedef const char* (*VersionNSFplug_UI_t) ();

/*
 * DLL�ɊȒP�ɃA�N�Z�X���邽�߂̃��b�p�[�N���X
 */
class NSFplug_UI_DLL : public NSFplug_UI
{
protected:
  HMODULE hModule;
  CreateNSFplug_UI_t pCreate;
  DeleteNSFplug_UI_t pDelete;
  VersionNSFplug_UI_t pVersion;
  NSFplug_UI *pUI;

public:
  NSFplug_UI_DLL(char *dllname, NSFplug_Model *cf, int mode = 0)
  {
    hModule = LoadLibrary(dllname);
    if(hModule)
    {
      pCreate =   (CreateNSFplug_UI_t)GetProcAddress(hModule,  "CreateNSFplug_UI");
      pDelete =   (DeleteNSFplug_UI_t)GetProcAddress(hModule,  "DeleteNSFplug_UI");
      pVersion = (VersionNSFplug_UI_t)GetProcAddress(hModule, "VersionNSFplug_UI");
      if(pCreate && pDelete && pVersion && !::strcmp(pVersion(),NSFPLAY_VERSION))
      {
        pUI = pCreate(cf,mode); 
      }
      else 
      {
        //MessageBox(NULL,"Can't get procedure addresses.","ERROR",MB_OK);
        pUI = NULL;
      }
    }
    else
    {
      //MessageBox(NULL,"Can't get UI module.","ERROR",MB_OK);
      pCreate = NULL;
      pDelete = NULL;
      pVersion = NULL;
      pUI = NULL;
    }

  }

  virtual ~NSFplug_UI_DLL()
  {
    if(hModule)
    {
      if(pUI) pDelete(pUI);
      FreeLibrary(hModule);
      hModule = NULL;
    }
  }

  virtual xgm::NSFPlayer* GetPlayer()
  {
    if(pUI)
      return pUI->GetPlayer();
    else 
      return NULL;
  }

  virtual xgm::NSFPlayerConfigIni* GetConfig()
  {
    if(pUI) 
      return pUI->GetConfig();
    else
      return NULL;
  }

  virtual void SetPlayerWindow(HWND hwnd)
  {
    if(pUI) pUI->SetPlayerWindow(hwnd);
  }

  virtual void OpenDialog(int id)
  {
    if(pUI)
      pUI->OpenDialog(id);
    else
      MessageBox(NULL,"No dialog.","ERROR",MB_OK);
  }
  virtual void SetInfoData(xgm::NSF *nsf)
  {
    if(pUI) pUI->SetInfoData(nsf);
  }
  virtual void SetInfoData(char *fn)
  {
    if(pUI) pUI->SetInfoData(fn);
  }

  virtual HMODULE GetModule()
  {
    return hModule;
  }

  virtual void SetWA2InputModule(WA2InputModule *p)
  {
    if(pUI) pUI->SetWA2InputModule(p);
  }

  virtual void StopUpdate()
  {
    if(pUI) pUI->StopUpdate();
  }
  virtual void StartUpdate()
  {
    if(pUI) pUI->StartUpdate();
  }
};


#endif
