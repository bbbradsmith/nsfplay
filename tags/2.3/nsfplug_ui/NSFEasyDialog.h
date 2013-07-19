#if !defined(AFX_NSFEASYDIALOG_H__1C420639_8E4D_4D95_AF54_EB89E50152ED__INCLUDED_)
#define AFX_NSFEASYDIALOG_H__1C420639_8E4D_4D95_AF54_EB89E50152ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NSFEasyDialog.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// NSFEasyDialog ダイアログ
#include "NSFDialog.h"
#include "afxwin.h"
#include "PresetManager.h"
#include "afxcmn.h"

class NSFEasyDialog : public CDialog, public NSFDialog
{
// コンストラクション
public:
	NSFEasyDialog(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(NSFEasyDialog)
	enum { IDD = IDD_EASY };
	CSliderCtrl	m_qualityCtrl;
	CSliderCtrl	m_lpfCtrl;
	//}}AFX_DATA

  void UpdateNSFPlayerConfig(bool b);


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(NSFEasyDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(NSFEasyDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  CComboBox m_comboCtrl;
  PresetManager pman;
  afx_msg void OnCbnSelchangeCombo();
  CSliderCtrl m_hpfCtrl;
  CSliderCtrl m_masterCtrl;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_NSFEASYDIALOG_H__1C420639_8E4D_4D95_AF54_EB89E50152ED__INCLUDED_)
