// NSFVRC6Panel.cpp
//

#include "stdafx.h"
#include "NSFVRC6Panel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace xgm;

/////////////////////////////////////////////////////////////////////////////
// NSFVRC6Panel


NSFVRC6Panel::NSFVRC6Panel(CWnd* pParent /*=NULL*/)
	: CDialog(NSFVRC6Panel::IDD, pParent)
{
	//{{AFX_DATA_INIT(NSFVRC6Panel)
	//}}AFX_DATA_INIT
}


void NSFVRC6Panel::UpdateNSFPlayerConfig(bool b)
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
		pm->cf->Notify(VRC6);
	}
}

void NSFVRC6Panel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NSFVRC6Panel)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(NSFVRC6Panel, CDialog)
	//{{AFX_MSG_MAP(NSFVRC6Panel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NSFVRC6Panel

BOOL NSFVRC6Panel::OnInitDialog()
{
	__super::OnInitDialog();

	UpdateNSFPlayerConfig(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
}
