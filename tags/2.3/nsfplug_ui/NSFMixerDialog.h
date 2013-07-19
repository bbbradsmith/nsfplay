#if !defined(AFX_NSFMIXERDIALOG_H__A6CB3FD2_F937_43CE_9365_3BB2FAC76ECE__INCLUDED_)
#define AFX_NSFMIXERDIALOG_H__A6CB3FD2_F937_43CE_9365_3BB2FAC76ECE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NSFMixerDialog.h : ヘッダー ファイル
//
#include "NSFDialog.h"
#include "NSFMixerPanel.h"

/////////////////////////////////////////////////////////////////////////////
// NSFMixerDialog ダイアログ

class NSFMixerDialog : public CDialog, public NSFDialog
{
// コンストラクション
public:
	NSFMixerDialog(CWnd* pParent = NULL);   // 標準のコンストラクタ

  void UpdateNSFPlayerConfig(bool b);

// ダイアログ データ
	//{{AFX_DATA(NSFMixerDialog)
	enum { IDD = IDD_MIXERBOX };
		// メモ: ClassWizard はこの位置にデータ メンバを追加します。
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(NSFMixerDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// ユーザー追加
public:
  NSFMixerPanel panel[xgm::NES_DEVICE_MAX];
  NSFMixerPanel master_panel;
  int m_kbflag;
  CMenu m_Menu;
  void SetDialogManager(NSFDialogManager *p);

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(NSFMixerDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnMask();
  afx_msg void OnUpall();
  afx_msg void OnDownall();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_NSFMIXERDIALOG_H__A6CB3FD2_F937_43CE_9365_3BB2FAC76ECE__INCLUDED_)
