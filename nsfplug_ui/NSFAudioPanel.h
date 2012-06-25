#if !defined(AFX_NSFAUDIOPANEL_H__97C80FEC_63C5_47AE_8D11_504F136735C1__INCLUDED_)
#define AFX_NSFAUDIOPANEL_H__97C80FEC_63C5_47AE_8D11_504F136735C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NSFAudioPanel.h : ヘッダー ファイル
//
#include "NSFDialog.h"

/////////////////////////////////////////////////////////////////////////////
// NSFAudioPanel ダイアログ

class NSFAudioPanel : public CDialog, public NSFDialog
{
// コンストラクション
public:
	NSFAudioPanel(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(NSFAudioPanel)
	enum { IDD = IDD_AUDIOPANEL };
	CSliderCtrl	m_threshold;
	CSliderCtrl	m_quality;
	CSliderCtrl	m_filter;
	CStatic	m_desc;
	//}}AFX_DATA

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(NSFAudioPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

public:
  int device_id;
  void UpdateNSFPlayerConfig(bool b);
  void SetDeviceID(int id);

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(NSFAudioPanel)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_NSFAUDIOPANEL_H__97C80FEC_63C5_47AE_8D11_504F136735C1__INCLUDED_)
