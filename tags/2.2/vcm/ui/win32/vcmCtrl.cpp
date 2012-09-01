#include "stdafx.h"
#include "resource.h"
#include "vcmDlg.h"
#include "vcmCtrl.h"

IMPLEMENT_DYNAMIC(CvcmCtrl, CDialog)

using namespace vcm;

CvcmCtrl::CvcmCtrl( const std::string &id, ValueCtrl *vt, UINT nIDTemplate, CWnd *pParent /*=NULL*/ ) 
  : CDialog ( nIDTemplate, pParent ), m_id(id), m_vt(vt)
{
}

CvcmCtrl::~CvcmCtrl()
{
}

void CvcmCtrl::AttachConfig( Configuration &config )
{
  m_pConfig = &config;
}

CPoint CvcmCtrl::AlignGridX( const CPoint pos )
{
  return CPoint ( ((pos.x+7)/8) * 8, pos.y );
}

void CvcmCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

CSize CvcmCtrl::GetTextSize( const CString &str )
{
  CDC *pDC = GetDC();
  CFont *pFont = GetFont();
  CFont *pOldFont = pDC->SelectObject(pFont);
  CSize size = pDC->GetTextExtent( str );
  pDC->SelectObject(pOldFont);
  ReleaseDC(pDC);
  return size + CSize(2,8);
}

void CvcmCtrl::NotifyUpdate()
{
  ((CvcmDlg *)GetParent())->UpdateConfig(false);
}

BEGIN_MESSAGE_MAP(CvcmCtrl, CDialog)
ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


BOOL CvcmCtrl::OnInitDialog()
{
  CDialog::OnInitDialog();
  ModifyStyle( 0, WS_VISIBLE, 0 );
  return TRUE;  // return TRUE unless you set the focus to a control
  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CvcmCtrl::UpdateStatusMessage()
{
  ((CvcmDlg *)GetParent())->UpdateStatusMessage( this->m_vt->desc );
}

void CvcmCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
  ((CvcmDlg *)GetParent())->UpdateStatusMessage( this->m_vt->desc );
  CDialog::OnMouseMove(nFlags, point);
}
