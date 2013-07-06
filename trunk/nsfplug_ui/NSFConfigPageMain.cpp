// NSFConfigPageMain.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "NSFConfigPageMain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NSFConfigPageMain プロパティ ページ

IMPLEMENT_DYNCREATE(NSFConfigPageMain, CPropertyPage)

NSFConfigPageMain::NSFConfigPageMain() : CPropertyPage(NSFConfigPageMain::IDD)
, m_bUseAlt(FALSE)
, m_bVsync(FALSE)
, m_nRegion(0)
, m_bStereo(FALSE)
, m_bNSFePlaylist(FALSE)
, m_nLimit(0)
, m_nThreshold(0)
, m_nVelocity(0)
, m_nHpfValue(0)
, m_nLpfValue(0)
{
	//{{AFX_DATA_INIT(NSFConfigPageMain)
	m_bAutoStop = FALSE;
	m_nStopSec = 0;
	m_nFadeTime = 0;
	m_nPlayTime = 0;
	m_nPlayFreq = -1;
	m_bAutoDetect = FALSE;
	m_nDetectTime = 0;
	m_bUpdatePlaylist = FALSE;
	m_nLoopNum = 0;
	m_bMaskInit = FALSE;
	m_format = _T("");
	//}}AFX_DATA_INIT
}

NSFConfigPageMain::~NSFConfigPageMain()
{
}

void NSFConfigPageMain::UpdateNSFPlayerConfig(bool b)
{
  int rate[] = {8000,11025,16000,22050,32000,44100,48000,88200,96000,0x7FFFFFFF};

  NSFDialog::UpdateNSFPlayerConfig(b);

  if(b)
  {
    int i;
    for(i = 0;rate[i]<CONFIG["RATE"];i++);
    m_nPlayFreq   = i;
    m_bAutoStop   = CONFIG["AUTO_STOP"];
    m_nStopSec    = CONFIG["STOP_SEC"];
    m_nFadeTime   = CONFIG["FADE_TIME"]/1000;
    m_nPlayTime   = CONFIG["PLAY_TIME"]/1000;
    m_nLoopNum    = CONFIG["LOOP_NUM"];
    m_bAutoDetect = CONFIG["AUTO_DETECT"];
    m_nDetectTime = CONFIG["DETECT_TIME"]/1000;
    m_bUpdatePlaylist = CONFIG["UPDATE_PLAYLIST"];
    m_nHpfValue = 256 - CONFIG["HPF"];
    m_nLpfValue = CONFIG["LPF"];
    m_bMaskInit = CONFIG["MASK_INIT"];
    m_format = CString(CONFIG["TITLE_FORMAT"]);
    m_bUseAlt = !(int)CONFIG["DETECT_ALT"];
    m_bVsync = CONFIG["VSYNC_ADJUST"];
    m_nRegion = CONFIG["REGION"];
    m_bStereo = (CONFIG["NCH"] == 2);
    m_bNSFePlaylist = CONFIG["NSFE_PLAYLIST"];
    m_nLimit = 100 - CONFIG["COMP_LIMIT"];
    m_nThreshold = 100 - CONFIG["COMP_THRESHOLD"];
    m_nVelocity = 100 - CONFIG["COMP_VELOCITY"];
    
    if(m_hWnd) UpdateData(FALSE);
  }
  else
  {
    if(m_hWnd) UpdateData(TRUE);
    if(0<=m_nPlayFreq&&m_nPlayFreq<sizeof(rate))
      CONFIG["RATE"] = rate[m_nPlayFreq];

    CONFIG["AUTO_STOP"] = m_bAutoStop;
    CONFIG["STOP_SEC"]  = m_nStopSec;
    CONFIG["FADE_TIME"] = m_nFadeTime * 1000;
    CONFIG["PLAY_TIME"] = m_nPlayTime * 1000;
    CONFIG["LOOP_NUM"]  = m_nLoopNum;
    CONFIG["AUTO_DETECT"] = m_bAutoDetect;
    CONFIG["DETECT_TIME"] = m_nDetectTime * 1000;
    CONFIG["UPDATE_PLAYLIST"] = m_bUpdatePlaylist;
    CONFIG["HPF"]   = 256 - m_nHpfValue;
    CONFIG["LPF"]   = m_nLpfValue;
    CONFIG["MASK_INIT"]    = m_bMaskInit;
    CONFIG["DETECT_ALT"]   = !m_bUseAlt;
    CONFIG["TITLE_FORMAT"] = m_format;
    CONFIG["VSYNC_ADJUST"] = m_bVsync;
    CONFIG["REGION"] = m_nRegion;
    CONFIG["NCH"] = m_bStereo ? 2 : 1;
    CONFIG["NSFE_PLAYLIST"] = m_bNSFePlaylist;
    CONFIG["COMP_LIMIT"] = 100 - m_nLimit;
    CONFIG["COMP_THRESHOLD"] = 100 - m_nThreshold;
    CONFIG["COMP_VELOCITY"] = 100 - m_nVelocity;

    pm->cf->Notify(-1);
  }

}

void NSFConfigPageMain::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(NSFConfigPageMain)
  DDX_Check(pDX, IDC_AUTOSTOP, m_bAutoStop);
  DDX_Text(pDX, IDC_STOPSEC, m_nStopSec);
  DDV_MinMaxUInt(pDX, m_nStopSec, 1, 30);
  DDX_Text(pDX, IDC_FADETIME, m_nFadeTime);
  DDV_MinMaxUInt(pDX, m_nFadeTime, 0, 32767);
  DDX_Text(pDX, IDC_PLAYTIME, m_nPlayTime);
  DDV_MinMaxUInt(pDX, m_nPlayTime, 0, 32767);
  DDX_CBIndex(pDX, IDC_PLAYFREQ, m_nPlayFreq);
  DDX_Check(pDX, IDC_AUTODETECT, m_bAutoDetect);
  DDX_Text(pDX, IDC_DETECTTIME, m_nDetectTime);
  DDV_MinMaxUInt(pDX, m_nDetectTime, 0, 32767);
  DDX_Check(pDX, IDC_UPDATE_PLAYLIST, m_bUpdatePlaylist);
  DDX_Text(pDX, IDC_LOOPNUM, m_nLoopNum);
  DDV_MinMaxUInt(pDX, m_nLoopNum, 1, 256);
  DDX_Check(pDX, IDC_MASKINIT, m_bMaskInit);
  DDX_Text(pDX, IDC_FORMAT, m_format);
  DDV_MaxChars(pDX, m_format, 128);
  //}}AFX_DATA_MAP
  DDX_Check(pDX, IDC_USEALT, m_bUseAlt);
  DDX_Check(pDX, IDC_VSYNC, m_bVsync);
  DDX_Check(pDX, IDC_PREF_PAL, m_nRegion); // TODO change UI
  DDX_Check(pDX, IDC_STEREO, m_bStereo);
  DDX_Check(pDX, IDC_NSFEPLS, m_bNSFePlaylist);
  DDX_Control(pDX, IDC_LIMIT, m_limitCtrl);
  DDX_Control(pDX, IDC_THRESHOLD, m_threshCtrl);
  DDX_Control(pDX, IDC_VELOCITY, m_velocityCtrl);
  DDX_Slider(pDX, IDC_LIMIT, m_nLimit);
  DDX_Slider(pDX, IDC_THRESHOLD, m_nThreshold);
  DDX_Slider(pDX, IDC_VELOCITY, m_nVelocity);
  DDX_Control(pDX, IDC_HPF, m_hpfCtrl);
  DDX_Slider(pDX, IDC_HPF, m_nHpfValue);
  DDX_Control(pDX, IDC_LPF, m_lpfCtrl);
  DDX_Slider(pDX, IDC_LPF, m_nLpfValue);
}

BEGIN_MESSAGE_MAP(NSFConfigPageMain, CPropertyPage)
	//{{AFX_MSG_MAP(NSFConfigPageMain)
	ON_CBN_SELCHANGE(IDC_PLAYFREQ, OnSelchangePlayfreq)
	ON_EN_CHANGE(IDC_STOPSEC, OnChangeStopsec)
	ON_EN_CHANGE(IDC_PLAYTIME, OnChangePlaytime)
	ON_EN_CHANGE(IDC_FADETIME, OnChangeFadetime)
	ON_BN_CLICKED(IDC_AUTOSTOP, OnAutostop)
	ON_BN_CLICKED(IDC_AUTODETECT, OnAutodetect)
	ON_EN_CHANGE(IDC_DETECTTIME, OnChangeDetecttime)
	ON_BN_CLICKED(IDC_UPDATE_PLAYLIST, OnUpdatePlaylist)
	ON_EN_CHANGE(IDC_LOOPNUM, OnChangeLoopnum)
	ON_BN_CLICKED(IDC_MASKINIT, OnMaskinit)
  ON_BN_CLICKED(IDC_USEALT, OnBnClickedUsealt)
  ON_BN_CLICKED(IDC_VSYNC, OnBnClickedVsync)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NSFConfigPageMain メッセージ ハンドラ

BOOL NSFConfigPageMain::OnApply() 
{
	UpdateNSFPlayerConfig(FALSE);
	return CPropertyPage::OnApply();
}

void NSFConfigPageMain::OnSelchangePlayfreq() 
{
	//SetModified(true);
}

void NSFConfigPageMain::OnChangeStopsec() 
{
	//SetModified(true);
}

void NSFConfigPageMain::OnChangePlaytime() 
{
	//SetModified(true);
}

void NSFConfigPageMain::OnChangeFadetime() 
{
	//SetModified(true);
}

void NSFConfigPageMain::OnAutostop() 
{
	//SetModified(true);
}

void NSFConfigPageMain::OnAutodetect() 
{
	//SetModified(true);
}

void NSFConfigPageMain::OnChangeDetecttime() 
{
	//SetModified(true);
}

void NSFConfigPageMain::OnUpdatePlaylist() 
{
	//SetModified(true);
}

void NSFConfigPageMain::OnChangeLoopnum() 
{
	//SetModified(true);
}

void NSFConfigPageMain::OnMaskinit() 
{
	//SetModified(true);
}

void NSFConfigPageMain::OnBnClickedUsealt()
{
  //SetModified(true);
}

void NSFConfigPageMain::OnBnClickedVsync()
{
  //SetModified(true);
}

BOOL NSFConfigPageMain::OnInitDialog()
{
  __super::OnInitDialog();

  m_limitCtrl.SetRange(0,100);
  m_limitCtrl.SetPageSize(5);
  m_limitCtrl.SetLineSize(1);
  m_limitCtrl.SetTicFreq(10);
  m_threshCtrl.SetRange(0,100);
  m_threshCtrl.SetPageSize(5);
  m_threshCtrl.SetTicFreq(10);
  m_threshCtrl.SetLineSize(1);
  m_velocityCtrl.SetRange(0,100);
  m_velocityCtrl.SetTicFreq(10);
  m_velocityCtrl.SetPageSize(5);
  m_velocityCtrl.SetLineSize(1);
  m_hpfCtrl.SetRange(0,256);
  m_hpfCtrl.SetTicFreq(32);
  m_hpfCtrl.SetPageSize(16);
  m_hpfCtrl.SetLineSize(1);
  m_lpfCtrl.SetRange(0,400);
  m_lpfCtrl.SetTicFreq(50);
  m_lpfCtrl.SetPageSize(25);
  m_lpfCtrl.SetLineSize(1);

  return TRUE;  // return TRUE unless you set the focus to a control
  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
