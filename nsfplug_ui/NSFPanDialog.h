#if !defined(AFX_NSFPANDIALOG_H__INCLUDED_)
#define AFX_NSFPANDIALOG_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NSFPanDialog.h
//
#include "NSFDialog.h"
#include "NSFPanPanel.h"

/////////////////////////////////////////////////////////////////////////////
// NSFPanDialog

class NSFPanDialog : public CDialog, public NSFDialog
{
//
public:
	NSFPanDialog(CWnd* pParent = NULL);   //

  void UpdateNSFPlayerConfig(bool b);

//
	//{{AFX_DATA(NSFPanDialog)
	enum { IDD = IDD_PANBOX };
		//
	//}}AFX_DATA


//
	// ClassWizard
	//{{AFX_VIRTUAL(NSFPanDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV
	//}}AFX_VIRTUAL

//
public:
  NSFPanPanel panel[xgm::NES_CHANNEL_MAX];
  CMenu m_Menu;
  void SetDialogManager(NSFDialogManager *p);

//
protected:

	//
	//{{AFX_MSG(NSFPanDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnVolReset();
  afx_msg void OnPanMono();
  afx_msg void OnPanRandom();
  afx_msg void OnPanSpread();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++

#endif // !defined(AFX_NSFPANDIALOG_H__INCLUDED_)
