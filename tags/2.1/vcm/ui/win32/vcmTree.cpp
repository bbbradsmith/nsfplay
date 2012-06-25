// vcmTree.cpp : 実装ファイル
//

#include "stdafx.h"
#include "win32.h"
#include "vcmDlg.h"
#include "vcmTree.h"

using namespace vcm;

// CvcmTree ダイアログ

IMPLEMENT_DYNAMIC(CvcmTree, CDialog)
CvcmTree::CvcmTree(CWnd* pParent /*=NULL*/)
  : CDialog(CvcmTree::IDD, pParent)
{
}

CvcmTree::~CvcmTree()
{
}

void CvcmTree::AttachConfig( Configuration &config )
{
  m_pConfig = &config;
}

void CvcmTree::AttachGroup( ConfigGroup &group )
{
  m_pGroups.push_back(&group);
}

void CvcmTree::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_TREE, m_tree);
}

void CvcmTree::CreateItem( HTREEITEM ht, ConfigGroup *cg )
{
  std::list < ConfigGroup * >::iterator it;
  ht = m_tree.InsertItem( cg->label.c_str(), ht );
  m_tree.SetItemData( ht, (DWORD_PTR)cg );
  for(it=cg->subGroup.begin(); it!=cg->subGroup.end(); it++)
    CreateItem(ht, *it);
}

BEGIN_MESSAGE_MAP(CvcmTree, CDialog)
  ON_WM_DESTROY()
  ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnTvnSelchangedTree)
  ON_WM_SIZE()
END_MESSAGE_MAP()


void CvcmTree::UpdateConfig( bool b )
{
  m_dlg.UpdateConfig( b );
}

// CvcmTree メッセージ ハンドラ
BOOL CvcmTree::OnInitDialog()
{
  CDialog::OnInitDialog();

  std::list < ConfigGroup * >::iterator it;
  for(it=m_pGroups.begin();it!=m_pGroups.end();it++)
    CreateItem( TVI_ROOT, *it );

  CRect rect;
  m_tree.GetWindowRect(rect);
  m_dlgPoint.SetPoint( rect.right+8, 8 );

  return TRUE;  // return TRUE unless you set the focus to a control
  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CvcmTree::OnDestroy()
{
  CDialog::OnDestroy();
  m_dlg.DestroyWindow();
}

void CvcmTree::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
  // TODO : ここにコントロール通知ハンドラ コードを追加します。
  ConfigGroup *cg = (ConfigGroup *)m_tree.GetItemData(  pNMTreeView->itemNew.hItem );

  if(m_dlg.m_hWnd)
  {
    m_tree.SetFocus();
    m_dlg.DestroyWindow();
  }

  m_dlg.AttachConfig( *m_pConfig );
  m_dlg.AttachGroup( *cg );
  m_dlg.Create( CvcmDlg::IDD, this );
  m_dlg.UpdateConfig(false);

  CRect rect;
  GetClientRect(rect);
  AdjustSize( rect.Width(), rect.Height() );
  m_dlg.ShowWindow(SW_SHOW);

  *pResult = 0;
}

void CvcmTree::AdjustSize( int cx, int cy )
{
  CRect wrect, trect, rect;
  
  GetWindowRect(wrect);
  GetClientRect(rect);
  int border_height = wrect.Height()-rect.Height();
  int border_width  = wrect.Width()-rect.Width();

  if( m_tree.m_hWnd )
  {
    m_tree.GetWindowRect( trect );
    ScreenToClient(trect);
    m_tree.MoveWindow( trect.left, trect.top, trect.Width(), cy - border_height );

    if( m_dlg.m_hWnd )
    {
      m_dlg.MoveWindow( trect.left + trect.Width() + 12, trect.top ,
        cx - (trect.left+trect.Width()+12+border_width), cy - border_height );
    }
  }
}

void CvcmTree::OnSize(UINT nType, int cx, int cy)
{
  CDialog::OnSize(nType, cx, cy);
  AdjustSize(cx,cy);
}
