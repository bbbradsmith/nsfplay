#if !defined(AFX_NSFDMCPANEL_H__3A5642E6_3DBA_4736_AC9E_070D5867260E__INCLUDED_)
#define AFX_NSFDMCPANEL_H__3A5642E6_3DBA_4736_AC9E_070D5867260E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NSFDmcPanel.h : ヘッダー ファイル
//
#include "NSFDialog.h"
#include "afxcmn.h"
/////////////////////////////////////////////////////////////////////////////
// NSFDmcPanel ダイアログ

class NSFDmcPanel : public CDialog, public NSFDialog
{
// コンストラクション
public:
	NSFDmcPanel(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(NSFDmcPanel)
	enum { IDD = IDD_DMCPANEL };
	BOOL	m_enable_4011;
	BOOL	m_enable_pnoise;
	BOOL	m_nonlinear_mixer;
	BOOL	m_anti_noise;
	BOOL	m_randomize_noise;
	BOOL	m_unmute;
	//}}AFX_DATA

public:
  void UpdateNSFPlayerConfig(bool b);

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(NSFDmcPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(NSFDmcPanel)
	afx_msg void OnEnable4011();
	afx_msg void OnEnablePnoise();
	afx_msg void OnNonlinearMixer();
	afx_msg void OnAntiNoise();
	afx_msg void OnRandomizeNoise();
	afx_msg void OnUnmute();
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_NSFDMCPANEL_H__3A5642E6_3DBA_4736_AC9E_070D5867260E__INCLUDED_)
