// vcmText.cpp : 実装ファイル
//

#include "stdafx.h"
#include "win32.h"
#include "vcmCtrl.h"
#include "vcmText.h"

using namespace vcm;

// CvcmText ダイアログ

IMPLEMENT_DYNAMIC(CvcmText, CvcmCtrl)
CvcmText::CvcmText( const std::string &id, VT_TEXT *vt, CWnd* pParent /*=NULL*/)
	: CvcmCtrl( id, vt, CvcmText::IDD, pParent ), m_vt(vt)
{
}

CvcmText::~CvcmText()
{
}

void CvcmText::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LABEL, m_label);
  DDX_Control(pDX, IDC_TEXT, m_edit);
}


BEGIN_MESSAGE_MAP(CvcmText, CvcmCtrl)
  ON_EN_KILLFOCUS(IDC_TEXT, OnEnKillfocusText)
END_MESSAGE_MAP()


// CvcmText メッセージ ハンドラ

BOOL CvcmText::OnInitDialog()
{
  CvcmCtrl::OnInitDialog();

  CPoint pos(0,0);
  CSize size = GetTextSize( _T(m_vt->label.c_str()) );
  CRect rect( pos, size );
  rect.OffsetRect( 0, 2 );
  m_label.MoveWindow( rect );
  m_label.SetWindowText( _T(m_vt->label.c_str()) );
  pos.Offset( rect.Width(), 0 );
  pos = AlignGridX( pos );

  size = GetTextSize( "M" );
  size.cx *= m_vt->maxLength;
  rect.SetRect( 0, 0, size.cx, size.cy );
  rect.MoveToXY(pos);
  m_edit.MoveWindow( rect );
  m_edit.LimitText( m_vt->maxLength );
  pos.Offset( rect.Width(), 0 );
  rect.SetRect( 0, 0, size.cy, size.cy );
  rect.MoveToXY(pos);

  ReadWork();

  SetWindowPos( NULL, 0, 0, rect.right, rect.bottom, SWP_NOMOVE|SWP_NOZORDER );
  return TRUE;  // return TRUE unless you set the focus to a control
  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CvcmText::ReadWork()
{
  Value result;

  if( m_vt->Export( *m_pConfig, m_id, result ) )
  {
    m_label.EnableWindow(TRUE);
    m_edit.EnableWindow(TRUE);
  }
  else
  {
    m_label.EnableWindow(FALSE);
    m_edit.EnableWindow(FALSE);
  }
  m_edit.SetWindowText( ((std::string)result).c_str() );
}

void CvcmText::WriteWork()
{
  char *buf = new char[m_vt->maxLength+2];
  m_edit.GetWindowText( buf, m_vt->maxLength+1 );
  m_vt->Import( *m_pConfig, m_id, Value( buf ) );
  delete [] buf;
  NotifyUpdate();
}

void CvcmText::OnEnKillfocusText()
{
  WriteWork();
}
