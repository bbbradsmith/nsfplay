#if !defined(AFX_NSFMEMORYDIALOG_H__C9B73D82_2DAB_4352_8D15_9CE32AD22EC4__INCLUDED_)
#define AFX_NSFMEMORYDIALOG_H__C9B73D82_2DAB_4352_8D15_9CE32AD22EC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NSFMemoryDialog.h : ヘッダー ファイル
//
#include "NSFDialog.h"
/////////////////////////////////////////////////////////////////////////////
// NSFMemoryDialog ダイアログ

class NSFMemoryDialog : public CDialog, public NSFDialog
{
// コンストラクション
public:
	NSFMemoryDialog(CWnd* pParent = NULL);   // 標準のコンストラクタ
  ~NSFMemoryDialog();

// ダイアログ データ
	//{{AFX_DATA(NSFMemoryDialog)
	enum { IDD = IDD_MEMORY };
	//}}AFX_DATA

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(NSFMemoryDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

protected:  
  xgm::UINT8 dumpbuf[65536];
  CFont dlgFont;

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(NSFMemoryDialog)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnJapanese();
	afx_msg void OnEnglish();
	afx_msg void OnMemwrite();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_NSFMEMORYDIALOG_H__C9B73D82_2DAB_4352_8D15_9CE32AD22EC4__INCLUDED_)
