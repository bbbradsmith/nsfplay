#if !defined(AFX_NSFPANPANEL_H__INCLUDED_)
#define AFX_NSFPANPANEL_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NSFPanPanel.h
//
#include "NSFDialog.h"

/////////////////////////////////////////////////////////////////////////////
// NSFPanPanel

class NSFPanPanel : public CDialog, public NSFDialog
{
//
public:
	NSFPanPanel(CWnd* pParent = NULL);   //

  int channel_id;
  void SetChannelID(int id);
  void SetVolume(int i);
  int GetVolume();
  void SetPan(int i);
  int GetPan();

  void UpdateNSFPlayerConfig(bool b);

//
	//{{AFX_DATA(NSFPanPanel)
	enum { IDD = IDD_PAN };
	CButton	m_name;
	CSliderCtrl	m_span;
	CSliderCtrl	m_svol;
	//}}AFX_DATA


//
	// ClassWizard
	//{{AFX_VIRTUAL(NSFPanPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV
	//}}AFX_VIRTUAL

//
protected:

	//
	//{{AFX_MSG(NSFPanPanel)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++

#endif // !defined(AFX_NSFPANPANEL_H__INCLUDED_)
