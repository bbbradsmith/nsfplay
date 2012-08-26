// NSFInfoDialog.cpp : ÉCÉìÉvÉäÉÅÉìÉeÅ[ÉVÉáÉì ÉtÉ@ÉCÉã
//
#include "stdafx.h"
#include "NSFDialogs.h"
#include "NSFInfoDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NSFInfoDialog É_ÉCÉAÉçÉO


NSFInfoDialog::NSFInfoDialog(CWnd* pParent /*=NULL*/)
	: CDialog(NSFInfoDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(NSFInfoDialog)
	m_song = 0;
	m_artist = _T("");
	m_copyright = _T("");
	m_title = _T("");
	m_info = _T("");
	m_fds = _T("");
	m_fme7 = _T("");
	m_mmc5 = _T("");
	m_n106 = _T("");
	m_vrc6 = _T("");
	m_vrc7 = _T("");
    m_pal = _T("");
	//}}AFX_DATA_INIT
}

void NSFInfoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NSFInfoDialog)
	DDX_Control(pDX, IDC_SONGSLIDER, m_songslider);
	DDX_Text(pDX, IDC_SONG, m_song);
	DDX_Text(pDX, IDC_ARTIST, m_artist);
	DDX_Text(pDX, IDC_COPYRIGHT, m_copyright);
	DDX_Text(pDX, IDC_TITLE, m_title);
	DDX_Text(pDX, IDC_INFO, m_info);
	DDX_Text(pDX, IDC_FDS, m_fds);
	DDX_Text(pDX, IDC_FME7, m_fme7);
	DDX_Text(pDX, IDC_MMC5, m_mmc5);
	DDX_Text(pDX, IDC_N106, m_n106);
	DDX_Text(pDX, IDC_VRC6, m_vrc6);
	DDX_Text(pDX, IDC_VRC7, m_vrc7);
	DDX_Text(pDX, IDC_PAL, m_pal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(NSFInfoDialog, CDialog)
	//{{AFX_MSG_MAP(NSFInfoDialog)
	ON_COMMAND(ID_MEMDLG, OnMemdlg)
	ON_COMMAND(ID_EASY, OnEasy)
	ON_COMMAND(ID_CONFIG, OnConfigBox)
	ON_COMMAND(ID_MIXERBOX, OnMixerbox)
	ON_COMMAND(ID_PANBOX, OnPanBox)
	ON_COMMAND(ID_FILE_ABOUT, OnAbout)
	ON_COMMAND(ID_SAVE, OnSave)
	ON_COMMAND(ID_LOAD, OnLoad)
	ON_COMMAND(ID_GENPLS, OnGenpls)
	ON_COMMAND(ID_MASKBOX, OnMaskbox)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_PREV, OnPrev)
	//}}AFX_MSG_MAP
  ON_COMMAND(ID_DELALL, OnDelall)
  ON_COMMAND(ID_DELONE, OnDelone)
//  ON_COMMAND(ID_USETAG, OnUsetag)
ON_COMMAND(ID_READTAG, OnReadtag)
ON_COMMAND(ID_WRITETAG, OnWritetag)
ON_COMMAND(ID_TRKINFO, OnTrkinfo)
ON_STN_CLICKED(IDC_ARTIST, OnStnClickedArtist)
ON_WM_DROPFILES()
ON_COMMAND(ID_NEWPLS, OnNewpls)
ON_COMMAND(ID_PRESET, OnLoadpreset)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// NSFInfoDialog ÇÃã@î
void NSFInfoDialog::SetInfo(NSF *nsf)
{
  CString ss;
  static char buf[4096], *p;
  int i=0;

  m_title = (CString) nsf->title;
  m_artist = (CString) nsf->artist;
  m_copyright = (CString) nsf->copyright;
  
  m_fds =  (CString)(nsf->use_fds?"FDS":"");
  m_mmc5 = (CString)(nsf->use_mmc5?"MMC5":"");
  m_fme7 = (CString)(nsf->use_fme7?"5B":"");
  m_vrc6 = (CString)(nsf->use_vrc6?"VRC6":"");
  m_vrc7 = (CString)(nsf->use_vrc7?"VRC7":"");
  m_n106 = (CString)(nsf->use_n106?"N163":"");
  m_pal =  (CString)((nsf->pal_ntsc == 0) ? "NTSC" :
      ((nsf->pal_ntsc == 1) ? "PAL" : "DUAL"));
  m_info = (CString)(buf);
  m_song = (nsf->song+1);
  m_songslider.SetRange(1, nsf->songs);
  m_songslider.SetTicFreq(1);
  m_songslider.SetPos(m_song);
  m_songslider.SetPageSize(16);
  m_songslider.SetLineSize(1);

  nsf_copy = (*nsf); nsf_copy.body=NULL; // ÉwÉbÉ_ÇÃÇ›ÉRÉsÅ[  
  ntag.SetNSF(&nsf_copy);

  if (nsf->text != NULL)
  {
    ss.Format("%s\n--- end of NSFe text ---", nsf->text);
    m_info += ss;
  }

  if((int)CONFIG["WRITE_TAGINFO"]&&!ntag.IsExistSection(true)&&!ntag.IsExistSection(false))
    ntag.CreateTag(CONFIG["TITLE_FORMAT"]);

  ss.Format(
      "FILE=%s\r\n"
      "Start track: %d of %d\r\n"
      "NTSC Speed: %fHz (%d)\r\n"
      "PAL Speed: %fHz (%d)\r\n"
      "Banks: %02X %02X %02X %02X %02X %02X %02X %02X\r\n"
      "Load/Init/Play: %04X/%04X/%04X\r\n"
      , nsf->filename
      , nsf->start, nsf->songs
      , 1000000.0 / nsf->speed_ntsc, nsf->speed_ntsc
      , 1000000.0 / nsf->speed_pal, nsf->speed_pal
      , nsf->bankswitch[0]
      , nsf->bankswitch[1]
      , nsf->bankswitch[2]
      , nsf->bankswitch[3]
      , nsf->bankswitch[4]
      , nsf->bankswitch[5]
      , nsf->bankswitch[6]
      , nsf->bankswitch[7]
      , nsf->load_address, nsf->init_address, nsf->play_address
      );
  m_info += ss;

  if(ntag.IsExistSection())
  {
    ss.LoadString(IDS_FOUNDTAG);
    m_info += ss + "\r\n";
    if(!ntag.IsWriteEnable())
    {
      ss.LoadString(IDS_READONLYTAG);
      m_info += ss + "\r\n";
    }
  }

  local_tag = ntag.IsExistSection(true);
  CMenu *menu = GetMenu();
  if(menu)
  {
    menu->EnableMenuItem(ID_DELONE, ntag.IsWriteEnable()&&!local_tag?MF_ENABLED:MF_GRAYED);
    menu->EnableMenuItem(ID_DELALL, ntag.IsWriteEnable()&&!local_tag?MF_ENABLED:MF_GRAYED);
  }

  if(local_tag)
  {
    local_tag = true;
    ss.LoadString(IDS_LOCALTAG);
    m_info += ss + "\r\n";
  }  

  GetDlgItem(IDC_SONGSLIDER)->EnableWindow(nsf->enable_multi_tracks?TRUE:FALSE);
  GetDlgItem(IDC_PREV)->EnableWindow(nsf->enable_multi_tracks?TRUE:FALSE);
  GetDlgItem(IDC_NEXT)->EnableWindow(nsf->enable_multi_tracks?TRUE:FALSE);

  UpdateData(false);
}

void NSFInfoDialog::SetInfo(char *fn)
{
  if(nsf.LoadFile(fn)==false)
  {
    GetDlgItem(IDC_INFO)->SetWindowText("Not a NSF file");
    return;
  }

  SetInfo(&nsf);
}

// ÉvÉåÉCÉäÉXÉgê∂ê¨
void NSFInfoDialog::GeneratePlaylist(bool clear)
{
  if (ntag.sdat == NULL)
  {
      MessageBox("No NSF loaded.","Fatal Error");
      return;
  }

  int i;
  FILE *fp;
  char path_buffer[_MAX_PATH];
  NSF local_nsf(*ntag.sdat);
  local_nsf.body = NULL;
  NSF_TAG local_ntag(&local_nsf);

  _makepath( path_buffer, ntag.drv, ntag.dir, ntag.fname, "pls" ); 

  if(!parent||!parent->hWinamp)
  {
    MessageBox("Can't find Winamp.","Fatal Error");
    return;
  }

  fp = fopen(path_buffer,"r");
  if(fp)
  {
    fclose(fp);
    if(IDNO==MessageBox("Playlist file already exists. Overwrite?", path_buffer, MB_YESNO|MB_ICONWARNING))
      return;
  }

  fp = fopen(path_buffer, "w");
  if(fp==NULL)
  {
    MessageBox("Playlist Write Error!", "Error", MB_OK|MB_ICONEXCLAMATION );
    return;
  }

  fprintf(fp, "[playlist]\n");
  nsf.playlist_mode = false;
  for(i=0;i<nsf.songs;i++)
  {
    local_nsf.SetSong(i);
    local_ntag.ReadTagItem(i);
    fprintf(fp, "File%d=%s\n", i+1, 
      local_nsf.GetPlaylistString(
        (*(parent->GetConfig()))["TITLE_FORMAT"],
        (*(parent->GetConfig()))["READ_TAGINFO"].GetInt()?true:false
      )
    );
  }
  fprintf(fp, "NumberOfEntries=%d\n",i);
  fprintf(fp, "Version=2\n");
  fclose(fp);

  if(clear) 
    ::SendMessage(parent->hWinamp,WM_WA_IPC,0,IPC_DELETE);

  COPYDATASTRUCT cds;
  cds.dwData = IPC_PLAYFILE;
  cds.lpData = (void *)path_buffer;
  cds.cbData = (DWORD)strlen((char *) cds.lpData)+1;
  ::SendMessage(parent->hWinamp,WM_COPYDATA,(WPARAM)NULL,(LPARAM)&cds);
  ::PostMessage(parent->hWinamp, WM_COMMAND, WINAMP_BUTTON2, 0) ;
  return;

}

/////////////////////////////////////////////////////////////////////////////
// NSFInfoDialog ÉÅÉbÉZÅ[ÉW ÉnÉìÉhÉâ

void NSFInfoDialog::OnNewpls()
{
  // menu item removed, causes crash on NSFPlay
  GeneratePlaylist(true);
}

void NSFInfoDialog::OnGenpls() 
{
  // menu item removed, causes crash on NSFPlay
  GeneratePlaylist(false);
}

void NSFInfoDialog::OnMemdlg() 
{
	parent->memory->Open();
}

void NSFInfoDialog::OnEasy() 
{
  parent->easy->Open();	
}

void NSFInfoDialog::OnConfigBox() 
{
	parent->config->Open();
}

void NSFInfoDialog::OnMixerbox() 
{
  parent->mixer->Open();
}

void NSFInfoDialog::OnPanBox() 
{
  parent->panner->Open();
}

BOOL NSFInfoDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  HICON hIcon = AfxGetApp()->LoadIcon(IDI_NSF);
  SetIcon(hIcon, TRUE);
  CMenu *menu = GetMenu();
  if(menu)
  {
    menu->CheckMenuItem(ID_READTAG, (int)CONFIG["READ_TAGINFO"]?MF_CHECKED:MF_UNCHECKED);
    menu->CheckMenuItem(ID_WRITETAG, (int)CONFIG["WRITE_TAGINFO"]?MF_CHECKED:MF_UNCHECKED);
    menu->EnableMenuItem(ID_DELONE, true?MF_ENABLED:MF_GRAYED);
    menu->EnableMenuItem(ID_DELALL, true?MF_ENABLED:MF_GRAYED);
  }
	return TRUE;  // ÉRÉìÉgÉçÅ[ÉãÇ…ÉtÉHÅ[ÉJÉXÇê›íËÇµÇ»Ç¢Ç∆Ç´ÅAñﬂÇËílÇÕ TRUE Ç∆Ç»ÇËÇ‹Ç∑
	              // ó·äO: OCX ÉvÉçÉpÉeÉB ÉyÅ[ÉWÇÃñﬂÇËílÇÕ FALSE Ç∆Ç»ÇËÇ‹Ç∑
}

void NSFInfoDialog::OnSave() 
{
  CFileDialog fd(FALSE,".ini","in_yansf",OFN_OVERWRITEPROMPT,"INI files (*.ini)|*.ini||",this);

  if(fd.DoModal()==IDOK)
    pm->cf->Save(fd.GetPathName().GetBuffer(1),"NSFPlug");	
}

void NSFInfoDialog::OnAbout()
{
    ::MessageBox(NULL,
        "NSFPlug " NSFPLUG_VERSION "\n"
        "Brad Smith, " __DATE__ "\n"
        "http://rainwarrior.ca/\n"
        "\n"
        "Original by Brezza, Dec 19 2006\n"
        "http://pokipoki.org/dsa/\n"
        ,
        "About",
        MB_OK);
}

void NSFInfoDialog::OnLoad() 
{
  CFileDialog fd(TRUE,".ini","in_yansf",OFN_FILEMUSTEXIST,"INI files (*.ini)|*.ini||",this);

  if(fd.DoModal()==IDOK)
  {
    pm->cf->Load(fd.GetPathName().GetBuffer(1),"NSFPlug");
    pm->cf->Notify(-1);
  }

}

void NSFInfoDialog::OnMaskbox() 
{
	parent->mask->Open();	
}

void NSFInfoDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  if(pScrollBar == (CScrollBar *)&m_songslider)
  {
    if(nSBCode == TB_THUMBTRACK)
    {
      m_song = nPos;
      UpdateData(false);
    }
    else if(nSBCode == TB_ENDTRACK)
    {
      m_song = m_songslider.GetPos();
      if(UpdateData(false))
        ::SendMessage(parent->hWinamp, WM_USER+2413, (WPARAM)0xdeadbeef, (LPARAM)m_song-1);
    }
  }
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void NSFInfoDialog::OnPrev()
{
  int min, max;
  m_songslider.GetRange(min, max);
  m_song = m_song - 1;
  if((int)m_song < min) m_song = min;
	if(UpdateData(false))
    ::SendMessage(parent->hWinamp, WM_USER+2413, (WPARAM)0xdeadbeef, (LPARAM)m_song-1);
}

void NSFInfoDialog::OnNext() 
{
  int min, max;
  m_songslider.GetRange(min, max);
  m_song = m_song + 1;
  if(max < (int)m_song ) m_song = max;
  if(UpdateData(false))
    ::SendMessage(parent->hWinamp, WM_USER+2413, (WPARAM)0xdeadbeef, (LPARAM)m_song-1);
}

void NSFInfoDialog::OnDelall()
{
  if(ntag.ClearTag()==0)
    MessageBox("Ç±ÇÃÉ^ÉOèÓïÒÇÕè¡ãéÇ≈Ç´Ç‹ÇπÇÒ");
}

void NSFInfoDialog::OnDelone()
{
  if(ntag.InitTagItem(m_song-1, CONFIG["TITLE_FORMAT"])==0)
    MessageBox("Ç±ÇÃÉ^ÉOèÓïÒÇÕè¡ãéÇ≈Ç´Ç‹ÇπÇÒ");
}

void NSFInfoDialog::OnReadtag()
{
  CMenu *menu = GetMenu();
  CONFIG["READ_TAGINFO"] = !(int)CONFIG["READ_TAGINFO"];
  if(menu)
    menu->CheckMenuItem(ID_READTAG, CONFIG["READ_TAGINFO"]?MF_CHECKED:MF_UNCHECKED);
}

void NSFInfoDialog::OnWritetag()
{
  CMenu *menu = GetMenu();
  CONFIG["WRITE_TAGINFO"] = !(int)CONFIG["WRITE_TAGINFO"];
  if(menu)
  {
    menu->CheckMenuItem(ID_WRITETAG, CONFIG["WRITE_TAGINFO"]?MF_CHECKED:MF_UNCHECKED);
    menu->EnableMenuItem(ID_DELONE, ntag.IsWriteEnable()&&!local_tag?MF_ENABLED:MF_GRAYED);
    menu->EnableMenuItem(ID_DELALL, ntag.IsWriteEnable()&&!local_tag?MF_ENABLED:MF_GRAYED);
  }
}

void NSFInfoDialog::OnTrkinfo()
{
  parent->OpenDialog(NSFplug_UI::DLG_TRACK);
}


void NSFInfoDialog::OnStnClickedArtist()
{
  // TODO : Ç±Ç±Ç…ÉRÉìÉgÉçÅ[Éãí ímÉnÉìÉhÉâ ÉRÅ[ÉhÇí«â¡ÇµÇ‹Ç∑ÅB
}

void NSFInfoDialog::OnDropFiles(HDROP hDropInfo)
{
  CArray<char,int> aryFile;
  UINT nSize, nCount = DragQueryFile(hDropInfo, -1, NULL, 0);

  if(parent->wa2mod)
  {
    parent->wa2mod->ClearList();
    for(UINT i=0;i<nCount;i++)
    {
      nSize = DragQueryFile(hDropInfo, i, NULL, 0);
      aryFile.SetSize(nSize+1);
      DragQueryFile(hDropInfo, i, aryFile.GetData(), nSize+1);
      parent->wa2mod->QueueFile(aryFile.GetData());
    }
    parent->wa2mod->PlayStart();
  }

  __super::OnDropFiles(hDropInfo);
}


void NSFInfoDialog::OnLoadpreset()
{
  parent->preset->ShowWindow(SW_SHOW);
}
