#if !defined(AFX_NSFCONFIGDIALOG_H__78FFE715_3A96_4C72_8B03_74F1230E2C59__INCLUDED_)
#define AFX_NSFCONFIGDIALOG_H__78FFE715_3A96_4C72_8B03_74F1230E2C59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NSFConfigDialog.h : ヘッダー ファイル
//
#include "NSFDialog.h"
#include "NSFConfigPageMain.h"
#include "NSFConfigPageDevice.h"
#include "NSFAudioPanel.h"
#include "NSFApuPanel.h"
#include "NSFFdsPanel.h"
#include "NSFDmcPanel.h"
#include "NSFMMC5Panel.h"

/////////////////////////////////////////////////////////////////////////////
// NSFConfigDialog

class NSFConfigDialog : public CPropertySheet, public NSFDialog
{
	DECLARE_DYNAMIC(NSFConfigDialog)
//
protected:
  NSFConfigPageMain *mainPage;
  NSFConfigPageDevice *devicePage[xgm::NES_DEVICE_MAX];
  NSFApuPanel *apuPanel;
  NSFFdsPanel *fdsPanel;
  NSFDmcPanel *dmcPanel;
  NSFMMC5Panel *mmc5Panel;
  
// コンストラクション
public:
	NSFConfigDialog(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	NSFConfigDialog(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
// アトリビュート
public:
  CButton m_ok, m_cancel, m_apply;

// オペレーション
public:
  void SetDialogManager(NSFDialogManager *p);
  void UpdateNSFPlayerConfig(bool b);
  void Open();

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(NSFConfigDialog)
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~NSFConfigDialog();

	// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG(NSFConfigDialog)
  afx_msg void OnOK();
  afx_msg void OnApply();
  afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  afx_msg void OnClose();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_NSFCONFIGDIALOG_H__78FFE715_3A96_4C72_8B03_74F1230E2C59__INCLUDED_)
