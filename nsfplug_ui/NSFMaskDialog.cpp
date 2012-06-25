// MaskDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "NSFMaskDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NSFMaskDialog ダイアログ


NSFMaskDialog::NSFMaskDialog(CWnd* pParent /*=NULL*/)
	: CDialog(NSFMaskDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(NSFMaskDialog)
	m_apu_2 = FALSE;
	m_apu_1 = FALSE;
	m_apu_3 = FALSE;
	m_apu_4 = FALSE;
	m_apu_5 = FALSE;
	m_fds = FALSE;
	m_fme7_1 = FALSE;
	m_fme7_2 = FALSE;
	m_fme7_3 = FALSE;
	m_mmc5_1 = FALSE;
	m_mmc5_2 = FALSE;
	m_mmc5_3 = FALSE;
	m_n106_1 = FALSE;
	m_n106_2 = FALSE;
	m_n106_3 = FALSE;
	m_n106_4 = FALSE;
	m_n106_5 = FALSE;
	m_n106_6 = FALSE;
	m_n106_7 = FALSE;
	m_n106_8 = FALSE;
	m_vrc6_1 = FALSE;
	m_vrc6_2 = FALSE;
	m_vrc6_3 = FALSE;
	m_vrc7_1 = FALSE;
	m_vrc7_2 = FALSE;
	m_vrc7_3 = FALSE;
	m_vrc7_4 = FALSE;
	m_vrc7_5 = FALSE;
	m_vrc7_6 = FALSE;
	//}}AFX_DATA_INIT
}

#define CONFIG (*(pm->cf))
void NSFMaskDialog::UpdateNSFPlayerConfig(bool b)
{
  NSFDialog::UpdateNSFPlayerConfig(b);

  if(b)
  {
    m_apu_1 = (int)CONFIG["MASK"]&0x00000001?false:true;
    m_apu_2 = (int)CONFIG["MASK"]&0x00000002?false:true;
    m_apu_3 = (int)CONFIG["MASK"]&0x00000004?false:true;
    m_apu_4 = (int)CONFIG["MASK"]&0x00000008?false:true;
    m_apu_5 = (int)CONFIG["MASK"]&0x00000010?false:true;
    m_fds   = (int)CONFIG["MASK"]&0x00000020?false:true;
    m_mmc5_1= (int)CONFIG["MASK"]&0x00000040?false:true;
    m_mmc5_2= (int)CONFIG["MASK"]&0x00000080?false:true;
    m_mmc5_3= (int)CONFIG["MASK"]&0x00000100?false:true;
    m_fme7_1= (int)CONFIG["MASK"]&0x00000200?false:true;
    m_fme7_2= (int)CONFIG["MASK"]&0x00000400?false:true;
    m_fme7_3= (int)CONFIG["MASK"]&0x00000800?false:true;
    m_vrc6_1= (int)CONFIG["MASK"]&0x00001000?false:true;
    m_vrc6_2= (int)CONFIG["MASK"]&0x00002000?false:true;
    m_vrc6_3= (int)CONFIG["MASK"]&0x00004000?false:true;
    m_vrc7_1= (int)CONFIG["MASK"]&0x00008000?false:true;
    m_vrc7_2= (int)CONFIG["MASK"]&0x00010000?false:true;
    m_vrc7_3= (int)CONFIG["MASK"]&0x00020000?false:true;
    m_vrc7_4= (int)CONFIG["MASK"]&0x00040000?false:true;
    m_vrc7_5= (int)CONFIG["MASK"]&0x00080000?false:true;
    m_vrc7_6= (int)CONFIG["MASK"]&0x00100000?false:true;
    m_n106_1= (int)CONFIG["MASK"]&0x00200000?false:true;
    m_n106_2= (int)CONFIG["MASK"]&0x00400000?false:true;
    m_n106_3= (int)CONFIG["MASK"]&0x00800000?false:true;
    m_n106_4= (int)CONFIG["MASK"]&0x01000000?false:true;
    m_n106_5= (int)CONFIG["MASK"]&0x02000000?false:true;
    m_n106_6= (int)CONFIG["MASK"]&0x04000000?false:true;
    m_n106_7= (int)CONFIG["MASK"]&0x08000000?false:true;
    m_n106_8= (int)CONFIG["MASK"]&0x10000000?false:true; 
    if (m_hWnd) UpdateData(false);
  }
  else if(m_hWnd)
  {
    UpdateData();
    CONFIG["MASK"] = !m_apu_1 |
               (!m_apu_2 <<1) |
               (!m_apu_3 <<2) |
               (!m_apu_4 <<3) |
               (!m_apu_5 <<4) |
               (!m_fds   <<5) |
               (!m_mmc5_1<<6) |
               (!m_mmc5_2<<7) |
               (!m_mmc5_3<<8) |
               (!m_fme7_1<<9) |
               (!m_fme7_2<<10) |
               (!m_fme7_3<<11) |
               (!m_vrc6_1<<12) |
               (!m_vrc6_2<<13) |
               (!m_vrc6_3<<14) |
               (!m_vrc7_1<<15) |
               (!m_vrc7_2<<16) |
               (!m_vrc7_3<<17) |
               (!m_vrc7_4<<18) |
               (!m_vrc7_5<<19) |
               (!m_vrc7_6<<20) |
               (!m_n106_1<<21) |
               (!m_n106_2<<22) |
               (!m_n106_3<<23) |
               (!m_n106_4<<24) |
               (!m_n106_5<<25) |
               (!m_n106_6<<26) |
               (!m_n106_7<<27) |
               (!m_n106_8<<28);
    pm->cf->Notify(-1);
  }

  if((int)CONFIG["MASK"]==0)
    m_reset.EnableWindow(false);
  else
    m_reset.EnableWindow(true);
}

void NSFMaskDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NSFMaskDialog)
	DDX_Control(pDX, IDC_RESET, m_reset);
	DDX_Check(pDX, IDC_APU_2, m_apu_2);
	DDX_Check(pDX, IDC_APU_1, m_apu_1);
	DDX_Check(pDX, IDC_APU_3, m_apu_3);
	DDX_Check(pDX, IDC_APU_4, m_apu_4);
	DDX_Check(pDX, IDC_APU_5, m_apu_5);
	DDX_Check(pDX, IDC_FDS, m_fds);
	DDX_Check(pDX, IDC_FME7_1, m_fme7_1);
	DDX_Check(pDX, IDC_FME7_2, m_fme7_2);
	DDX_Check(pDX, IDC_FME7_3, m_fme7_3);
	DDX_Check(pDX, IDC_MMC5_1, m_mmc5_1);
	DDX_Check(pDX, IDC_MMC5_2, m_mmc5_2);
	DDX_Check(pDX, IDC_MMC5_3, m_mmc5_3);
	DDX_Check(pDX, IDC_N106_1, m_n106_1);
	DDX_Check(pDX, IDC_N106_2, m_n106_2);
	DDX_Check(pDX, IDC_N106_3, m_n106_3);
	DDX_Check(pDX, IDC_N106_4, m_n106_4);
	DDX_Check(pDX, IDC_N106_5, m_n106_5);
	DDX_Check(pDX, IDC_N106_6, m_n106_6);
	DDX_Check(pDX, IDC_N106_7, m_n106_7);
	DDX_Check(pDX, IDC_N106_8, m_n106_8);
	DDX_Check(pDX, IDC_VRC6_1, m_vrc6_1);
	DDX_Check(pDX, IDC_VRC6_2, m_vrc6_2);
	DDX_Check(pDX, IDC_VRC6_3, m_vrc6_3);
	DDX_Check(pDX, IDC_VRC7_1, m_vrc7_1);
	DDX_Check(pDX, IDC_VRC7_2, m_vrc7_2);
	DDX_Check(pDX, IDC_VRC7_3, m_vrc7_3);
	DDX_Check(pDX, IDC_VRC7_4, m_vrc7_4);
	DDX_Check(pDX, IDC_VRC7_5, m_vrc7_5);
	DDX_Check(pDX, IDC_VRC7_6, m_vrc7_6);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(NSFMaskDialog, CDialog)
	//{{AFX_MSG_MAP(NSFMaskDialog)
	ON_BN_CLICKED(IDC_APU_1, OnApu1)
	ON_BN_CLICKED(IDC_APU_2, OnApu2)
	ON_BN_CLICKED(IDC_APU_3, OnApu3)
	ON_BN_CLICKED(IDC_APU_4, OnApu4)
	ON_BN_CLICKED(IDC_APU_5, OnApu5)
	ON_BN_CLICKED(IDC_FDS, OnFds)
	ON_BN_CLICKED(IDC_FME7_1, OnFme71)
	ON_BN_CLICKED(IDC_FME7_2, OnFme72)
	ON_BN_CLICKED(IDC_FME7_3, OnFme73)
	ON_BN_CLICKED(IDC_MMC5_1, OnMmc51)
	ON_BN_CLICKED(IDC_MMC5_2, OnMmc52)
	ON_BN_CLICKED(IDC_MMC5_3, OnMmc53)
	ON_BN_CLICKED(IDC_N106_1, OnN1061)
	ON_BN_CLICKED(IDC_N106_2, OnN1062)
	ON_BN_CLICKED(IDC_N106_3, OnN1063)
	ON_BN_CLICKED(IDC_N106_4, OnN1064)
	ON_BN_CLICKED(IDC_N106_5, OnN1065)
	ON_BN_CLICKED(IDC_N106_6, OnN1066)
	ON_BN_CLICKED(IDC_N106_7, OnN1067)
	ON_BN_CLICKED(IDC_N106_8, OnN1068)
	ON_BN_CLICKED(IDC_VRC6_1, OnVrc61)
	ON_BN_CLICKED(IDC_VRC6_2, OnVrc62)
	ON_BN_CLICKED(IDC_VRC6_3, OnVrc63)
	ON_BN_CLICKED(IDC_VRC7_1, OnVrc71)
	ON_BN_CLICKED(IDC_VRC7_2, OnVrc72)
	ON_BN_CLICKED(IDC_VRC7_3, OnVrc73)
	ON_BN_CLICKED(IDC_VRC7_4, OnVrc74)
	ON_BN_CLICKED(IDC_VRC7_5, OnVrc75)
	ON_BN_CLICKED(IDC_VRC7_6, OnVrc76)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_BN_CLICKED(IDC_REVERSE, OnReverse)
	//}}AFX_MSG_MAP
//  ON_COMMAND(IDM_MASK, OnMask)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NSFMaskDialog メッセージ ハンドラ
void NSFMaskDialog::OnApu1() 
{
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnApu2() 
{
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnApu3() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnApu4() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);	
}

void NSFMaskDialog::OnApu5() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnFds() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);	
}

void NSFMaskDialog::OnFme71() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnFme72() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnFme73() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnMmc51() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnMmc52() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnMmc53() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnN1061() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnN1062() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnN1063() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnN1064() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnN1065() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnN1066() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnN1067() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnN1068() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnVrc61() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnVrc62() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnVrc63() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnVrc71() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnVrc72() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnVrc73() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnVrc74() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnVrc75() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnVrc76() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
  UpdateNSFPlayerConfig(false);
}

void NSFMaskDialog::OnReset() 
{
  CONFIG["MASK"] = 0;
  UpdateNSFPlayerConfig(true);
  pm->cf->Notify(-1);  
}

void NSFMaskDialog::OnReverse() 
{
  CONFIG["MASK"] = (~(int)CONFIG["MASK"])&0x7FFFFFFF;
  UpdateNSFPlayerConfig(true);
  pm->cf->Notify(-1);  
}
