#pragma once

#include "NSFDialog.h"
#include "afxcmn.h"
#include "afxwin.h"

// NSFTrackSetupDialog ダイアログ

class NSFTrackSetupDialog : public CDialog
{
	DECLARE_DYNAMIC(NSFTrackSetupDialog)

public:
	NSFTrackSetupDialog(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~NSFTrackSetupDialog();

// ダイアログ データ
	enum { IDD = IDD_TRKINFO_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
  CSliderCtrl m_delay;
  CSliderCtrl m_freq;
  virtual BOOL OnInitDialog();
  afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  int m_freq_value;
  int m_delay_value;
  CStatic m_freq_text;
  CStatic m_delay_text;
  BOOL m_graphic_mode;
  BOOL m_freq_mode;
};
