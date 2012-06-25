// NSFMixerPanel.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "NSFMixerPanel.h"
#include "NSFDialogs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NSFMixerPanel ダイアログ

using namespace xgm;

NSFMixerPanel::NSFMixerPanel(CWnd* pParent /*=NULL*/)
	: CDialog(NSFMixerPanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(NSFMixerPanel)
	//}}AFX_DATA_INIT
}

void NSFMixerPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NSFMixerPanel)
	DDX_Control(pDX, IDC_TITLE, m_title);
	DDX_Control(pDX, IDC_MUTE, m_mute);
	DDX_Control(pDX, IDC_SLIDER, m_slider);
	//}}AFX_DATA_MAP
}

void NSFMixerPanel::SetDeviceID(int id)
{
  device_id = id;
}

void NSFMixerPanel::SetVolume(int i)
{
  int val = 255 - i;

  if(val < 0)
    val = 0;
  else if(255 < val)
    val = 255;
  m_slider.SetPos(val);

  UpdateNSFPlayerConfig(false);
}

int NSFMixerPanel::GetVolume()
{
  return 255 - m_slider.GetPos();
}

void NSFMixerPanel::UpdateNSFPlayerConfig(bool b)
{
  NSFDialog::UpdateNSFPlayerConfig(b);

  if(b)
  {
    if(device_id<0)
    {
      if(m_slider)
        m_slider.SetPos(255-(int)(CONFIG["MASTER_VOLUME"]));
    }
    else
    {
      if(m_slider)
        m_slider.SetPos(255 - (int)(pm->cf->GetDeviceConfig(device_id,"VOLUME")));
      if(m_mute)
        m_mute.SetCheck(pm->cf->GetDeviceConfig(device_id,"MUTE"));
    }
  }
  else
  {
    if(device_id<0)
    {
      CONFIG["MASTER_VOLUME"] = 255 - m_slider.GetPos();
    }
    else
    {
      pm->cf->GetDeviceConfig(device_id,"VOLUME") = 255 - m_slider.GetPos();
      pm->cf->GetDeviceConfig(device_id,"MUTE") = m_mute.GetCheck()?true:false;
      pm->cf->Notify(device_id);
    }
  }
}

BEGIN_MESSAGE_MAP(NSFMixerPanel, CDialog)
	//{{AFX_MSG_MAP(NSFMixerPanel)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_MUTE, OnMute)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NSFMixerPanel メッセージ ハンドラ

BOOL NSFMixerPanel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_slider.SetRange(0,255);
  m_slider.SetTicFreq(32);
  m_slider.SetLineSize(1);
  m_slider.SetPageSize(16);

  if(device_id<0)
  {
    m_title.SetWindowText("MASTER");
    m_mute.EnableWindow(FALSE);
    //m_mute.ShowWindow(SW_HIDE);
  }
  else
    m_title.SetWindowText(pm->cf->dname[device_id]); 
	
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void NSFMixerPanel::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  if((CSliderCtrl *)pScrollBar == &m_slider)
    UpdateNSFPlayerConfig(false);
  
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void NSFMixerPanel::OnMute() 
{
  UpdateNSFPlayerConfig(false);
}
