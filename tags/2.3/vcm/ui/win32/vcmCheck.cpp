// vcmCheck.cpp : 実装ファイル
//

#include "stdafx.h"
#include "win32.h"
#include "vcmCtrl.h"
#include "vcmCheck.h"

using namespace vcm;

// CvcmCheck ダイアログ

IMPLEMENT_DYNAMIC(CvcmCheck, CvcmCtrl)
CvcmCheck::CvcmCheck( const std::string &id, VT_CHECK *vt, CWnd* pParent /*=NULL*/)
	: CvcmCtrl( id, vt, CvcmCheck::IDD, pParent ), m_vt(vt)
{
}

CvcmCheck::~CvcmCheck()
{
}

void CvcmCheck::DoDataExchange(CDataExchange* pDX)
{
  CvcmCtrl::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_CHECK, m_check);
}


BEGIN_MESSAGE_MAP(CvcmCheck, CvcmCtrl)
  ON_BN_CLICKED(IDC_CHECK, OnBnClickedCheck)
END_MESSAGE_MAP()


// CvcmCheck メッセージ ハンドラ

BOOL CvcmCheck::OnInitDialog()
{
  CvcmCtrl::OnInitDialog();

  CPoint pos(0,0);
  CSize size;
  CRect rect;

  size = GetTextSize( _T(m_vt->label.c_str()) );
  rect.SetRect( 0, 0, size.cx+8, size.cy );
  rect.MoveToXY(pos);
  m_check.MoveWindow( rect );
  m_check.SetWindowText( _T(m_vt->label.c_str()) );

  ReadWork();

  SetWindowPos( NULL, 0, 0, rect.right, rect.bottom, SWP_NOMOVE|SWP_NOZORDER );

  return TRUE;  // return TRUE unless you set the focus to a control
  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CvcmCheck::ReadWork()
{
  Value result;

  if( m_vt->Export( *m_pConfig, m_id, result ) )
  {
    m_check.SetCheck( ((int)result)?BST_CHECKED:BST_UNCHECKED );
    m_check.EnableWindow(TRUE);
  }
  else
  {
    m_check.EnableWindow(FALSE);
  }
}

void CvcmCheck::WriteWork()
{
  m_vt->Import( *m_pConfig, m_id, Value( m_check.GetCheck()==BST_CHECKED ));
  NotifyUpdate();
}

void CvcmCheck::OnBnClickedCheck()
{
  WriteWork();
}

