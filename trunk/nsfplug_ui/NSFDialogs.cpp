#include "stdafx.h"
#include "NSFDialogs.h"

NSFDialogManager::NSFDialogManager(NSFplug_Model *p, int mode = 0)
{
  pm = p;
  hWinamp = NULL;
  this->mode = mode;
  info = new NSFInfoDialog();
  info->SetDialogManager(this);
  config = new NSFConfigDialog("NSFPlug");
  config->SetDialogManager(this);
  memory = new NSFMemoryDialog();
  memory->SetDialogManager(this);
  easy = new NSFEasyDialog();
  easy->SetDialogManager(this);
  mixer = new NSFMixerDialog();
  mixer->m_kbflag = mode;
  mixer->SetDialogManager(this);
  panner = new NSFPanDialog();
  panner->SetDialogManager(this);
  mask = new NSFMaskDialog();
  mask->SetDialogManager(this);
  preset = new NSFPresetDialog();
  preset->SetDialogManager(this);
  
  info->Create(mode?MAKEINTRESOURCE(IDD_INFOBOX2):MAKEINTRESOURCE(info->IDD));
  if(mode==0)
  {
    track = new NSFTrackDialog();
    track->SetDialogManager(this);
    track->Create(track->IDD);
  }
  memory->Create(memory->IDD);
  memory->SetWindowPos(NULL,0,0,523,328,SWP_NOZORDER|SWP_NOMOVE);
  mixer->Create(mixer->IDD);
  panner->Create(panner->IDD);
  easy->Create(easy->IDD);
  mask->Create(mask->IDD);
  CString path;
  preset->Reset(CONFIG["INI_FILE"].GetStr().c_str());
  preset->Create(preset->IDD); config->Create(NULL,WS_SYSMENU|WS_POPUP|WS_CAPTION|DS_MODALFRAME|DS_CONTEXTHELP);

  wa2mod = NULL;
  InitializeCriticalSection(&cso);
}

void NSFDialogManager::SetPlayerWindow(HWND hwnd)
{ 
  hWinamp = hwnd ;
}

void NSFDialogManager::UpdateNSFPlayerConfig(bool b)
{
  info->UpdateNSFPlayerConfig(b);
  easy->UpdateNSFPlayerConfig(b);
  memory->UpdateNSFPlayerConfig(b);
  mixer->UpdateNSFPlayerConfig(b);
  panner->UpdateNSFPlayerConfig(b);
  config->UpdateNSFPlayerConfig(b);
  mask->UpdateNSFPlayerConfig(b);
  preset->UpdateNSFPlayerConfig(b);
}

NSFDialogManager::~NSFDialogManager()
{
  info->DestroyWindow();
  delete info;
  memory->DestroyWindow();
  delete memory;
  config->DestroyWindow();
  delete config;
  easy->DestroyWindow();
  delete easy;
  mixer->DestroyWindow();
  delete mixer;
  panner->DestroyWindow();
  delete panner;
  mask->DestroyWindow();
  delete mask;
  preset->DestroyWindow();
  delete preset;

  if(mode==0)
  {
    track->DestroyWindow();
    delete track;
  }
}

// 演奏情報へのアクセスを許可
void NSFDialogManager::StartUpdate()
{ 
  EnterCriticalSection(&cso);
  if(mode==0)
  {
    track->InitList();
    track->m_active = true;
  }
  LeaveCriticalSection(&cso); 
}

// 演奏情報へのアクセスを禁止
void NSFDialogManager::StopUpdate()
{ 
  EnterCriticalSection(&cso); 
  if(mode==0)
  {
    track->m_active = false; 
    track->InitList();
  }
  LeaveCriticalSection(&cso); 
}

#define OPENDIALOG(x,y) case x: y->Open(); break;

void NSFDialogManager::OpenDialog(int id)
{
  switch(id)
  {
    OPENDIALOG(DLG_CONFIG,config);
    OPENDIALOG(DLG_INFO,info);
    OPENDIALOG(DLG_EASY,easy);
    OPENDIALOG(DLG_MASK,mask);
    OPENDIALOG(DLG_MEMORY,memory);
    OPENDIALOG(DLG_TRACK,track);
    OPENDIALOG(DLG_MIXER,mixer);
    OPENDIALOG(DLG_PRESET,preset);
  default:
    break;
  }
}

