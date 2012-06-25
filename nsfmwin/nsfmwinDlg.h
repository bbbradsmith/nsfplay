// nsfmwinDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "OptionDlg.h"
#include <set>

// CnsfmwinDlg ダイアログ
class CnsfmwinDlg : public CDialog
{
// コンストラクション
public:
	CnsfmwinDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_NSFMWIN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート

// 実装
protected:
	HICON m_hIcon;
  CWinThread *m_pThread;
  void StartFileFindThread();

public:
  BOOL m_bStopflag;
  BOOL m_bBtnMode;
  std::set<CString> m_setTarget;
  int m_nPriority, m_nSearchLen, m_nMinLoop, m_nUseAlt;
  int m_nListCx;
  void EnableDlgItem(BOOL bEnable);

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnDropFiles(HDROP hDropInfo);
  CListBox m_listbox;
  afx_msg void OnBnClickedDelete();
  afx_msg void OnBnClickedUp();
  afx_msg void OnBnClickedDown();
  afx_msg void OnBnClickedStart();
  CProgressCtrl m_progress2;
  CButton m_start;
  CButton m_delete;
  CButton m_up;
  CButton m_down;
  CProgressCtrl m_progress1;
  CStatic m_track;
  CStatic m_file;
  CStatic m_total;
  CProgressCtrl m_progress0;
  afx_msg void OnBnClickedOption();
  afx_msg void OnDestroy();
  CButton m_option;
};
