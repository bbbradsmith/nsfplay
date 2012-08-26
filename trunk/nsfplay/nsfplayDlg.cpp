/*
 * nsfplayDlg.cpp.h written by Mitsutaka Okazaki 2004.
 *
 * This software is provided 'as-is', without any express or implied warranty. 
 * In no event will the authors be held liable for any damages arising from 
 * the use of this software. 
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it 
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not 
 *    claim that you wrote the original software. If you use this software 
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not 
 *    be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 */
#include "stdafx.h"
#include "nsfplay.h"
#include "nsfplayDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


CnsfplayDlg::CnsfplayDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CnsfplayDlg::IDD, pParent)
    , m_cancel_open(false)
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

  char buf[MAX_PATH];
  GetModuleFileName(AfxGetApp()->m_hInstance, m_IniPath, MAX_PATH);
  strrchr(m_IniPath,'\\')[1] = '\0';
  strcat(m_IniPath,"nsfplay.ini");
  GetPrivateProfileString("NSFPLAY","PLUGIN","plugins\\in_yansf.dll",buf,MAX_PATH,m_IniPath);
  m_emu = new EmuWinamp(buf);
}

CnsfplayDlg::~CnsfplayDlg()
{
  delete m_emu;
}

void CnsfplayDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_PLAYTIME, m_timebox);
  DDX_Control(pDX, IDC_SLIDER, m_slider);
  DDX_Control(pDX, IDC_TITLE, m_title);
  DDX_Control(pDX, IDC_TIME_MAX, m_time_max);
  DDX_Control(pDX, IDC_OPEN, m_open_btn);
  DDX_Control(pDX, IDC_INFO, m_prop_btn);
  DDX_Control(pDX, IDC_VOLUME, m_volume);
}

BEGIN_MESSAGE_MAP(CnsfplayDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_DROPFILES()
    ON_BN_CLICKED(IDC_PREV, OnBnClickedPrev)
    ON_BN_CLICKED(IDC_NEXT, OnBnClickedNext)
    ON_BN_CLICKED(IDC_INFO, OnBnClickedInfo)
    ON_WM_TIMER()
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_STOP, OnBnClickedStop)
    ON_BN_CLICKED(IDC_PLAY, OnBnClickedPlay)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_PAUSE, OnBnClickedPause)
    ON_BN_CLICKED(IDC_OPEN, OnBnClickedOpen)
    ON_BN_CLICKED(IDC_CONFIG, OnBnClickedConfig)
    ON_BN_CLICKED(IDC_WAVEOUT, OnBnClickedWaveout)
END_MESSAGE_MAP()


BOOL CnsfplayDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_last_title[0] = 0; // BS this was uninitialized

    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    ((CButton *)GetDlgItem(IDC_OPEN))->SetIcon((HICON)::LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_OPEN),IMAGE_ICON,16,16,LR_DEFAULTCOLOR));
    ((CButton *)GetDlgItem(IDC_INFO))->SetIcon((HICON)::LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_PROP),IMAGE_ICON,16,16,LR_DEFAULTCOLOR));
    ((CButton *)GetDlgItem(IDC_PREV))->SetIcon((HICON)::LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_PREV),IMAGE_ICON,16,16,LR_DEFAULTCOLOR));
    ((CButton *)GetDlgItem(IDC_NEXT))->SetIcon((HICON)::LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_NEXT),IMAGE_ICON,16,16,LR_DEFAULTCOLOR));
    ((CButton *)GetDlgItem(IDC_PLAY))->SetIcon((HICON)::LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_PLAY),IMAGE_ICON,16,16,LR_DEFAULTCOLOR));
    ((CButton *)GetDlgItem(IDC_STOP))->SetIcon((HICON)::LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_STOP),IMAGE_ICON,16,16,LR_DEFAULTCOLOR));
    ((CButton *)GetDlgItem(IDC_PAUSE))->SetIcon((HICON)::LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_PAUSE),IMAGE_ICON,16,16,LR_DEFAULTCOLOR));
    ((CButton *)GetDlgItem(IDC_CONFIG))->SetIcon((HICON)::LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_CONFIG),IMAGE_ICON,16,16,LR_DEFAULTCOLOR));
    ((CButton *)GetDlgItem(IDC_WAVEOUT))->SetIcon((HICON)::LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_WAVEOUT),IMAGE_ICON,16,16,LR_DEFAULTCOLOR));

	m_timerID = SetTimer(1,100,NULL);
    m_slider.SetRange(0,60*5,TRUE);
    m_slider.SetTicFreq(10);
    m_sb_dragging = false;
    m_volume.SetRange(0,255,TRUE);
    int v = GetPrivateProfileInt("NSFPLAY","VOLUME",255,m_IniPath);
    m_volume.SetPos(v);
    m_volume.SetTicFreq(32);
    m_emu->SetVolume(v);
    
    if(m_init_file.GetLength()) {
      m_emu->Play(m_init_file.GetBuffer());
      m_update_wait = 0;
    }

    if (m_cancel_open)
    {
        EndDialog(0);
    }

    // for debugging
    char cWndName[256];
    ::GetWindowText(m_hWnd, cWndName, sizeof(cWndName));
    ::OutputDebugString("Window name: ");
    ::OutputDebugString(cWndName);
    ::OutputDebugString("\n");

    return TRUE;
}

void CnsfplayDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CnsfplayDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CnsfplayDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CnsfplayDlg::OnDropFiles(HDROP hDropInfo)
{
  UINT nCount, nSize;
  CArray <char, int> aryFile;
  CSize size;

  nCount = DragQueryFile(hDropInfo, -1, NULL, 0);
  if(nCount==1) {
    nSize = DragQueryFile(hDropInfo, 0, NULL, 0);
    aryFile.SetSize(nSize+1);
    DragQueryFile(hDropInfo, 0, aryFile.GetData(), nSize+1);
    m_emu->Play(aryFile.GetData());
    m_last_len = -1;
    m_update_wait = 0;
  }

  CDialog::OnDropFiles(hDropInfo);
}

void CnsfplayDlg::OnBnClickedPrev()
{
  m_emu->Prev();
  m_last_len = -1;
  m_update_wait = 0;
}

void CnsfplayDlg::OnBnClickedNext()
{
  m_emu->Next();
  m_last_len = -1;
  m_update_wait = 0;
}

void CnsfplayDlg::OnBnClickedConfig()
{
  m_emu->Config(m_hWnd);
}

void CnsfplayDlg::OnBnClickedWaveout()
{
  OnBnClickedStop();

  char title[1024];
  strcpy(title,m_last_title);
  for (char* t = title; *t; ++t)
  {
      if (*t == '<' ||
          *t == '>' ||
          *t == ':' ||
          *t == '/' ||
          *t == '\\' ||
          *t == '|' ||
          *t == '?' ||
          *t == '*')
      {
          *t = '.';
      }
  }

  CFileDialog fd(FALSE,".wav",title,OFN_HIDEREADONLY,"WAV files (*.wav)|*.wav|All files (*.*)|*.*||",this);
  if(fd.DoModal()==IDOK) {
    m_emu->Waveout(fd.GetPathName().GetBuffer());
    OnBnClickedPlay();
  }
}

void CnsfplayDlg::OnBnClickedInfo()
{
  m_emu->Info(m_hWnd);
}

void CnsfplayDlg::OnTimer(UINT nIDEvent)
{
  int t = m_emu->GetOutputTime() / 1000;
  int mt = m_slider.GetRangeMax();
  if(mt<t) t = mt;
  CString str;

  if(m_emu->IsPlaying()) {
    if(m_last_time!=t) {
      str.Format("%02d:%02d",t/60,t%60);
      m_timebox.SetWindowText(str);
      if(!m_sb_dragging) m_slider.SetPos(t);
      m_last_time = t;
    }
    if(m_update_wait==0) UpdateInfo();
    m_update_wait = (m_update_wait+1)%10;
  }
  
  CDialog::OnTimer(nIDEvent);
}

void CnsfplayDlg::OnDestroy()
{
  m_emu->Stop();

  CDialog::OnDestroy();

  if(m_timerID) {
    KillTimer(m_timerID);
    m_timerID = 0;
  }

  CString str;
  str.Format("%d",m_volume.GetPos());
  WritePrivateProfileString("NSFPLAY","VOLUME",str,m_IniPath);
}

void CnsfplayDlg::OnBnClickedStop()
{
  m_timebox.SetWindowText("00:00");
  m_slider.SetPos(0);
  m_emu->Stop();
}

void CnsfplayDlg::OnBnClickedPlay()
{
  if (m_emu->IsPaused())
  {
    OnBnClickedPause();
    return;
  }

  m_emu->Play(NULL);
  m_last_len = -1;
  m_update_wait = 0;
}

void CnsfplayDlg::UpdateInfo()
{
  CString str;
  char title[1024];
  int len;
  m_emu->GetFileInfo(NULL,title,&len);
  len/=1000;
  if(m_last_len!=len) {
    m_slider.SetRangeMax(len);
    str.Format("%02d:%02d",len/60,len%60);
    m_time_max.SetWindowText(str);
    m_last_len = len;
  }
  if(strcmp(m_last_title,title)!=0) {
    m_title.SetWindowText(title);
    strcpy(m_last_title,title);
  }

}

void CnsfplayDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  if(&m_slider==(CSliderCtrl *)pScrollBar) {
    if(nSBCode==SB_ENDSCROLL) {
      if(m_emu->IsPlaying())
        m_emu->SetOutputTime(m_slider.GetPos()*1000);
      m_sb_dragging = false;
    } else {
      m_sb_dragging = true;
      int t = m_slider.GetPos();
      CString str;
      str.Format("%02d:%02d",t/60,t%60);
      m_timebox.SetWindowText(str);
    }
  } else if(&m_volume==(CSliderCtrl *)pScrollBar) {
    m_emu->SetVolume(m_volume.GetPos());
  }
  CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CnsfplayDlg::OnBnClickedPause()
{
  m_emu->Pause();
}

void CnsfplayDlg::OnBnClickedOpen()
{
  CFileDialog fd(TRUE,".nsf;.nsfe",0,OFN_FILEMUSTEXIST|OFN_HIDEREADONLY,"NSF files (*.nsf;*.nsfe)|*.nsf;*.nsfe|All files (*.*)|*.*||",this);
  if(fd.DoModal()==IDOK) {
    if (m_emu->Play(fd.GetPathName().GetBuffer()))
    {
      MessageBox("Unable to read file.","Error reading file!",MB_ICONEXCLAMATION | MB_OK);
    }
    m_last_len = -1;
    m_update_wait = 0;
  }
}

// BS command line wave output
int CnsfplayDlg::WriteSingleWave(char* nsf_file, char* wave_file, char* track, char* ms)
{
    int it = ::atoi(track);
    int ims = ::atoi(ms);

    int last_pos = 0;
    int hang_count = 0;

    const int SLEEP_TIME = 5;
    const int HANG_TIME = 3000;

    m_emu->Play(nsf_file);
    m_emu->Stop();
    for (int t=1; t < it; ++t)
    {
        m_emu->Next();
        m_emu->Stop();
    }
    m_emu->Waveout(wave_file);
    m_emu->Play(NULL);
    do
    {
        ::Sleep(SLEEP_TIME);
        if (!m_emu->IsPlaying()) break;
        int iot = m_emu->GetOutputTime();
        if (iot >= ims) break;

        if (last_pos != iot)
        {
            hang_count = 0;
        }
        else
        {
            ++hang_count;
            if (hang_count > (HANG_TIME/SLEEP_TIME)) break;
        }
        last_pos = iot;
    }
    while (true);
    m_emu->Stop();

    return 0;
}
