// KeyDialog.cpp : 実装ファイル
//

#include "stdafx.h"
#include "nsfplug_ui.h"
#include "resource.h"
#include "KeyDialog.h"


IMPLEMENT_DYNAMIC(KeyDialog, CDialog)
KeyDialog::KeyDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KeyDialog::IDD, pParent)
{
  m_bInit = false;
  m_nTimer = 0;
}

KeyDialog::~KeyDialog()
{
}

void KeyDialog::Start(int interval)
{
  if(m_nTimer)
    KillTimer(m_nTimer);
  m_nTimer = SetTimer(1,interval,NULL);
}

void KeyDialog::Stop()
{
  if(m_nTimer)
  {
    KillTimer(m_nTimer);
    m_nTimer = 0;
  }
}

void KeyDialog::Reset()
{
  m_keyheader.Reset();
  m_keywindow.Reset();
}

void KeyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(KeyDialog, CDialog)
  ON_WM_PAINT()
  ON_WM_SIZE()
  ON_WM_TIMER()
END_MESSAGE_MAP()


// KeyDialog メッセージ ハンドラ

void KeyDialog::OnPaint()
{
  CPaintDC dc(this); // device context for painting
}

BOOL KeyDialog::OnInitDialog()
{
  CDialog::OnInitDialog();
  m_keywindow.Create(m_keywindow.IDD, this);
  m_keywindow.ShowWindow(SW_SHOW);
  m_keyheader.Create(m_keyheader.IDD, this);
  m_keyheader.ShowWindow(SW_SHOW);
  return TRUE;  // return TRUE unless you set the focus to a control
  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void KeyDialog::OnSize(UINT nType, int cx, int cy)
{
  CDialog::OnSize(nType, cx, cy);
  if(m_keyheader.m_hWnd)
    m_keyheader.MoveWindow(0,0,cx,m_keyheader.MinHeight());
  if(m_keywindow.m_hWnd)
    m_keywindow.MoveWindow(0,m_keyheader.MinHeight(),cx,cy-m_keyheader.MinHeight());
}

void KeyDialog::OnTimer(UINT nIDEvent)
{
  if(nIDEvent==1) 
  {
    m_keywindow.Invalidate(FALSE);
    m_keyheader.Invalidate(FALSE);
  }
  CDialog::OnTimer(nIDEvent);
}
