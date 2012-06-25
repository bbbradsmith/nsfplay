// NSFMMC5Panel.cpp
//

#include "stdafx.h"
#include "NSFMMC5Panel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NSFMMC5Panel

using namespace xgm;

NSFMMC5Panel::NSFMMC5Panel(CWnd* pParent /*=NULL*/)
	: CDialog(NSFMMC5Panel::IDD, pParent)
{
	//{{AFX_DATA_INIT(NSFMMC5Panel)
    m_nonlinear_mixer = FALSE;
    m_phase_refresh = FALSE;
	//}}AFX_DATA_INIT
}


void NSFMMC5Panel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NSFMMC5Panel)
	DDX_Check(pDX, IDC_NONLINEAR_MIXER, m_nonlinear_mixer);
    DDX_Check(pDX, IDC_PHASE_REFRESH, m_phase_refresh);
	//}}AFX_DATA_MAP
}

void NSFMMC5Panel::UpdateNSFPlayerConfig(bool b)
{
  NSFDialog::UpdateNSFPlayerConfig(b);

  if(!m_hWnd) return ;

  if(b)
  {
    m_nonlinear_mixer = pm->cf->GetDeviceOption(MMC5, NES_MMC5::OPT_NONLINEAR_MIXER).GetInt();
    m_phase_refresh = pm->cf->GetDeviceOption(MMC5, NES_MMC5::OPT_PHASE_REFRESH).GetInt();
    UpdateData(FALSE);
  }
  else
  {
    UpdateData(TRUE);
    pm->cf->GetDeviceOption(MMC5, NES_MMC5::OPT_NONLINEAR_MIXER) = m_nonlinear_mixer;
    pm->cf->GetDeviceOption(MMC5, NES_MMC5::OPT_PHASE_REFRESH) = m_phase_refresh;
    pm->cf->Notify(MMC5);
  }
}

BEGIN_MESSAGE_MAP(NSFMMC5Panel, CDialog)
	//{{AFX_MSG_MAP(NSFMMC5Panel)
	ON_BN_CLICKED(IDC_NONLINEAR_MIXER, OnNonlinear)
	ON_BN_CLICKED(IDC_NONLINEAR_MIXER, OnPhaseRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NSFMMC5Panel

void NSFMMC5Panel::OnNonlinear() 
{
  //dynamic_cast<CPropertyPage*>(GetParent())->SetModified(true);	
}

void NSFMMC5Panel::OnPhaseRefresh() 
{
  //dynamic_cast<CPropertyPage*>(GetParent())->SetModified(true);	
}

BOOL NSFMMC5Panel::OnInitDialog()
{
  __super::OnInitDialog();

  // TODO : ??
  UpdateNSFPlayerConfig(TRUE);

  return TRUE;  // return TRUE unless you set the focus to a control
  //
}
