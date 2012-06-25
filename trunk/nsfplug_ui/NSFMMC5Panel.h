#if !defined(AFX_NSFMMC5PANEL_H__INCLUDED_)
#define AFX_NSFMMC5PANEL_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NSFMMC5Panel.h : 
//
#include "NSFDialog.h"

/////////////////////////////////////////////////////////////////////////////
// NSFMMC5Panel

class NSFMMC5Panel : public CDialog, public NSFDialog
{
public:
	NSFMMC5Panel(CWnd* pParent = NULL);

	//{{AFX_DATA(NSFMMC5Panel)
	//enum { IDD = IDD_MMC5PANEL };
	enum { IDD = 0 };
	BOOL  m_nonlinear_mixer;
	BOOL  m_phase_refresh;
	//}}AFX_DATA

public:
  void UpdateNSFPlayerConfig(bool b);

//
	// ClassWizard
	//{{AFX_VIRTUAL(NSFApuPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV
	//}}AFX_VIRTUAL

//
protected:

	//
	//{{AFX_MSG(NSFMMC5Panel)
	afx_msg void OnNonlinear();
	afx_msg void OnPhaseRefresh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++

#endif // !defined(AFX_NSFMMC5PANEL_H__INCLUDED_)
