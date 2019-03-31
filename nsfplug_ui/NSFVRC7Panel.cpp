// NSFVRC7Panel.cpp
//

#include "stdafx.h"
#include "NSFVRC7Panel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace xgm;

/////////////////////////////////////////////////////////////////////////////
// NSFVRC7Panel


NSFVRC7Panel::NSFVRC7Panel(CWnd* pParent /*=NULL*/)
	: CDialog(NSFVRC7Panel::IDD, pParent)
{
	//{{AFX_DATA_INIT(NSFVRC7Panel)
	m_opll = FALSE;
	//}}AFX_DATA_INIT
}


void NSFVRC7Panel::UpdateNSFPlayerConfig(bool b)
{
	NSFDialog::UpdateNSFPlayerConfig(b);

	if(!m_hWnd) return ;

	if(b)
	{
		m_opll = pm->cf->GetDeviceOption(VRC7,NES_VRC7::OPT_OPLL).GetInt();
		UpdateData(FALSE);
	}
	else
	{
		UpdateData(TRUE);
		pm->cf->GetDeviceOption(VRC7,NES_VRC7::OPT_OPLL) = m_opll;
		pm->cf->Notify(VRC7);
	}
}

void NSFVRC7Panel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NSFVRC7Panel)
	DDX_Check(pDX, IDC_OPLL, m_opll);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(NSFVRC7Panel, CDialog)
	//{{AFX_MSG_MAP(NSFVRC7Panel)
	ON_BN_CLICKED(IDC_OPLL, OnOPLL)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NSFVRC7Panel

void NSFVRC7Panel::OnOPLL()
{
	//SetModified(true);
}

BOOL NSFVRC7Panel::OnInitDialog()
{
	__super::OnInitDialog();

	UpdateNSFPlayerConfig(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
}
