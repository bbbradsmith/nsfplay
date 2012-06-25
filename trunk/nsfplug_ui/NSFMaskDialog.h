#if !defined(AFX_MASKDLG_H__6D54ED0C_4C2F_49E5_9541_C04A9FC16B5E__INCLUDED_)
#define AFX_MASKDLG_H__6D54ED0C_4C2F_49E5_9541_C04A9FC16B5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CNSFMaskDialog.h : ヘッダー ファイル
//
#include "NSFDialog.h"
/////////////////////////////////////////////////////////////////////////////
// NSFMaskDialog ダイアログ

class NSFMaskDialog : public CDialog, public NSFDialog
{
// コンストラクション
public:
	NSFMaskDialog(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(NSFMaskDialog)
	enum { IDD = IDD_MASK };
	CButton	m_reset;
	BOOL	m_apu_2;
	BOOL	m_apu_1;
	BOOL	m_apu_3;
	BOOL	m_apu_4;
	BOOL	m_apu_5;
	BOOL	m_fds;
	BOOL	m_fme7_1;
	BOOL	m_fme7_2;
	BOOL	m_fme7_3;
	BOOL	m_mmc5_1;
	BOOL	m_mmc5_2;
	BOOL	m_mmc5_3;
	BOOL	m_n106_1;
	BOOL	m_n106_2;
	BOOL	m_n106_3;
	BOOL	m_n106_4;
	BOOL	m_n106_5;
	BOOL	m_n106_6;
	BOOL	m_n106_7;
	BOOL	m_n106_8;
	BOOL	m_vrc6_1;
	BOOL	m_vrc6_2;
	BOOL	m_vrc6_3;
	BOOL	m_vrc7_1;
	BOOL	m_vrc7_2;
	BOOL	m_vrc7_3;
	BOOL	m_vrc7_4;
	BOOL	m_vrc7_5;
	BOOL	m_vrc7_6;
	//}}AFX_DATA

public:
  void UpdateNSFPlayerConfig(bool b);

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(NSFMaskDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(NSFMaskDialog)
	afx_msg void OnApu1();
	afx_msg void OnApu2();
	afx_msg void OnApu3();
	afx_msg void OnApu4();
	afx_msg void OnApu5();
	afx_msg void OnFds();
	afx_msg void OnFme71();
	afx_msg void OnFme72();
	afx_msg void OnFme73();
	afx_msg void OnMmc51();
	afx_msg void OnMmc52();
	afx_msg void OnMmc53();
	afx_msg void OnN1061();
	afx_msg void OnN1062();
	afx_msg void OnN1063();
	afx_msg void OnN1064();
	afx_msg void OnN1065();
	afx_msg void OnN1066();
	afx_msg void OnN1067();
	afx_msg void OnN1068();
	afx_msg void OnVrc61();
	afx_msg void OnVrc62();
	afx_msg void OnVrc63();
	afx_msg void OnVrc71();
	afx_msg void OnVrc72();
	afx_msg void OnVrc73();
	afx_msg void OnVrc74();
	afx_msg void OnVrc75();
	afx_msg void OnVrc76();
	afx_msg void OnReset();
	afx_msg void OnReverse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
//  afx_msg void OnMask();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_MASKDLG_H__6D54ED0C_4C2F_49E5_9541_C04A9FC16B5E__INCLUDED_)
