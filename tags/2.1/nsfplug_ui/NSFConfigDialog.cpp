// NSFConfigDialog.cpp : インプリメンテーション ファイル
//
#include "stdafx.h"
#include "NSFConfigDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace xgm;

/////////////////////////////////////////////////////////////////////////////
// NSFConfigDialog

IMPLEMENT_DYNAMIC(NSFConfigDialog, CPropertySheet)

NSFConfigDialog::NSFConfigDialog(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

NSFConfigDialog::NSFConfigDialog(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
  m_psh.dwFlags |= PSH_USEHICON;
  m_psh.hIcon = AfxGetApp()->LoadIcon(IDI_NSF);

  mainPage = new NSFConfigPageMain();
  AddPage(mainPage);

  for(int i=0; i<NES_DEVICE_MAX; i++)
  {
    devicePage[i] = new NSFConfigPageDevice();
    devicePage[i]->SetDeviceID(i);
    AddPage(devicePage[i]);
  }

  apuPanel = new NSFApuPanel();
  devicePage[APU]->SetPanel(apuPanel, IDD_APUPANEL);
  fdsPanel = new NSFFdsPanel();
  devicePage[FDS]->SetPanel(fdsPanel, IDD_FDSPANEL);  
  dmcPanel = new NSFDmcPanel();
  devicePage[DMC]->SetPanel(dmcPanel, IDD_DMCPANEL);  
  mmc5Panel = new NSFMMC5Panel();
  devicePage[MMC5]->SetPanel(mmc5Panel, IDD_MMC5PANEL);  
}

void NSFConfigDialog::SetDialogManager(NSFDialogManager *p)
{
  NSFDialog::SetDialogManager(p);
  mainPage->SetDialogManager(p);
  for(int i=0; i<NES_DEVICE_MAX; i++)
    devicePage[i]->SetDialogManager(p);
}

NSFConfigDialog::~NSFConfigDialog()
{
  delete mainPage;
  for(int i=0; i<NES_DEVICE_MAX; i++)
    delete devicePage[i];
  delete apuPanel;
  delete fdsPanel;
  delete dmcPanel;
  delete mmc5Panel;
}

void NSFConfigDialog::Open()
{
  UpdateNSFPlayerConfig(TRUE);
  ShowWindow(SW_SHOW);
}

void NSFConfigDialog::UpdateNSFPlayerConfig(bool b)
{
  NSFDialog::UpdateNSFPlayerConfig(b);
  mainPage->UpdateNSFPlayerConfig(b);
  for(int i=0; i<NES_DEVICE_MAX; i++)
    devicePage[i]->UpdateNSFPlayerConfig(b);
}

BEGIN_MESSAGE_MAP(NSFConfigDialog, CPropertySheet)
	ON_BN_CLICKED(1, OnOK)
	ON_BN_CLICKED(2, OnApply)
	ON_BN_CLICKED(3, OnCancel)
  ON_WM_CLOSE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NSFConfigDialog メッセージ ハンドラ

BOOL NSFConfigDialog::OnInitDialog()
{
  BOOL bResult = __super::OnInitDialog();

  RECT rect, rect_b;
  GetClientRect(&rect);
  rect_b.top = rect.bottom + 8;
  rect_b.right = rect.right-8;
  rect_b.left= rect_b.right;

  CString str;
  str.LoadString(IDS_CANCEL);

  CDC *pDC = GetDC();
  CSize cs = pDC->GetTextExtent(str);
  ReleaseDC(pDC);

  rect_b.bottom = rect_b.top + cs.cy * 3 / 2;
  rect_b.left = rect_b.right - cs.cx;
  m_cancel.Create(str, BS_TEXT|BS_CENTER|BS_PUSHBUTTON|WS_CHILD|WS_TABSTOP|WS_VISIBLE, rect_b, this, 3);
  m_cancel.SetFont(GetFont());
  rect_b.right = rect_b.left - 4;
  str.LoadString(IDS_APPLY);
  rect_b.left = rect_b.right - cs.cx;
  m_apply.Create(str, BS_TEXT|BS_CENTER|BS_PUSHBUTTON|WS_CHILD|WS_TABSTOP|WS_VISIBLE, rect_b, this, 2);
  m_apply.SetFont(GetFont());
  rect_b.right = rect_b.left - 4;
  rect_b.left = rect_b.right - cs.cx;
  m_ok.Create("OK", BS_TEXT|BS_CENTER|BS_DEFPUSHBUTTON|WS_CHILD|WS_TABSTOP|WS_VISIBLE, rect_b, this, 1);
  m_ok.SetFont(GetFont());

  GetWindowRect(&rect);
  rect.bottom += cs.cy * 3 / 2 + 8 + 8;
  MoveWindow(&rect);

  UpdateNSFPlayerConfig(TRUE);

  return bResult;
}

void NSFConfigDialog::OnOK()
{
  OnApply();
  OnCancel();
}

void NSFConfigDialog::OnApply()
{
  UpdateNSFPlayerConfig(FALSE);
}

void NSFConfigDialog::OnCancel()
{
  UpdateNSFPlayerConfig(TRUE);
  ShowWindow(SW_HIDE);
}

void NSFConfigDialog::OnClose()
{
  // TODO : ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
  ShowWindow(SW_HIDE);
  //__super::OnClose();
}
