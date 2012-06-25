// vcmSpin.cpp : 実装ファイル
//

#include "stdafx.h"
#include <math.h>
#include <string>
#include <sstream>
#include "win32.h"
#include "vcmCtrl.h"
#include "vcmSpin.h"

using namespace vcm;

// CvcmSpin ダイアログ
IMPLEMENT_DYNAMIC(CvcmSpin, CvcmCtrl)

CvcmSpin::CvcmSpin( const std::string &id, VT_SPIN *vt, CWnd* pParent /*=NULL*/)
	: CvcmCtrl( id, vt, CvcmSpin::IDD, pParent ), m_vt(vt)
{
}

CvcmSpin::~CvcmSpin()
{
}

void CvcmSpin::DoDataExchange(CDataExchange* pDX)
{
  CvcmCtrl::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LABEL, m_label);
  DDX_Control(pDX, IDC_EDIT, m_edit);
  DDX_Control(pDX, IDC_SPIN, m_spin);
}

BEGIN_MESSAGE_MAP(CvcmSpin, CvcmCtrl)
ON_EN_KILLFOCUS(IDC_EDIT, OnEnKillfocusEdit)
ON_WM_VSCROLL()
END_MESSAGE_MAP()

// CvcmSpin メッセージ ハンドラ

BOOL CvcmSpin::OnInitDialog()
{
  CvcmCtrl::OnInitDialog();

  CPoint pos(0,0);
  CSize size = GetTextSize( _T(m_vt->label.c_str()) );
  CRect rect( pos, size );
  rect.OffsetRect( 0, 4 );
  m_label.MoveWindow( rect );
  m_label.SetWindowText( _T(m_vt->label.c_str()) );
  pos.Offset( rect.Width(), 0 );
  pos = AlignGridX( pos );

  size = GetTextSize( "O" );
  size.cx *= ( (int)log10( m_vt->maxValue ) ) + 1;
  rect.SetRect( 0, 0, size.cx, size.cy );
  rect.MoveToXY(pos);
  m_edit.MoveWindow( rect );
  m_edit.LimitText( (int) log10( m_vt->maxValue ) + 1 );
  pos.Offset( rect.Width(), 0 );
  rect.SetRect( 0, 0, size.cy, size.cy );
  rect.MoveToXY(pos);
  m_spin.MoveWindow( rect );
  m_spin.SetRange( m_vt->minValue, m_vt->maxValue );

  ReadWork();

  SetWindowPos( NULL, 0, 0, rect.right, rect.bottom, SWP_NOMOVE|SWP_NOZORDER );
  return TRUE;  // return TRUE unless you set the focus to a control
  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CvcmSpin::ReadWork()
{
  Value result;

  if( m_vt->Export( *m_pConfig, m_id, result ) )
  {
    m_label.EnableWindow(TRUE);
    m_edit.EnableWindow(TRUE);
    m_spin.EnableWindow(TRUE);
  }
  else
  {
    m_label.EnableWindow(FALSE);
    m_edit.EnableWindow(FALSE);
    m_spin.EnableWindow(FALSE);
  }

  m_edit.SetWindowText( result );
}

void CvcmSpin::WriteWork()
{
  char buf[128];
  m_edit.GetWindowText(buf,16);
  OutputDebugString(buf);
  m_vt->Import( *m_pConfig, m_id, Value(buf) );
  sprintf(buf,"CvcmSpin::WriteWork(%d)\n",m_spin.GetPos());
  OutputDebugString(buf);
  NotifyUpdate();
}

void CvcmSpin::OnEnKillfocusEdit()
{
  WriteWork();
}

void CvcmSpin::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  if( pScrollBar == (CScrollBar *)&m_spin )
  {
    WriteWork();
  }

  CvcmCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}