#if !defined(AFX_NSFMIXERPANEL_H__19434FC4_26B8_4CF5_ABEA_74EFFA4D82B5__INCLUDED_)
#define AFX_NSFMIXERPANEL_H__19434FC4_26B8_4CF5_ABEA_74EFFA4D82B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NSFMixerPanel.h : ヘッダー ファイル
//
#include "NSFDialog.h"

/////////////////////////////////////////////////////////////////////////////
// NSFMixerPanel ダイアログ

class NSFMixerPanel : public CDialog, public NSFDialog
{
// コンストラクション
public:
	NSFMixerPanel(CWnd* pParent = NULL);   // 標準のコンストラクタ

  int device_id;
  void SetDeviceID(int id);
  void UpdateNSFPlayerConfig(bool b);
  void SetVolume(int i);
  int GetVolume();

// ダイアログ データ
	//{{AFX_DATA(NSFMixerPanel)
	enum { IDD = IDD_MIXER };
	CButton	m_title;
	CButton	m_mute;
	CSliderCtrl	m_slider;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(NSFMixerPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(NSFMixerPanel)
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMute();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_NSFMIXERPANEL_H__19434FC4_26B8_4CF5_ABEA_74EFFA4D82B5__INCLUDED_)
