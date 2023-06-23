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
#include "../xgm/fileutil.h" // file_utf8
#include "../in_yansf/direct.h"

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


CnsfplayDlg::CnsfplayDlg(CWnd* pParent, int wargc, const wchar_t* const * wargv)
    : CDialog(CnsfplayDlg::IDD, pParent)
    , m_cancel_open(false)
    , in_yansf(NULL)
    , m_volume_init(-1)
    , m_volume_save(-1)
    , m_ini_save(true)
    , m_plugin_path("")
    , m_yansf_ini_path("")
    , m_nsfplay_ini_path("")
{
  m_hIcon = AfxGetApp()->LoadIcon(IDI_MAINFRAME);

  ParseArgs(wargc,wargv,true); // prepass to get nsfplay.ini overrides

  // find nsfplay.ini (or override)
  char buf[1024+16];
  if (m_nsfplay_ini_path.length() == 0)
  {
    const char* INI_FILENAME = "nsfplay.ini";
    
    GetModuleFileName(AfxGetApp()->m_hInstance, buf, 1024);
    strrchr(buf,'\\')[1] = '\0';
    strcat(buf,INI_FILENAME);
    if (FALSE == PathFileExists(buf))
    {
      // first try to see if we have permission to create a new one in that location...
      HANDLE f = CreateFile(buf,GENERIC_READ,FILE_SHARE_READ,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
      if (f != INVALID_HANDLE_VALUE)
      {
        CloseHandle(f);
      }
      else  // Otherwise, attempt to use AppData instead.
      {
        if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, buf)))
        {
          strcat(buf,"\\NSFPlay");
          CreateDirectory(buf,NULL);
          strcat(buf,"\\");
          strcat(buf,INI_FILENAME);
        }
      }
    }
    m_nsfplay_ini_path = buf;
  }
  else
  {
    GetFullPathName(m_nsfplay_ini_path.c_str(),1024+16,buf,NULL);
    m_nsfplay_ini_path = buf;
    if (FALSE == PathFileExists(buf)) // try to create if needed
    {
      HANDLE f = CreateFile(buf,GENERIC_READ,FILE_SHARE_READ,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
      if (f != INVALID_HANDLE_VALUE) CloseHandle(f);
    }
  }

  // find plugin (or override)
  if (m_plugin_path.length() == 0)
  {
    GetPrivateProfileString("NSFPLAY","PLUGIN","plugins\\in_yansf.dll",buf,1024,m_nsfplay_ini_path.c_str());
    m_plugin_path = buf;
  }

  // open plugin, override in_yansf.ini
  try
  {
      strcpy(buf,m_plugin_path.c_str());
      m_emu = new EmuWinamp(buf,
         (m_yansf_ini_path.length()==0) ? NULL : m_yansf_ini_path.c_str());
  }
  catch (int e)
  {
     char msg[1024+256];
     strcpy(msg,"Unable to open plugin:\n");
     if (e == 2) strcpy(msg,"Not a Winamp plugin:\n");
     strcat(msg,buf);
     printf(msg,buf); printf("\n");
     ::MessageBox(NULL,msg,"FATAL ERROR",MB_OK);
     exit(-1);
  }

  // direct plugin access (only safe if matches current version)
  in_yansf = (InYansfDirect*)m_emu->PluginDirect();
  if (in_yansf == NULL ||
      strcmp(m_emu->GetDescription(), NSFPLUG_TITLE) ||
      strcmp(in_yansf->version, NSFPLUG_TITLE))
      in_yansf = NULL;

  // finish unpacking options
  ParseArgs(wargc,wargv,false);
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
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

void CnsfplayDlg::LoadError(const char* filename)
{
    const char* err = "Unknown plugin file load error.";
    if (in_yansf) err = in_yansf->npm->sdat->LoadError();
    if (!filename) filename = "<unknown filename>";

    // create alert message
    CArray<wchar_t,int> wmsg;
    int msgsize = utf8_file(err,NULL,0); if (msgsize<1) msgsize = 1;
    int fnsize = utf8_file(filename,NULL,0); if (fnsize<1) fnsize = 1;
    wmsg.SetSize(msgsize+fnsize);
    utf8_file(err,wmsg.GetData(),msgsize);
    wmsg[msgsize-1] = L'\n'; // replace end of error message with newline
    utf8_file(filename,wmsg.GetData()+msgsize,fnsize);
    MessageBoxW(GetSafeHwnd(),wmsg.GetData(),L"Error reading file!",MB_ICONEXCLAMATION | MB_OK);
}

BOOL CnsfplayDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_last_title[0] = 0;

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
    //SetIcon(m_hIcon, FALSE);

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
    m_volume.SetTicFreq(32);

    int v = 255;
    if (m_volume_init >= 0) v = m_volume_init;
    else v = GetPrivateProfileInt("NSFPLAY","VOLUME",255,m_nsfplay_ini_path.c_str());
    m_volume_init = v;
    m_volume.SetPos(v);
    m_emu->SetVolume(v);
    
    if(m_init_file.GetLength()) {
      if(m_emu->Play(m_init_file.GetBuffer()))
          LoadError(m_init_file.GetBuffer());
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
  UINT nCount, wSize;
  int nSize;
  CArray <char,int> aryFile;
  CArray <wchar_t, int> w_aryFile;

  nCount = DragQueryFileW(hDropInfo, -1, NULL, 0);
  if(nCount==1) {
    wSize = DragQueryFileW(hDropInfo, 0, NULL, 0);
    w_aryFile.SetSize(wSize+1);
    DragQueryFileW(hDropInfo, 0, w_aryFile.GetData(), wSize+1);
    nSize = file_utf8(w_aryFile.GetData(),NULL,0);
    if (nSize < 0) nSize = 1;
    aryFile.SetSize(nSize);
    file_utf8(w_aryFile.GetData(),aryFile.GetData(),nSize);
    if (m_emu->Play(aryFile.GetData()))
      LoadError(aryFile.GetData());
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
  title[1024-5]=0; // truncate if too long for .wav
  strcat(title,".wav");
  wchar_t wtitle[1024];
  utf8_file(title,wtitle,1024);

  const int MAX_FN = 2048;
  wchar_t wfn[MAX_FN];
  utf8_file(title,wfn,2048); // default filename

  OPENFILENAMEW ofn;
  ZeroMemory(&ofn,sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = GetSafeHwnd();
  ofn.lpstrTitle = L"Export WAV";
  ofn.lpstrDefExt = L".wav";
  ofn.Flags = OFN_HIDEREADONLY;
  ofn.lpstrFilter = L"WAV files (*.wav)\0*.wav\0All files (*.*)\0*.*\0\0";
  ofn.lpstrFile = wfn;
  ofn.nMaxFile = MAX_FN;
  ofn.lpstrFileTitle = wtitle; wtitle[0] = 0;
  ofn.nMaxFileTitle = 1024;
  if(GetSaveFileNameW(&ofn))
  {
    char fn[MAX_FN];
    file_utf8(wfn,fn,MAX_FN);
	m_emu->Waveout(fn);
    OnBnClickedPlay();
  }
}

void CnsfplayDlg::OnBnClickedInfo()
{
  m_emu->Info(m_hWnd);
}

void CnsfplayDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
  if (pWnd == GetDlgItem(IDC_INFO)) // right click on info button
  {
    if (in_yansf && *(in_yansf->ui))
    {
       (*(in_yansf->ui))->SetPlayerWindow(m_emu->GetMainWindow());
	   (*(in_yansf->ui))->OpenDialog(NSFplug_UI::DLG_TRACK);
    }
  }
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

  if (m_ini_save)
  {
    WritePrivateProfileString("NSFPLAY","PLUGIN",m_plugin_path.c_str(),m_nsfplay_ini_path.c_str());

    if (m_volume_save < 0) m_volume_save = 1;
    m_volume_save = GetPrivateProfileInt("NSFPLAY","SAVEVOLUME",m_volume_save,m_nsfplay_ini_path.c_str());

    int v = m_volume.GetPos();
    if (!m_volume_save) v = m_volume_init;
    char str[16]; ::itoa(v,str,10);
    WritePrivateProfileString("NSFPLAY","VOLUME",str,m_nsfplay_ini_path.c_str());
    WritePrivateProfileString("NSFPLAY","SAVEVOLUME",m_volume_save?"1":"0",m_nsfplay_ini_path.c_str());
  }
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
  char title[GETFILEINFO_TITLE_LENGTH];
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
    strcpy(m_last_title,title);
    // convert utf8 to unicode
    CArray<wchar_t,int> w;
    w.SetSize(utf8_file(title,NULL,0));
    utf8_file(title,w.GetData(),w.GetSize());
    SetWindowTextW(GetDlgItem(IDC_TITLE)->GetSafeHwnd(),w.GetData());
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
  const int MAX_FN = 2048;
  wchar_t wfn[MAX_FN];
  OPENFILENAMEW ofn;
  ZeroMemory(&ofn,sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = GetSafeHwnd();
  ofn.lpstrTitle = L"Open NSF";
  ofn.lpstrDefExt = L".nsf;.nsfe";
  ofn.Flags = OFN_FILEMUSTEXIST;
  ofn.lpstrFilter = L"NSF files (*.nsf;*.nsfe)\0*.nsf;*.nsfe\0All files (*.*)\0*.*\0\0";
  ofn.lpstrFile = wfn; wfn[0] = 0;
  ofn.nMaxFile = MAX_FN;
  if(GetOpenFileNameW(&ofn))
  {
    char fn[MAX_FN];
    file_utf8(wfn,fn,MAX_FN);
    if (m_emu->Play(fn))
      LoadError(fn);
    m_last_len = -1;
    m_update_wait = 0;
  }
}

int CnsfplayDlg::ParseArgs(int wargc, const wchar_t* const * wargv, bool prepass)
{
	int result = 0;
	for (int i=1; i<wargc; ++i)
	{
		if (wargv[i][0] != L'-') continue;
		char arg[1024];
		file_utf8(wargv[i],arg,1024);
		char* split = strstr(arg,"=");
		if (split == NULL)
		{
			printf("Argument = expected but not found: %s\n",arg);
			++result;
			continue;
		}
		std::string key(arg+1,split-(arg+1));
		const char* val = split+1;
		// special values
		if      (key == "INI")        { if (prepass) { m_yansf_ini_path = val; } }
		else if (key == "NSFPLAYINI") { if (prepass) { m_nsfplay_ini_path = val; } }
		// check NSFPlay ini values
		else if (key == "VOLUME")     { if (prepass) { m_volume_init = atoi(val); m_ini_save = false; } }
		else if (key == "SAVEVOLUME") { if (prepass) { m_volume_save = (atoi(val) != 0); m_ini_save = false; } }
		else if (key == "PLUGIN")     { if (prepass) { m_plugin_path = val; m_ini_save = false; } }
		else if (!prepass) // after prepass, only affects plugin, warn if invalid arguments
		{
			if((in_yansf && in_yansf->npm->cf->HasValue(key)))
			{
				(*(in_yansf->npm->cf))[key] = val;
				in_yansf->npm->no_save_config = true; // changes are temporary unless explicitly saved by user
			}
			else
			{
				printf("Unknown configuration argument: %s\n",key.c_str());
				++result;
			}
		}
	}
	return result;
}

// command line wave output
int CnsfplayDlg::WriteSingleWave(char* nsf_file, char* wave_file, int track, int ms)
{
    if (track < 1) track = 1;

    int last_pos = 0;
    int hang_count = 0;

    int sleep_time = 100;
    const int HANG_TIME = 5000; // timeout if unresponsive
    if (in_yansf) // direct connection allows to set play time exactly
    {
        in_yansf->npm->no_save_config = true;
        if (ms >= 0) (*(in_yansf->npm->cf))["PLAY_TIME"] = ms;
        else  ms = (*(in_yansf->npm->cf))["PLAY_TIME"];
        (*(in_yansf->npm->cf))["AUTO_DETECT"] = 0;
        ms += (*(in_yansf->npm->cf))["FADE_TIME"];
        ms += 10000; // buffer against early cutoff
    }
    else
    {
        if (ms < 0) ms = 3 * 60000; // 3 minute default?
        sleep_time = 5; // no plugin: kludge a cutoff length with 5ms accuracy
    }

    if (m_emu->Play(nsf_file))
    {
        printf("Error opening file: %s\n",nsf_file);
        if (in_yansf) printf("Error: %s\n",in_yansf->npm->sdat->LoadError());
        m_emu->Stop();
        return 1;
    }
    printf("Opened: %s\n",nsf_file);
    m_emu->Stop();
    for (int t=1; t < track; ++t)
    {
        m_emu->Next();
        m_emu->Stop();
    }
    m_emu->Waveout(wave_file);
    printf("Wave Output: %s\n",wave_file);
    m_emu->Play(NULL);
    do
    {
        ::Sleep(sleep_time);
        if (!m_emu->IsPlaying()) break;
        int iot = m_emu->GetOutputTime();
        if (iot >= ms) break;

        if (last_pos != iot)
        {
            hang_count = 0;
        }
        else
        {
            ++hang_count;
            if (hang_count > (HANG_TIME/sleep_time)) break;
        }
        last_pos = iot;
    }
    while (true);
    printf("Play time: %d ms\n",m_emu->GetOutputTime());
    m_emu->Stop();

    return 0;
}
