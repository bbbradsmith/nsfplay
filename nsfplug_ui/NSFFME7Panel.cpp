// NSFFME7Panel.cpp
//

#include "stdafx.h"
#include "NSFFME7Panel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace xgm;

/////////////////////////////////////////////////////////////////////////////
// NSFFME7Panel


NSFFME7Panel::NSFFME7Panel(CWnd* pParent /*=NULL*/)
	: CDialog(NSFFME7Panel::IDD, pParent)
{
	//{{AFX_DATA_INIT(NSFFME7Panel)
	//}}AFX_DATA_INIT
}


void NSFFME7Panel::UpdateNSFPlayerConfig(bool b)
{
	NSFDialog::UpdateNSFPlayerConfig(b);

	if(!m_hWnd) return ;

	if(b)
	{
		UpdateData(FALSE);
	}
	else
	{
		UpdateData(TRUE);
		pm->cf->Notify(FME7);
	}
}

void NSFFME7Panel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NSFFME7Panel)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(NSFFME7Panel, CDialog)
	//{{AFX_MSG_MAP(NSFFME7Panel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NSFFME7Panel

BOOL NSFFME7Panel::OnInitDialog()
{
	__super::OnInitDialog();

	UpdateNSFPlayerConfig(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
}
