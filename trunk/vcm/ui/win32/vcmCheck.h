#pragma once
#include "afxwin.h"


// CvcmCheck ダイアログ

class CvcmCheck : public CvcmCtrl
{
	DECLARE_DYNAMIC(CvcmCheck)

public:
  vcm::VT_CHECK *m_vt;
  CvcmCheck(const std::string &id, vcm::VT_CHECK *vt, CWnd* pParent=NULL );
	virtual ~CvcmCheck();

  void ReadWork();
  void WriteWork();

// ダイアログ データ
	enum { IDD = IDD_CHECK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  CButton m_check;
  afx_msg void OnBnClickedCheck();
};
