// NSFAudioPanel.cpp : インプリメンテーション ファイル
//
#include "stdafx.h"
#include "NSFAudioPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NSFAudioPanel ダイアログ


NSFAudioPanel::NSFAudioPanel(CWnd* pParent /*=NULL*/)
	: CDialog(NSFAudioPanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(NSFAudioPanel)
	//}}AFX_DATA_INIT
}


void NSFAudioPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NSFAudioPanel)
	DDX_Control(pDX, IDC_THRESHOLD, m_threshold);
	DDX_Control(pDX, IDC_QUALITY, m_quality);
	DDX_Control(pDX, IDC_LFP, m_filter);
	DDX_Control(pDX, IDC_DESC, m_desc);
	//}}AFX_DATA_MAP
}

void NSFAudioPanel::SetDeviceID(int id)
{
  device_id = id;
}

void NSFAudioPanel::UpdateNSFPlayerConfig(bool b)
{
  if(!m_hWnd) return;

  if(b)
  {
    m_quality.SetPos(pm->cf->GetDeviceConfig(device_id,"QUALITY"));
    m_filter.SetPos(pm->cf->GetDeviceConfig(device_id, "FC"));
    m_threshold.SetPos(pm->cf->GetDeviceConfig(device_id,"THRESHOLD"));
  }
  else
  {
    pm->cf->GetDeviceConfig(device_id,"QUALITY") = m_quality.GetPos();
    pm->cf->GetDeviceConfig(device_id,"FC") = m_filter.GetPos()<25?0:m_filter.GetPos();
    pm->cf->GetDeviceConfig(device_id,"FILTER") = 1;
    pm->cf->GetDeviceConfig(device_id,"THRESHOLD") = m_threshold.GetPos();
    pm->cf->Notify(device_id);
  }
}

BEGIN_MESSAGE_MAP(NSFAudioPanel, CDialog)
	//{{AFX_MSG_MAP(NSFAudioPanel)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NSFAudioPanel メッセージ ハンドラ

void NSFAudioPanel::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  if((CSliderCtrl*)pScrollBar == &m_quality)
  {
    CString ss;
    ss.LoadString(IDS_RCONV0 + m_quality.GetPos());
    m_desc.SetWindowText(ss);
    //dynamic_cast<CPropertyPage*>(GetParent())->SetModified(true);
  }
  else if((CSliderCtrl*)pScrollBar == &m_filter)
  {
    //dynamic_cast<CPropertyPage*>(GetParent())->SetModified(true);
  }
  else if((CSliderCtrl*)pScrollBar == &m_threshold)
  {
    //dynamic_cast<CPropertyPage*>(GetParent())->SetModified(true);
  }
  
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL NSFAudioPanel::OnInitDialog() 
{
	CDialog::OnInitDialog();

  m_filter.SetRange(0,400);
  m_filter.SetTicFreq(50);
  m_filter.SetPageSize(25);
  m_filter.SetLineSize(5);

  m_quality.SetRange(0,3);
  m_quality.SetTicFreq(1);
  m_quality.SetPageSize(1);
  m_quality.SetLineSize(1);

  m_threshold.SetRange(0,100);
  m_threshold.SetTicFreq(10);
  m_threshold.SetPageSize(10);
  m_threshold.SetLineSize(1);

  UpdateNSFPlayerConfig(TRUE);

  CString ss;
  ss.LoadString(IDS_RCONV0 + m_quality.GetPos());
  m_desc.SetWindowText(ss);
	
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}
