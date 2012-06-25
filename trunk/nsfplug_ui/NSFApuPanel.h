#if !defined(AFX_NSFAPUPANEL_H__1CDA36BC_EB7A_44A9_A188_3F9CFD53A92C__INCLUDED_)
#define AFX_NSFAPUPANEL_H__1CDA36BC_EB7A_44A9_A188_3F9CFD53A92C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NSFApuPanel.h : ヘッダー ファイル
//
#include "NSFDialog.h"

/////////////////////////////////////////////////////////////////////////////
// NSFApuPanel ダイアログ

class NSFApuPanel : public CDialog, public NSFDialog
{
// コンストラクション
public:
	NSFApuPanel(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(NSFApuPanel)
	enum { IDD = IDD_APUPANEL };
	BOOL	m_phase_refresh;
	BOOL	m_unmute_on_reset;
	BOOL	m_freq_limit;
  BOOL  m_nonlinear_mixer;
	//}}AFX_DATA

public:
  void UpdateNSFPlayerConfig(bool b);

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(NSFApuPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(NSFApuPanel)
	afx_msg void OnPhaseRefresh();
	afx_msg void OnUnmuteOnReset();
	afx_msg void OnFreqLimit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_NSFAPUPANEL_H__1CDA36BC_EB7A_44A9_A188_3F9CFD53A92C__INCLUDED_)
