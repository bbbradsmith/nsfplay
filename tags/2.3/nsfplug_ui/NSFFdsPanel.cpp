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
	m_nCutoff = 2000;
	m_4085_reset = FALSE;
	m_write_protect = FALSE;
	//}}AFX_DATA_INIT
}


void NSFFdsPanel::UpdateNSFPlayerConfig(bool b)
{
  NSFDialog::UpdateNSFPlayerConfig(b);

  if(!m_hWnd) return ;

  if(b)
  {
    m_nCutoff = pm->cf->GetDeviceOption(FDS,NES_FDS::OPT_CUTOFF).GetInt();
    m_4085_reset = pm->cf->GetDeviceOption(FDS, NES_FDS::OPT_4085_RESET).GetInt();
    m_write_protect = pm->cf->GetDeviceOption(FDS, NES_FDS::OPT_WRITE_PROTECT).GetInt();
    UpdateData(FALSE);
  }
  else
  {
    UpdateData(TRUE);
    pm->cf->GetDeviceOption(FDS,NES_FDS::OPT_CUTOFF) = m_nCutoff; 
    pm->cf->GetDeviceOption(FDS, NES_FDS::OPT_4085_RESET) = m_4085_reset;
	pm->cf->GetDeviceOption(FDS, NES_FDS::OPT_WRITE_PROTECT) = m_write_protect;
    pm->cf->Notify(FDS);
  }
}

void NSFFdsPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NSFFdsPanel)
	DDX_Text(pDX, IDC_FDS_LOWPASS, m_nCutoff);
	DDV_MinMaxUInt(pDX, m_nCutoff, 0, 99999);
	DDX_Check(pDX, IDC_4085_RESET, m_4085_reset);
	DDX_Check(pDX, IDC_WRITE_PROTECT, m_write_protect);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(NSFFdsPanel, CDialog)
	//{{AFX_MSG_MAP(NSFFdsPanel)
	ON_EN_CHANGE(IDC_FDS_LOWPASS, OnChangeCutoff)
	ON_BN_CLICKED(IDC_4085_RESET, On4085Reset)
	ON_BN_CLICKED(IDC_4085_RESET, OnWriteProtect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NSFFdsPanel メッセージ ハンドラ

void NSFFdsPanel::OnChangeCutoff()
{
	//SetModified(true);
}

void NSFFdsPanel::On4085Reset()
{
	//SetModified(true);
}

void NSFFdsPanel::OnWriteProtect()
{
	//SetModified(true);
}

BOOL NSFFdsPanel::OnInitDialog()
{
  __super::OnInitDialog();

  UpdateNSFPlayerConfig(TRUE);

  return TRUE;  // return TRUE unless you set the focus to a control
  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
