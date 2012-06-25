// vcmSlider.cpp : 実装ファイル
//

#include "stdafx.h"
#include "win32.h"
#include "vcmCtrl.h"
#include "vcmSlider.h"

using namespace vcm;

// CvcmSlider ダイアログ

IMPLEMENT_DYNAMIC(CvcmSlider, CvcmCtrl)
CvcmSlider::CvcmSlider( const std::string &id, VT_SLIDER *vt, CWnd* pParent /*=NULL*/)
	: CvcmCtrl( id, vt, CvcmSlider::IDD, pParent ), m_vt(vt)
{
}

CvcmSlider::~CvcmSlider()
{
}

void CvcmSlider::DoDataExchange(CDataExchange* pDX)
{
  CvcmCtrl::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LABEL, m_label);
  DDX_Control(pDX, IDC_MIN, m_min);
  DDX_Control(pDX, IDC_SLIDER, m_slider);
  DDX_Control(pDX, IDC_MAX, m_max);
}

BEGIN_MESSAGE_MAP(CvcmSlider, CvcmCtrl)
  ON_WM_HSCROLL()
END_MESSAGE_MAP()

// CvcmSlider メッセージ ハンドラ

BOOL CvcmSlider::OnInitDialog()
{
  CvcmCtrl::OnInitDialog();

  CPoint pos(0,0);
  CSize size;
  CRect rect;

  size = GetTextSize( _T(m_vt->label.c_str()) );
  rect.SetRect( 0, 0, size.cx, size.cy );
  rect.OffsetRect( 0, 4 );
  rect.MoveToXY(pos);
  m_label.MoveWindow( rect );
  m_label.SetWindowText( _T(m_vt->label.c_str()) );
  pos.Offset( rect.Width(), 0 );
  pos = AlignGridX( pos );

  size = GetTextSize( _T( m_vt->minDesc.c_str() ) );
  rect.SetRect( 0, 0, size.cx, size.cy );
  rect.OffsetRect( 0, 4 );
  rect.MoveToXY(pos);
  m_min.MoveWindow( rect );
  m_min.SetWindowText( _T( m_vt->minDesc.c_str() ) );
  pos.Offset( rect.Width(), 0 );
  //pos = AlignGridX( pos );

  size.cx = 128;
  rect.SetRect( 0, 0, size.cx, size.cy );
  rect.MoveToXY(pos);
  m_slider.MoveWindow( rect );
  m_slider.SetTicFreq( m_vt->ticFreq );
  m_slider.SetPageSize( m_vt->pageSize );
  m_slider.SetRange( m_vt->minValue, m_vt->maxValue );
  pos.Offset( rect.Width(), 0 );
  pos = AlignGridX( pos );

  size = GetTextSize( _T( m_vt->maxDesc.c_str()) );
  rect.SetRect( 0, 0, size.cx, size.cy );
  rect.OffsetRect( 0, 4 );
  rect.MoveToXY(pos);
  m_max.SetWindowText( _T( m_vt->maxDesc.c_str()) );
  m_max.MoveWindow( rect );

  SetWindowPos( NULL, 0, 0, rect.right, rect.bottom, SWP_NOMOVE|SWP_NOZORDER );
  ReadWork();

  return TRUE;  // return TRUE unless you set the focus to a control
  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CvcmSlider::ReadWork()
{
  Value result;

  if( m_vt->Export( *m_pConfig, m_id, result ) )
  {
    m_label.EnableWindow(TRUE);
    m_min.EnableWindow(TRUE);
    m_max.EnableWindow(TRUE);
    m_slider.EnableWindow(TRUE);
    m_slider.SetPos( (int) result );
  }
  else
  {
    m_label.EnableWindow(FALSE);
    m_min.EnableWindow(FALSE);
    m_max.EnableWindow(FALSE);
    m_slider.EnableWindow(FALSE);
  }
}

void CvcmSlider::WriteWork()
{
  m_vt->Import( *m_pConfig, m_id, Value(m_slider.GetPos()) );
  NotifyUpdate();
}

void CvcmSlider::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  // TODO : ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
  if( pScrollBar == (CScrollBar *)(&m_slider) )
    WriteWork();

  CvcmCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

