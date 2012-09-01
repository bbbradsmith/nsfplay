#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CvcmSlider ダイアログ

class CvcmSlider : public CvcmCtrl
{
	DECLARE_DYNAMIC(CvcmSlider)

public:
  vcm::VT_SLIDER *m_vt;
  CvcmSlider(const std::string &id, vcm::VT_SLIDER *vt, CWnd* pParent=NULL );
	virtual ~CvcmSlider();
  void ReadWork();
  void WriteWork();

// ダイアログ データ
	enum { IDD = IDD_SLIDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  CStatic m_label;
  CStatic m_min;
  CSliderCtrl m_slider;
  CStatic m_max;
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
