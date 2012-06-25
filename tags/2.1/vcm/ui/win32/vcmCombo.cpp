// vcmCombo.cpp : 実装ファイル
//

#include "stdafx.h"
#include "win32.h"
#include "vcmCtrl.h"
#include "vcmCombo.h"

using namespace vcm;

// CvcmCombo ダイアログ

IMPLEMENT_DYNAMIC(CvcmCombo, CvcmCtrl)
CvcmCombo::CvcmCombo( const std::string &id, VT_COMBO *vt, CWnd* pParent /*=NULL*/)
	: CvcmCtrl( id, vt, CvcmCombo::IDD, pParent ), m_vt(vt)
{
}

CvcmCombo::~CvcmCombo()
{
}

void CvcmCombo::DoDataExchange(CDataExchange* pDX)
{
  CvcmCtrl::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LABEL, m_label);
  DDX_Control(pDX, IDC_COMBO, m_combo);
}


BEGIN_MESSAGE_MAP(CvcmCombo, CvcmCtrl)
  ON_CBN_SELCHANGE(IDC_COMBO, OnCbnSelchangeCombo)
END_MESSAGE_MAP()


// CvcmCombo メッセージ ハンドラ

BOOL CvcmCombo::OnInitDialog()
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

  for(unsigned int i=0; i< m_vt->items.size(); i++)
  {
    CSize tmp = GetTextSize(_T(m_vt->items[i].c_str())); 
    if( size.cx < tmp.cx ) size = tmp;
    m_combo.InsertString( i, _T(m_vt->items[i].c_str()) );
  }
  m_combo.SetItemHeight(-1, size.cy );
  rect.SetRect( 0, 0, size.cx, size.cy * i );
  rect.MoveToXY(pos);
  m_combo.MoveWindow( rect );
  rect.SetRect( 0, 0, size.cx, size.cy );
  rect.MoveToXY(pos);

  ReadWork();

  SetWindowPos( NULL, 0, 0, rect.right, rect.bottom, SWP_NOMOVE|SWP_NOZORDER );
  return TRUE;  // return TRUE unless you set the focus to a control
  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CvcmCombo::ReadWork()
{
  Value result;

  if( m_vt->Export( *m_pConfig, m_id, result ) )
  {
    m_label.EnableWindow(TRUE);
    m_combo.EnableWindow(TRUE);
  }
  else
  {
    m_label.EnableWindow(FALSE);
    m_combo.EnableWindow(FALSE);
  }

  m_combo.SelectString( -1, _T(result) );
}

void CvcmCombo::WriteWork()
{
  CString str;
  m_combo.GetWindowText(str);
  m_vt->Import( *m_pConfig, m_id, Value((std::string)str.GetBuffer()) );
  NotifyUpdate();
}

void CvcmCombo::OnCbnSelchangeCombo()
{
  WriteWork();
}
