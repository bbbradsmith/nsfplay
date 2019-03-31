#if !defined(AFX_NSFVRC6Panel_H__INCLUDED_)
#define AFX_NSFVRC6Panel_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NSFVRC6Panel.h
//
#include "NSFDialog.h"
/////////////////////////////////////////////////////////////////////////////
// NSFVRC6Panel

class NSFVRC6Panel : public CDialog, public NSFDialog
{
public:
	NSFVRC6Panel(CWnd* pParent = NULL);

	//{{AFX_DATA(NSFVRC6Panel)
	enum { IDD = IDD_VRC6PANEL };
	//}}AFX_DATA

public:
	void UpdateNSFPlayerConfig(bool b);

	// ClassWizard
	//{{AFX_VIRTUAL(NSFVRC6Panel)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(NSFVRC6Panel)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}

#endif
