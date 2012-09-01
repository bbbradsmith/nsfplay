// NSFEasyDialog.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "NSFEasyDialog.h"
#include "NSFDialogs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NSFEasyDialog ダイアログ


NSFEasyDialog::NSFEasyDialog(CWnd* pParent /*=NULL*/)
	: CDialog(NSFEasyDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(NSFEasyDialog)
	//}}AFX_DATA_INIT
}


void NSFEasyDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(NSFEasyDialog)
  DDX_Control(pDX, IDC_QUALITY, m_qualityCtrl);
  DDX_Control(pDX, IDC_LPF, m_lpfCtrl);
  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_COMBO, m_comboCtrl);
  DDX_Control(pDX, IDC_HPF, m_hpfCtrl);
  DDX_Control(pDX, IDC_MASTER, m_masterCtrl);
}

BEGIN_MESSAGE_MAP(NSFEasyDialog, CDialog)
	//{{AFX_MSG_MAP(NSFEasyDialog)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
  ON_CBN_SELCHANGE(IDC_COMBO, OnCbnSelchangeCombo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NSFEasyDialog メッセージ ハンドラ

void NSFEasyDialog::OnOK() 
{  
	CDialog::OnOK();
}

void NSFEasyDialog::UpdateNSFPlayerConfig(bool b)
{
  NSFDialog::UpdateNSFPlayerConfig(b);

  if(!m_hWnd) return;

  if(b)
  {
    std::set<CString> list;
    pman.Reset(CONFIG["INI_FILE"].GetStr().c_str());
    list = pman.GetPresetNames();
    m_comboCtrl.ResetContent();
    for(std::set<CString>::iterator it=list.begin();it!=list.end();it++)
      m_comboCtrl.AddString(*it);
    m_comboCtrl.SelectString(-1,CONFIG["LAST_PRESET"].GetStr().c_str());
    m_lpfCtrl.SetPos((int)CONFIG["LPF"]);
    m_hpfCtrl.SetPos(256 - (int)CONFIG["HPF"]);
    m_masterCtrl.SetPos((int)CONFIG["MASTER_VOLUME"]);
    m_qualityCtrl.SetPos(CONFIG.GetDeviceConfig(APU,"QUALITY"));
  }

}

BOOL NSFEasyDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_masterCtrl.SetRange(0,256);
  m_masterCtrl.SetPos(0);
  m_masterCtrl.SetTicFreq(32);
  m_masterCtrl.SetPageSize(16);
  m_masterCtrl.SetLineSize(1);

	m_qualityCtrl.SetRange(0,3);
  m_qualityCtrl.SetPos(0);
  m_qualityCtrl.SetTicFreq(1);
  m_qualityCtrl.SetPageSize(1);
  m_qualityCtrl.SetLineSize(1);

	m_lpfCtrl.SetRange(0,400);
  m_lpfCtrl.SetPos(0);
  m_lpfCtrl.SetTicFreq(50);
  m_lpfCtrl.SetPageSize(25);
  m_lpfCtrl.SetLineSize(5);

  m_hpfCtrl.SetRange(0,256);
  m_hpfCtrl.SetPos(0);
  m_hpfCtrl.SetTicFreq(32);
  m_hpfCtrl.SetPageSize(16);
  m_hpfCtrl.SetLineSize(1);

	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void NSFEasyDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  if((CSliderCtrl*)pScrollBar == &m_masterCtrl)
  {
    CONFIG["MASTER_VOLUME"] = m_masterCtrl.GetPos();
    pm->cf->Notify(-1);
    parent->UpdateNSFPlayerConfig(true);
  }
  if((CSliderCtrl*)pScrollBar == &m_lpfCtrl)
  { 
    CONFIG["LPF"] = m_lpfCtrl.GetPos();
    pm->cf->Notify(-1);
    parent->UpdateNSFPlayerConfig(true);
  }
  else if((CSliderCtrl*)pScrollBar == &m_hpfCtrl)
  {
    CONFIG["HPF"] = 256 - m_hpfCtrl.GetPos();
    pm->cf->Notify(-1);
    parent->UpdateNSFPlayerConfig(true);
  }
  else if((CSliderCtrl*)pScrollBar == &m_qualityCtrl)
  {
    int i;
    int quality = m_qualityCtrl.GetPos();
    int qtbl[NES_DEVICE_MAX][4] = 
    {
      { 0, 1, 2, 3 }, //APU
      { 0, 1, 2, 3 }, //DMC
      { 0, 1, 2, 3 }, //FME7
      { 0, 1, 2, 3 }, //MMC5
      { 0, 1, 1, 1 }, //N106
      { 0, 1, 1, 3 }, //VRC6
      { 0, 1, 3, 3 }, //VRC7
      { 0, 1, 2, 3 }  //FDS
    };

    for(i=0;i<NES_DEVICE_MAX;i++)
    {
      pm->cf->GetDeviceConfig(i,"QUALITY") = qtbl[i][quality];
      pm->cf->Notify(i);
    }
    if(qtbl[FDS][quality]==0)
      pm->cf->GetDeviceOption(FDS,NES_FDS::OPT_USE_PWM) = 0;
    parent->UpdateNSFPlayerConfig(true);
  }

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void NSFEasyDialog::OnCbnSelchangeCombo()
{
  CString text;
  m_comboCtrl.GetWindowText(text);
  pman.LoadPreset(pm->cf, text);
  CONFIG["LAST_PRESET"] = text;
  parent->UpdateNSFPlayerConfig(true);
}
