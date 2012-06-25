// NSFPanPanel.cpp
//

#include "stdafx.h"
#include "NSFPanPanel.h"
#include "NSFDialogs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NSFPanPanel

using namespace xgm;

NSFPanPanel::NSFPanPanel(CWnd* pParent /*=NULL*/)
	: CDialog(NSFPanPanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(NSFPanPanel)
	//}}AFX_DATA_INIT
}

void NSFPanPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NSFPanPanel)
	DDX_Control(pDX, IDC_NAME, m_name);
	DDX_Control(pDX, IDC_PAN, m_span);
	DDX_Control(pDX, IDC_VOL, m_svol);
	//}}AFX_DATA_MAP
}

void NSFPanPanel::SetChannelID(int id)
{
  channel_id = id;
}

void NSFPanPanel::SetVolume(int val)
{
  if(val < 0)
    val = 0;
  else if(128 < val)
    val = 128;
  m_svol.SetPos(val);

  UpdateNSFPlayerConfig(false);
}

int NSFPanPanel::GetVolume()
{
  return m_svol.GetPos();
}

void NSFPanPanel::SetPan(int val)
{
  if(val < 0)
    val = 0;
  else if(255 < val)
    val = 255;
  m_span.SetPos(val);

  UpdateNSFPlayerConfig(false);
}

int NSFPanPanel::GetPan()
{
  return m_span.GetPos();
}

void NSFPanPanel::UpdateNSFPlayerConfig(bool b)
{
  NSFDialog::UpdateNSFPlayerConfig(b);

  if(b)
  {
    if (m_span)
    {
        m_span.SetPos(pm->cf->GetChannelConfig(channel_id, "PAN"));
    }
    if (m_svol)
    {
        m_svol.SetPos(pm->cf->GetChannelConfig(channel_id, "VOL"));
    }
  }
  else
  {
    pm->cf->GetChannelConfig(channel_id,"PAN") = m_span.GetPos();
    pm->cf->GetChannelConfig(channel_id,"VOL") = m_svol.GetPos();
    pm->cf->Notify(pm->cf->channel_device[channel_id]);
  }
}

BEGIN_MESSAGE_MAP(NSFPanPanel, CDialog)
	//{{AFX_MSG_MAP(NSFPanPanel)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NSFPanPanel

BOOL NSFPanPanel::OnInitDialog() 
{
  CDialog::OnInitDialog();

  m_span.SetRange(0,255);
  m_span.SetTic(128);
  m_span.SetLineSize(1);
  m_span.SetPageSize(16);

  m_svol.SetRange(0,128);
  m_svol.SetTicFreq(16);
  m_svol.SetLineSize(1);
  m_svol.SetPageSize(16);

  m_name.SetWindowText(pm->cf->channel_name[channel_id]); 

  return TRUE;
}

void NSFPanPanel::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  if((CSliderCtrl *)pScrollBar == &m_span)
    UpdateNSFPlayerConfig(false);
  if((CSliderCtrl *)pScrollBar == &m_svol)
    UpdateNSFPlayerConfig(false);
  
  CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
