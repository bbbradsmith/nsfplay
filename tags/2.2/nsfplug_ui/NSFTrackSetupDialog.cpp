// NSFTrackSetupDialog.cpp : 実装ファイル
//

#include "stdafx.h"
#include "nsfplug_ui.h"
#include "NSFTrackSetupDialog.h"

// NSFTrackSetupDialog ダイアログ

IMPLEMENT_DYNAMIC(NSFTrackSetupDialog, CDialog)
NSFTrackSetupDialog::NSFTrackSetupDialog(CWnd* pParent /*=NULL*/)
	: CDialog(NSFTrackSetupDialog::IDD, pParent)
  , m_freq_value(0)
  , m_delay_value(0)
  , m_graphic_mode(FALSE)
  , m_freq_mode(FALSE)
{
}

NSFTrackSetupDialog::~NSFTrackSetupDialog()
{
}

void NSFTrackSetupDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_DELAY, m_delay);
  DDX_Control(pDX, IDC_FREQ, m_freq);
  DDX_Slider(pDX, IDC_FREQ, m_freq_value);
  DDX_Slider(pDX, IDC_DELAY, m_delay_value);
  DDX_Control(pDX, IDC_FREQ_TEXT, m_freq_text);
  DDX_Control(pDX, IDC_DELAY_TEXT, m_delay_text);
  DDX_Check(pDX, IDC_GRAPHIC_MODE, m_graphic_mode);
  DDX_Check(pDX, IDC_FREQ_MODE, m_freq_mode);
}


BEGIN_MESSAGE_MAP(NSFTrackSetupDialog, CDialog)
  ON_WM_SHOWWINDOW()
  ON_WM_HSCROLL()
END_MESSAGE_MAP()


// NSFTrackSetupDialog メッセージ ハンドラ

BOOL NSFTrackSetupDialog::OnInitDialog()
{
  __super::OnInitDialog();

  m_freq.SetRange(1,60);
  m_freq.SetTicFreq(10);
  m_freq.SetPageSize(5);
  m_freq.SetLineSize(1);
  m_delay.SetRange(-1000,1000);
  m_delay.SetTicFreq(250);
  m_delay.SetPageSize(250);
  m_delay.SetLineSize(50);
  UpdateData(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void NSFTrackSetupDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
  CDialog::OnShowWindow(bShow, nStatus);
  CString text;
  text.Format("%2dHz",m_freq_value);
  m_freq_text.SetWindowText(text);
  text.Format("%3dms",m_delay_value);
  m_delay_text.SetWindowText(text);
}

void NSFTrackSetupDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  CString text;

  if((CSliderCtrl *)pScrollBar == &m_freq)
  {
    text.Format("%2dHz",m_freq.GetPos());
    m_freq_text.SetWindowText(text);
  }
  else if((CSliderCtrl *)pScrollBar == &m_delay)
  {
    text.Format("%3dms",m_delay.GetPos());
    m_delay_text.SetWindowText(text);
  }

  CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

