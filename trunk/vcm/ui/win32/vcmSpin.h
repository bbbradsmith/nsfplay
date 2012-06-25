#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CvcmSpin ダイアログ

class CvcmSpin : public CvcmCtrl
{
	DECLARE_DYNAMIC(CvcmSpin)

public:
  vcm::VT_SPIN *m_vt;
  CvcmSpin(const std::string &id, vcm::VT_SPIN *vt, CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CvcmSpin();
  void ReadWork();
  void WriteWork();

// ダイアログ データ
	enum { IDD = IDD_SPIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  CStatic m_label;
  CEdit m_edit;
  CSpinButtonCtrl m_spin;
  afx_msg void OnEnKillfocusEdit();
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
