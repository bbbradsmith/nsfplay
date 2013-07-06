// NSFFdsPanel.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "NSFFdsPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace xgm;

/////////////////////////////////////////////////////////////////////////////
// NSFFdsPanel ダイアログ


NSFFdsPanel::NSFFdsPanel(CWnd* pParent /*=NULL*/)
	: CDialog(NSFFdsPanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(NSFFdsPanel)
	m_car_phase_refresh = FALSE;
	m_mod_phase_refresh = FALSE;
	//}}AFX_DATA_INIT
}


void NSFFdsPanel::UpdateNSFPlayerConfig(bool b)
{
  NSFDialog::UpdateNSFPlayerConfig(b);

  if(!m_hWnd) return ;

  if(b)
  {
    // TODO remove
    //m_car_phase_refresh = pm->cf->GetDeviceOption(FDS,NES_FDS::OPT_CAR_PHASE_REFRESH).GetInt();
    //m_mod_phase_refresh = pm->cf->GetDeviceOption(FDS,NES_FDS::OPT_MOD_PHASE_REFRESH).GetInt();
    UpdateData(FALSE);
  }
  else
  {
    UpdateData(TRUE);
    //pm->cf->GetDeviceOption(FDS,NES_FDS::OPT_CAR_PHASE_REFRESH) = m_car_phase_refresh; 
    //pm->cf->GetDeviceOption(FDS,NES_FDS::OPT_MOD_PHASE_REFRESH) = m_mod_phase_refresh;
    // TODO remove
    pm->cf->Notify(FDS);
  }
}

void NSFFdsPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NSFFdsPanel)
	DDX_Check(pDX, IDC_CAR_PHASE_REFRESH, m_car_phase_refresh);
	DDX_Check(pDX, IDC_MOD_PHASE_REFRESH, m_mod_phase_refresh);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(NSFFdsPanel, CDialog)
	//{{AFX_MSG_MAP(NSFFdsPanel)
	ON_BN_CLICKED(IDC_CAR_PHASE_REFRESH, OnCarPhaseRefresh)
	ON_BN_CLICKED(IDC_MOD_PHASE_REFRESH, OnModPhaseRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NSFFdsPanel メッセージ ハンドラ

void NSFFdsPanel::OnCarPhaseRefresh() 
{
  //dynamic_cast<CPropertyPage*>(GetParent())->SetModified(TRUE);	
}

void NSFFdsPanel::OnModPhaseRefresh() 
{
  //dynamic_cast<CPropertyPage*>(GetParent())->SetModified(TRUE);	
}

BOOL NSFFdsPanel::OnInitDialog()
{
  __super::OnInitDialog();

  UpdateNSFPlayerConfig(TRUE);

  return TRUE;  // return TRUE unless you set the focus to a control
  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
