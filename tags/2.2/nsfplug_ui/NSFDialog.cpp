#include "stdafx.h"
#include "NSFDialogs.h"

void NSFDialog::SetDialogManager(NSFDialogManager *p)
{
  parent = p;
  pm = parent->pm;
}

void NSFDialog::UpdateNSFPlayerConfig(bool b)
{
}

void NSFDialog::Open()
{
  UpdateNSFPlayerConfig(true);
  dynamic_cast<CWnd *>(this)->SetWindowPos(&CWnd::wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER);
  dynamic_cast<CWnd *>(this)->ShowWindow(SW_SHOW);
}
