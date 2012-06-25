#ifndef _NSFDIALOG_H_
#define _NSFDIALOG_H_
#include "xgm.h"
#include "resource.h"
#include "nsfplug_ui.h"

class NSFDialogManager;

class NSFDialog
{
protected:
  NSFDialogManager *parent;
  NSFplug_Model *pm;

public:
  virtual void Open();
  virtual void SetDialogManager(NSFDialogManager *p);
  virtual void UpdateNSFPlayerConfig(bool b);
};

#define CONFIG (*(pm->cf))

#endif
