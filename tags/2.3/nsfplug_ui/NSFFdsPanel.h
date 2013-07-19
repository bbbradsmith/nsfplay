#if !defined(AFX_NSFFDSPANEL_H__FE81618A_15CD_40A7_B503_C43DA1BFC0D7__INCLUDED_)
#define AFX_NSFFDSPANEL_H__FE81618A_15CD_40A7_B503_C43DA1BFC0D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NSFFdsPanel.h : ヘッダー ファイル
//
#include "NSFDialog.h"
/////////////////////////////////////////////////////////////////////////////
// NSFFdsPanel ダイアログ

class NSFFdsPanel : public CDialog, public NSFDialog
{
// コンストラクション
public:
	NSFFdsPanel(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(NSFFdsPanel)
	enum { IDD = IDD_FDSPANEL };
	UINT	m_nCutoff;
	BOOL	m_4085_reset;
	BOOL	m_write_protect;
	//}}AFX_DATA

public:
	void UpdateNSFPlayerConfig(bool b);

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(NSFFdsPanel)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(NSFFdsPanel)
	afx_msg void OnChangeCutoff();
	afx_msg void On4085Reset();
	afx_msg void OnWriteProtect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_NSFFDSPANEL_H__FE81618A_15CD_40A7_B503_C43DA1BFC0D7__INCLUDED_)
