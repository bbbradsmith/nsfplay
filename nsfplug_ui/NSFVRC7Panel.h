#if !defined(AFX_NSFVRC7Panel_H__INCLUDED_)
#define AFX_NSFVRC7Panel_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NSFVRC7Panel.h
//
#include "NSFDialog.h"
/////////////////////////////////////////////////////////////////////////////
// NSFVRC7Panel

class NSFVRC7Panel : public CDialog, public NSFDialog
{
public:
	NSFVRC7Panel(CWnd* pParent = NULL);

	//{{AFX_DATA(NSFVRC7Panel)
	enum { IDD = IDD_VRC7PANEL };
	BOOL m_opll;
	//}}AFX_DATA

public:
	void UpdateNSFPlayerConfig(bool b);

	// ClassWizard
	//{{AFX_VIRTUAL(NSFVRC7Panel)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(NSFVRC7Panel)
	afx_msg void OnOPLL();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}

#endif
