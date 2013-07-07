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
	//}}AFX_DATA_INIT
}


void NSFFdsPanel::UpdateNSFPlayerConfig(bool b)
{
  NSFDialog::UpdateNSFPlayerConfig(b);

  if(!m_hWnd) return ;

  if(b)
  {
    m_nCutoff = pm->cf->GetDeviceOption(FDS,NES_FDS::OPT_CUTOFF).GetInt();
    UpdateData(FALSE);
  }
  else
  {
    UpdateData(TRUE);
    pm->cf->GetDeviceOption(FDS,NES_FDS::OPT_CUTOFF) = m_nCutoff; 
    pm->cf->Notify(FDS);
  }
}

void NSFFdsPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NSFFdsPanel)
	DDX_Text(pDX, IDC_FDS_LOWPASS, m_nCutoff);
	DDV_MinMaxUInt(pDX, m_nCutoff, 0, 99999);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(NSFFdsPanel, CDialog)
	//{{AFX_MSG_MAP(NSFFdsPanel)
	ON_EN_CHANGE(IDC_FDS_LOWPASS, OnChangeCutoff)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NSFFdsPanel メッセージ ハンドラ

void NSFFdsPanel::OnChangeCutoff() 
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
