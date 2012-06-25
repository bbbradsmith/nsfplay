#pragma once
#include "afxwin.h"


// CvcmCombo ダイアログ

class CvcmCombo : public CvcmCtrl
{
	DECLARE_DYNAMIC(CvcmCombo)

public:
  vcm::VT_COMBO *m_vt;
  CvcmCombo(const std::string &id, vcm::VT_COMBO *vt, CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CvcmCombo();
  void WriteWork();
  void ReadWork();

// ダイアログ データ
	enum { IDD = IDD_COMBO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  CStatic m_label;
  CComboBox m_combo;
  afx_msg void OnCbnSelchangeCombo();
};
