#if !defined(AFX_NSFFME7Panel_H__INCLUDED_)
#define AFX_NSFFME7Panel_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NSFFME7Panel.h
//
#include "NSFDialog.h"
/////////////////////////////////////////////////////////////////////////////
// NSFFME7Panel

class NSFFME7Panel : public CDialog, public NSFDialog
{
public:
	NSFFME7Panel(CWnd* pParent = NULL);

	//{{AFX_DATA(NSFFME7Panel)
	enum { IDD = IDD_FME7PANEL };
	//}}AFX_DATA

public:
	void UpdateNSFPlayerConfig(bool b);

	// ClassWizard
	//{{AFX_VIRTUAL(NSFFME7Panel)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(NSFFME7Panel)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}

#endif
