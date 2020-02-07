#ifndef _NSFDIALOGMANAGER_H_
#define _NSFDIALOGMANAGER_H_

#include "resource.h"

#include "nsfplug_ui.h"
#include "NSFDialog.h"
#include "NSFEasyDialog.h"
#include "NSFMemoryDialog.h"
#include "NSFMemoryWriteDialog.h"
#include "NSFInfoDialog.h"
#include "NSFConfigDialog.h"
#include "NSFMixerDialog.h"
#include "NSFPanDialog.h"
#include "NSFMaskDialog.h"
#include "NSFTrackDialog.h"
#include "NSFPresetDialog.h"

// �r���[���R���g���[��
class NSFDialogManager : public NSFplug_UI
{
protected:
public:
  NSFplug_Model *pm;

  int mode;
  NSFInfoDialog *info;
  NSFConfigDialog *config;
  NSFMemoryDialog *memory;
  NSFEasyDialog *easy;
  NSFMixerDialog *mixer;
  NSFPanDialog *panner;
  NSFMaskDialog *mask;
  NSFTrackDialog *track;
  NSFPresetDialog *preset;


  HWND hWinamp;
  WA2InputModule *wa2mod;
  CRITICAL_SECTION cso;

  NSFDialogManager(NSFplug_Model *, int mode);
  virtual ~NSFDialogManager();
  virtual NSFPlayer *GetPlayer(){ return pm->pl; }
  virtual NSFPlayerConfigIni *GetConfig(){ return pm->cf; }
  virtual void UpdateNSFPlayerConfig(bool b);
  virtual void SetPlayerWindow(HWND hwnd);
  virtual void SetWA2InputModule(WA2InputModule *p){ wa2mod = p; }
  virtual void OpenDialog(int id);
  virtual void SetInfoData(xgm::NSF *nsf){ info->SetInfo(nsf); }
  virtual void SetInfoData(char *fn){ info->SetInfo(fn); }
  // ���t���ւ̃A�N�Z�X������
  virtual void StartUpdate();
  // ���t���ւ̃A�N�Z�X���֎~
  virtual void StopUpdate();
};

#endif