#pragma once
#include "afxcmn.h"


// COptionDlg ダイアログ

class COptionDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionDlg)

public:
	COptionDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~COptionDlg();

// ダイアログ データ
	enum { IDD = IDD_OPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
  int m_priority;
  CSliderCtrl m_prioCtrl;
  virtual BOOL OnInitDialog();
  int m_length;
  int m_minloop;
  BOOL m_usealt;
};
