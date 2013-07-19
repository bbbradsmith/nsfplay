// vcmDlg.h : ヘッダー ファイル
//

#pragma once
#include "../../vcm.h"
#include "vcmCtrl.h"
#include "vcmSpin.h"
#include "vcmSlider.h"
#include "vcmCheck.h"
#include "vcmText.h"
#include "afxwin.h"

// CvcmDlg ダイアログ
//
// ConfigGroupを与えると勝手に項目を列挙して
// Preferenceページを作ってくれる素敵なダイアログ
//
class CvcmDlg : public CDialog
{
// コンストラクション
public:
	CvcmDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_WIN32_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;
  int m_nItemID;
  CPoint m_ctrlPos;
  bool m_block_update;
  vcm::Configuration *m_pConfig;
  vcm::ConfigGroup *m_pGroup;
  std::map< CvcmCtrl *, std::string > m_ctrl2id;
  int m_nPosV, m_nPosH;

public:
  CSize m_dlgSize;
  CSize GetTextSize( const CString &str );
  void UpdateStatusMessage( const std::string &str );

  void AttachConfig( vcm::Configuration & );
  void AttachGroup( vcm::ConfigGroup & );
  void UpdateConfig( bool b );

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
