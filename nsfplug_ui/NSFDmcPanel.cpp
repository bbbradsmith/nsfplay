WM// NSFDmcPanel.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "NSFDmcPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace xgm;

/////////////////////////////////////////////////////////////////////////////
// NSFDmcPanel ダイアログ


NSFDmcPanel::NSFDmcPanel(CWnd* pParent /*=NULL*/)
  : CDialog(NSFDmcPanel::IDD, pParent)
  , m_enable_4011(TRUE)
  , m_enable_pnoise(TRUE)
  , m_nonlinear_mixer(TRUE)
  , m_anti_click(FALSE)
  , m_randomize_noise(TRUE)
  , m_unmute(TRUE)
  , m_tri_mute(TRUE)
{
}

void NSFDmcPanel::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Check(pDX, IDC_ENABLE_4011, m_enable_4011);
  DDX_Check(pDX, IDC_ENABLE_PNOISE, m_enable_pnoise);
  DDX_Check(pDX, IDC_NONLINEAR_MIXER, m_nonlinear_mixer);
  DDX_Check(pDX, IDC_ANTI_NOISE, m_anti_click);
  DDX_Check(pDX, IDC_RANDOMIZE_NOISE, m_randomize_noise);
  DDX_Check(pDX, IDC_UNMUTE, m_unmute);
  DDX_Check(pDX, IDC_TRI_MUTE, m_tri_mute);
}

void NSFDmcPanel::UpdateNSFPlayerConfig(bool b)
{
  NSFDialog::UpdateNSFPlayerConfig(b);

  if(!m_hWnd) return;

  if(b)
  {
    m_nonlinear_mixer = pm->cf->GetDeviceOption(DMC,NES_DMC::OPT_NONLINEAR_MIXER).GetInt();
    m_enable_pnoise   = pm->cf->GetDeviceOption(DMC,NES_DMC::OPT_ENABLE_PNOISE  ).GetInt();
    m_enable_4011     = pm->cf->GetDeviceOption(DMC,NES_DMC::OPT_ENABLE_4011    ).GetInt();
    m_anti_click      = pm->cf->GetDeviceOption(DMC,NES_DMC::OPT_DPCM_ANTI_CLICK).GetInt();
    m_randomize_noise = pm->cf->GetDeviceOption(DMC,NES_DMC::OPT_RANDOMIZE_NOISE).GetInt();
    m_unmute          = pm->cf->GetDeviceOption(DMC,NES_DMC::OPT_UNMUTE_ON_RESET).GetInt();
    m_tri_mute        = pm->cf->GetDeviceOption(DMC,NES_DMC::OPT_TRI_MUTE       ).GetInt();
    UpdateData(FALSE);
  }
  else
  {
    UpdateData(TRUE);
    pm->cf->GetDeviceOption(DMC,NES_DMC::OPT_NONLINEAR_MIXER) = m_nonlinear_mixer;
    pm->cf->GetDeviceOption(DMC,NES_DMC::OPT_ENABLE_PNOISE  ) = m_enable_pnoise; 
    pm->cf->GetDeviceOption(DMC,NES_DMC::OPT_ENABLE_4011    ) = m_enable_4011; 
    pm->cf->GetDeviceOption(DMC,NES_DMC::OPT_DPCM_ANTI_CLICK) = m_anti_click;
    pm->cf->GetDeviceOption(DMC,NES_DMC::OPT_RANDOMIZE_NOISE) = m_randomize_noise;
    pm->cf->GetDeviceOption(DMC,NES_DMC::OPT_UNMUTE_ON_RESET) = m_unmute;
    pm->cf->GetDeviceOption(DMC,NES_DMC::OPT_TRI_MUTE       ) = m_tri_mute;
    pm->cf->Notify(DMC);
  }
}

BEGIN_MESSAGE_MAP(NSFDmcPanel, CDialog)
	//{{AFX_MSG_MAP(NSFDmcPanel)
	ON_BN_CLICKED(IDC_ENABLE_4011, OnEnable4011)
	ON_BN_CLICKED(IDC_ENABLE_PNOISE, OnEnablePnoise)
	ON_BN_CLICKED(IDC_NONLINEAR_MIXER, OnNonlinearMixer)
	ON_BN_CLICKED(IDC_ANTI_NOISE, OnAntiNoise)
	ON_BN_CLICKED(IDC_RANDOMIZE_NOISE, OnRandomizeNoise)
	ON_BN_CLICKED(IDC_UNMUTE, OnUnmute)
	ON_BN_CLICKED(IDC_TRI_MUTE, OnTriMute)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NSFDmcPanel メッセージ ハンドラ

void NSFDmcPanel::OnEnable4011() 
{
  //dynamic_cast<CPropertyPage*>(GetParent())->SetModified(true);	
}

void NSFDmcPanel::OnEnablePnoise() 
{
  //dynamic_cast<CPropertyPage*>(GetParent())->SetModified(true);	
}

void NSFDmcPanel::OnNonlinearMixer() 
{
  //dynamic_cast<CPropertyPage*>(GetParent())->SetModified(true);		
}

void NSFDmcPanel::OnAntiNoise() 
{
  //dynamic_cast<CPropertyPage*>(GetParent())->SetModified(true);		
}

void NSFDmcPanel::OnRandomizeNoise() 
{
  //dynamic_cast<CPropertyPage*>(GetParent())->SetModified(true);		
}

void NSFDmcPanel::OnUnmute() 
{
  //dynamic_cast<CPropertyPage*>(GetParent())->SetModified(true);		
}

void NSFDmcPanel::OnTriMute() 
{
  //dynamic_cast<CPropertyPage*>(GetParent())->SetModified(true);		
}

BOOL NSFDmcPanel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateNSFPlayerConfig(TRUE);
	
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void NSFDmcPanel::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
