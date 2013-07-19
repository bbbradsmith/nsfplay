// NSFPanDialog.cpp
//

#include "stdafx.h"
#include "NSFDialogs.h"
#include "NSFPanDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace xgm;

/////////////////////////////////////////////////////////////////////////////
// NSFPanDialog

NSFPanDialog::NSFPanDialog(CWnd* pParent /*=NULL*/)
	: CDialog(NSFPanDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(NSFPanDialog)
		// ClassWizard
	//}}AFX_DATA_INIT
}

void NSFPanDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NSFPanDialog)
		// ClassWizard
	//}}AFX_DATA_MAP
}

void NSFPanDialog::SetDialogManager(NSFDialogManager *p)
{
  for(int i=0;i<NES_CHANNEL_MAX;i++)
  {
    panel[i].SetDialogManager(p);
    panel[i].SetChannelID(i);
  }
  NSFDialog::SetDialogManager(p);
}

void NSFPanDialog::UpdateNSFPlayerConfig(bool b)
{
  NSFDialog::UpdateNSFPlayerConfig(b);

  for(int i=0;i<NES_CHANNEL_MAX;i++)
    panel[i].UpdateNSFPlayerConfig(b);
}

BEGIN_MESSAGE_MAP(NSFPanDialog, CDialog)
	//{{AFX_MSG_MAP(NSFPanDialog)
    ON_COMMAND(IDM_RESET, OnVolReset)
    ON_COMMAND(ID_MONO, OnPanMono)
    ON_COMMAND(ID_RANDOM, OnPanRandom)
    ON_COMMAND(ID_SPREAD, OnPanSpread)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NSFPanDialog

BOOL NSFPanDialog::OnInitDialog() 
{
  CDialog::OnInitDialog();

  HICON hIcon = AfxGetApp()->LoadIcon(IDI_MIXER);
  SetIcon(hIcon, TRUE);
  
  RECT rect;
  int x = 0;
  int y = 0;
  int maxx = 0;
  int maxy = 0;
  rect.top = 0;
  rect.bottom = 0;
  rect.left = 0;
  rect.right = 0;

  const int SPLIT = 15;

  for(int i=0;i<NES_CHANNEL_MAX; ++i)
  {
      if (i == SPLIT)
      {
          y = 0;
          x += rect.right;
      }

      panel[i].Create(IDD_PAN,this);
      panel[i].GetClientRect(&rect);
      panel[i].SetWindowPos(NULL,x,y,rect.right,rect.bottom,SWP_NOZORDER|SWP_SHOWWINDOW);
      y += rect.bottom;
      maxx = x + rect.right;
      if (i < SPLIT) maxy = y;
  }

  GetWindowRect(&rect);
  int th = rect.bottom - rect.top;
  GetClientRect(&rect);
  th -= rect.bottom;
  SetWindowPos(NULL,0,0,maxx+6,maxy+th, SWP_NOMOVE|SWP_NOZORDER);

  return TRUE;
}

void NSFPanDialog::OnVolReset() 
{
  for(int i=0;i<NES_CHANNEL_MAX; i++)
    panel[i].SetVolume(128);
}

void NSFPanDialog::OnPanMono() 
{
  for(int i=0;i<NES_CHANNEL_MAX; i++)
    panel[i].SetPan(128);
}

void NSFPanDialog::OnPanRandom() 
{
  for(int i=0;i<NES_CHANNEL_MAX; i++)
    panel[i].SetPan(::rand() & 255);
}

void NSFPanDialog::OnPanSpread() 
{
  static const int SPREAD_PAN[NES_CHANNEL_MAX] =
  {
      128-64, 128+64, 128-10, 128+10, 128-6 , // APU / DMC
      128+8 , // FDS
      128-80, 128+80, 128+6 , // MMC5
      128-70, 128+70, 128+15, // 5B
      128-90, 128+90, 128+10, // VRC6
      128-30, 128+30, 128-70, 128+70, 128-90, 128+90, // VRC7
      128-20, 128+20, 128-40, 128+40, 128-75, 128+75, 128-99, 128+99 // N163
  };

  for(int i=0;i<NES_CHANNEL_MAX; i++)
    panel[i].SetPan(SPREAD_PAN[i]);
}

