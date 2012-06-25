// OptionDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "nsfmwin.h"
#include "OptionDlg.h"


// COptionDlg ダイアログ

IMPLEMENT_DYNAMIC(COptionDlg, CDialog)
COptionDlg::COptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionDlg::IDD, pParent)
  , m_priority(0)
  , m_length(0)
  , m_minloop(0)
  , m_usealt(TRUE)
{
}

COptionDlg::~COptionDlg()
{
}

void COptionDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Slider(pDX, IDC_PRIORITY, m_priority);
  DDX_Control(pDX, IDC_PRIORITY, m_prioCtrl);
  DDX_Text(pDX, IDC_SEARCH, m_length);
  DDV_MinMaxInt(pDX, m_length, 1, 65536);
  DDX_Text(pDX, IDC_LOOPLEN, m_minloop);
  DDV_MinMaxInt(pDX, m_minloop, 0, 3600);
  DDX_Check(pDX, IDC_USEALT, m_usealt);
}


BEGIN_MESSAGE_MAP(COptionDlg, CDialog)
END_MESSAGE_MAP()


// COptionDlg メッセージ ハンドラ

BOOL COptionDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  m_prioCtrl.SetRange(0,5,TRUE);

  return TRUE;  // return TRUE unless you set the focus to a control
  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
