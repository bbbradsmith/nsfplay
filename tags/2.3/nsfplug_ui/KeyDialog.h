#pragma once

#include "KeyWindow.h"
#include "KeyHeader.h"

// KeyDialog ダイアログ

class KeyDialog : public CDialog
{
	DECLARE_DYNAMIC(KeyDialog)

public:
	KeyDialog(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~KeyDialog();
  
  // 描画開始
  virtual void Start(int);
  // 描画停止
  virtual void Stop();

  virtual void Reset();

  inline int MinWidth(){ return m_keywindow.MinWidth(); }
  inline int MaxWidth(){ return m_keywindow.MaxWidth(); }

  // 鍵盤部分
  KeyWindow m_keywindow;

  // ヘッダ部分
  KeyHeader m_keyheader;

  bool m_bInit;
  UINT_PTR m_nTimer;

// ダイアログ データ
	enum { IDD = IDD_KEYDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnPaint();
  virtual BOOL OnInitDialog();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnTimer(UINT nIDEvent);
};
