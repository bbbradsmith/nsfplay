// vcmDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "win32.h"
#include "vcmDlg.h"
#include "vcmSpin.h"
#include "vcmSlider.h"
#include "vcmCheck.h"
#include "vcmCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace vcm;

// CvcmDlg ダイアログ

CvcmDlg::CvcmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CvcmDlg::IDD, pParent), m_pConfig(NULL), m_pGroup(NULL), m_dlgSize(64,64)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CvcmDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CvcmDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
  ON_WM_DESTROY()
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_HSCROLL()
END_MESSAGE_MAP()

// CvcmDlg メッセージ ハンドラ

BOOL CvcmDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

  // コンフィグデータの取り出し
  m_ctrlPos.SetPoint(0,0);
  m_dlgSize.SetSize(0,0);

  ConfigGroup::GroupList::iterator it;

  for( it=m_pGroup->members.begin(); it!=m_pGroup->members.end(); it++)
  {
    CvcmCtrl *pCtrl;
    CRect rect;

    switch(it->second->ctrlType)
    {
    case ValueCtrl::CT_CHECK:
      pCtrl = new CvcmCheck( it->first, (VT_CHECK *)it->second, this );
      pCtrl->AttachConfig( *m_pConfig );
      ((CvcmCheck *)pCtrl)->Create( CvcmCheck::IDD, this );
      break;
    
    case ValueCtrl::CT_SPIN:
      pCtrl = new CvcmSpin( it->first, (VT_SPIN *)it->second, this );
      pCtrl->AttachConfig( *m_pConfig );
      ((CvcmSpin *)pCtrl)->Create( CvcmSpin::IDD, this );
      break;
    
    case ValueCtrl::CT_TEXT:
      pCtrl = new CvcmText( it->first, (VT_TEXT *)it->second, this );
      pCtrl->AttachConfig( *m_pConfig );
      ((CvcmText *)pCtrl)->Create( CvcmText::IDD, this );
      break;
    
    case ValueCtrl::CT_SLIDER:
      pCtrl = new CvcmSlider( it->first, (VT_SLIDER *)it->second, this );
      pCtrl->AttachConfig( *m_pConfig );
      ((CvcmSlider *)pCtrl)->Create( CvcmSlider::IDD, this );
      break;

    case ValueCtrl::CT_COMBO:
      pCtrl = new CvcmCombo( it->first, (VT_COMBO *)it->second, this );
      pCtrl->AttachConfig( *m_pConfig );
      ((CvcmCombo *)pCtrl)->Create( CvcmCombo::IDD, this );
      break;

    default:
      pCtrl = NULL;
      break;
    }

    if(pCtrl)
    {
      m_ctrl2id[pCtrl] = it->first;

      pCtrl->GetWindowRect(rect);      
      pCtrl->SetWindowPos( NULL, m_ctrlPos.x, m_ctrlPos.y, rect.Width(), rect.Height()*3/2, SWP_NOZORDER);
      m_ctrlPos.y += rect.Height() * 3 / 2;

      if( m_dlgSize.cx < rect.Width() )
        m_dlgSize.cx = rect.Width()+8;
      if( m_dlgSize.cy < m_ctrlPos.y )
        m_dlgSize.cy = m_ctrlPos.y ;
    }

    m_nPosV = m_nPosH = 0;

    SetWindowPos ( NULL, 0, 0, m_dlgSize.cx, m_dlgSize.cy, SWP_NOMOVE|SWP_NOZORDER );
  }

  m_block_update = false;
  UpdateConfig(false);
	
	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CvcmDlg::AttachConfig( Configuration &config )
{
  m_pConfig = &config;
}

void CvcmDlg::AttachGroup( ConfigGroup &group )
{
  m_pGroup = &group;
}

void CvcmDlg::UpdateConfig(bool b)
{
  if(!m_block_update)
  {
    std::map<CvcmCtrl*,std::string>::iterator it;
    if(b) // ダイアログ→コンフィグ
    {
      for(it=m_ctrl2id.begin();it!=m_ctrl2id.end();it++)
        it->first->WriteWork();
    } 
    else  // コンフィグ→ダイアログ
    {
      for(it=m_ctrl2id.begin();it!=m_ctrl2id.end();it++)
        it->first->ReadWork();
    }
  }
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CvcmDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CvcmDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CvcmDlg::OnDestroy()
{
  CDialog::OnDestroy();

  m_block_update = true;

  std::map< CvcmCtrl *, std::string >::iterator it;
  for(it=m_ctrl2id.begin();it!=m_ctrl2id.end();it++)
  {
    it->first->DestroyWindow();
    delete it->first;
  }
  m_ctrl2id.clear();

}

void CvcmDlg::UpdateStatusMessage( const std::string &str )
{
}

void CvcmDlg::OnSize(UINT nType, int cx, int cy)
{
  CDialog::OnSize(nType, cx, cy);

  CRect rect(0,0,0,0);

  if( m_dlgSize.cy > cy )
  {
    ShowScrollBar(SB_VERT);
    SCROLLINFO sif;
    sif.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
    sif.nPos = 0;
    sif.nMin = 0;
    sif.nMax = m_dlgSize.cy - cy;
    sif.nPage = sif.nMax / 4 ;
    SetScrollInfo( SB_VERT, &sif );
    ScrollWindow( 0, m_nPosV );
    m_nPosV = 0;
  }
  else
    ShowScrollBar(SB_VERT,FALSE);

  if( m_dlgSize.cx > cx )
  {
    ShowScrollBar(SB_HORZ);
    SCROLLINFO sif;
    sif.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
    sif.nPos = 0;
    sif.nMin = 0;
    sif.nMax = m_dlgSize.cx - cx ;
    sif.nPage = sif.nMax / 4 ;
    SetScrollInfo( SB_HORZ, &sif );
    ScrollWindow( m_nPosH, 0 );
    m_nPosH = 0;
  }
  else
    ShowScrollBar(SB_HORZ,FALSE);
}


void CvcmDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  SCROLLINFO sif;
  GetScrollInfo(SB_VERT, &sif);

  switch( nSBCode )
  {
  case SB_THUMBTRACK:
  case SB_THUMBPOSITION:
    sif.nPos = nPos;
    break;

  case SB_PAGEDOWN:
    sif.nPos += sif.nPage;
    break;

  case SB_LINEDOWN:
    sif.nPos ++;
    break;

  case SB_PAGEUP:
    sif.nPos -= sif.nPage;
    break;

  case SB_LINEUP:
    sif.nPos --;
    break;

  default:
    break;
  }

  SetScrollInfo( SB_VERT, &sif );
  ScrollWindow( 0, m_nPosV - GetScrollPos(SB_VERT) );
  m_nPosV = GetScrollPos(SB_VERT);
  UpdateWindow();

  CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CvcmDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  SCROLLINFO sif;
  GetScrollInfo(SB_HORZ, &sif);

  switch( nSBCode )
  {
  case SB_THUMBTRACK:
  case SB_THUMBPOSITION:
    sif.nPos = nPos;
    break;

  case SB_PAGERIGHT:
    sif.nPos += sif.nPage;
    break;

  case SB_LINERIGHT:
    sif.nPos ++;
    break;

  case SB_PAGELEFT:
    sif.nPos -= sif.nPage;
    break;

  case SB_LINELEFT:
    sif.nPos --;
    break;

  default:
    break;
  }

  SetScrollInfo( SB_HORZ, &sif );
  ScrollWindow( m_nPosH - GetScrollPos(SB_HORZ), 0 );
  m_nPosH = GetScrollPos(SB_HORZ);
  UpdateWindow();

  CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
