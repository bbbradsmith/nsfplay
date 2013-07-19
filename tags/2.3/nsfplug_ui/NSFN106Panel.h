#if !defined(AFX_NSFN106PANEL_H__FE81618A_15CD_40A7_B503_C43DA1BFC0D7__INCLUDED_)
#define AFX_NSFN106PANEL_H__FE81618A_15CD_40A7_B503_C43DA1BFC0D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NSFDialog.h"
/////////////////////////////////////////////////////////////////////////////
// NSFN106Panel

class NSFN106Panel : public CDialog, public NSFDialog
{
public:
	NSFN106Panel(CWnd* pParent = NULL);

	//{{AFX_DATA(NSF106Panel)
	enum { IDD = IDD_N106PANEL };
	BOOL	m_serial;
	//}}AFX_DATA

public:
	void UpdateNSFPlayerConfig(bool b);

	// ClassWizard
	//{{AFX_VIRTUAL(NSFN106Panel)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(NSFN106Panel)
	afx_msg void OnSerial();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++

#endif // !defined(AFX_NSFN106PANEL_H__FE81618A_15CD_40A7_B503_C43DA1BFC0D7__INCLUDED_)
