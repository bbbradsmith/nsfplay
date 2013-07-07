// NSFTrackDialog.cpp : 実装ファイル
//

#include "stdafx.h"
#include "nsfplug_ui.h"
#include "NSFDialogs.h"
#include "NSFTrackDialog.h"

// color reader
int read_color(const char* c)
{
    if (c == NULL) return -1;
    //DEBUG_OUT("read_color: %s\n",c);
    if (::strlen(c) != 6) return -1;
    int shift = 6;
    int result = 0;
    do
    {
        --shift;
        char x = *c;
        ++c;
        int val = -1;
        if      (x >= '0' && x <= '9') val = x - '0';
        else if (x >= 'a' && x <= 'f') val = 10 + x - 'a';
        else if (x >= 'A' && x <= 'F') val = 10 + x - 'A';
        if (val < 0 || val > 15) return -1;
        result += (val << (shift * 4));
    } while (shift != 0);
    return result;
}

// NSFTrackDialog ダイアログ

IMPLEMENT_DYNAMIC(NSFTrackDialog, CDialog)
NSFTrackDialog::NSFTrackDialog(CWnd* pParent /*=NULL*/)
	: CDialog(NSFTrackDialog::IDD, pParent)
{
  m_active = false;
  for(int i=0;i<NSFPlayer::NES_TRACK_MAX;i++)
    m_showtrk[i]=true;

  green_pen.CreatePen(PS_SOLID,1,RGB(0,212,0));
  m_pDCtrk = NULL;
  m_nTimer = 0;
  m_nTimer2 = 0;
  m_rmenu.LoadMenu(IDR_TRKINFOMENU);
}

NSFTrackDialog::~NSFTrackDialog()
{
  if(green_pen.DeleteObject()==0)
    DEBUG_OUT("Error in deleting the green_pen object.\n");
}

#define CONFIG (*(pm->cf))

void NSFTrackDialog::UpdateNSFPlayerConfig(bool b)
{
  if(b)
  {
    m_setup.m_freq_value   = CONFIG["INFO_FREQ"];
    m_setup.m_delay_value  = CONFIG["INFO_DELAY"];
    m_speed.SetPos( 256 / CONFIG["MULT_SPEED"] );
    m_setup.m_freq_mode    = !(int)CONFIG["FREQ_MODE"];
    m_setup.m_graphic_mode = !(int)CONFIG["GRAPHIC_MODE"];
  }
  else
  {
    CONFIG["INFO_FREQ"]    = m_setup.m_freq_value;
    CONFIG["INFO_DELAY"]   = m_setup.m_delay_value;
    CONFIG["MULT_SPEED"]   = 256 / m_speed.GetPos();
    CONFIG["FREQ_MODE"]    = !m_setup.m_freq_mode;
    CONFIG["GRAPHIC_MODE"] = !m_setup.m_graphic_mode;
  }
}

void NSFTrackDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_TRACKINFO, m_trkinfo);
  DDX_Control(pDX, IDC_SPEED, m_speed);
}


BEGIN_MESSAGE_MAP(NSFTrackDialog, CDialog)
  ON_WM_TIMER()
  ON_WM_SHOWWINDOW()
  ON_NOTIFY(LVN_ITEMCHANGED, IDC_TRACKINFO, OnLvnItemchangedTrackinfo)
  ON_BN_CLICKED(IDC_SETUP, OnBnClickedSetup)
  ON_WM_DROPFILES()
  ON_WM_HSCROLL()
  ON_WM_SIZE()
//  ON_WM_SIZECLIPBOARD()
ON_WM_SIZING()
ON_WM_VSCROLL()
ON_COMMAND(ID_COPY, OnCopy)
ON_COMMAND(ID_SETTINGS, OnSettings)
//ON_WM_RBUTTONDOWN()
//ON_WM_RBUTTONUP()
ON_NOTIFY(NM_RCLICK, IDC_TRACKINFO, OnNMRclickTrackinfo)
END_MESSAGE_MAP()


// NSFTrackDialog メッセージ ハンドラ

inline static char *note2string(int note)
{
  static char *notename[12]=
  {
    "C ","C#","D ","D#",
    "E ","F ","F#","G ",
    "G#","A ","A#","B "
  };

  if(note)
    return notename[note%12];
  else
    return "* ";
}

static int keyColMap[NSFPlayer::NES_TRACK_MAX] =
{
  0xFF0000, 0xFF0000, //APU1
  0x00FF00, 0x00FF00, 0x000000, //APU2
  0x0080FF, //FDS
  0xFFC000, 0xFFC000, 0x000000, //MMC5
  0x0000FF, 0x0000FF, 0x0000FF, 0x0000FF, 0x000000, //FME7
  0xFF8000, 0xFF8000, 0xFF8000, //VRC6
  0x8000FF, 0x8000FF, 0x8000FF, 0x8000FF, 0x8000FF, 0x8000FF,//VRC7
  0xFF0080, 0xFF0080, 0xFF0080, 0xFF0080, 0xFF0080, 0xFF0080, 0xFF0080, 0xFF0080  //N106
};

void NSFTrackDialog::OnTimer(UINT nIDEvent)
{
  __super::OnTimer(nIDEvent);

  EnterCriticalSection(&parent->cso);

  if(parent->wa2mod && m_active
     && !parent->wa2mod->GetResetFlag() // track info is not reset yet on Play(), it happens in PlayThread
     )
  {
    int time_in_ms;
    time_in_ms = parent->wa2mod->GetOutputTime();
    
    if(nIDEvent==2)
    {
      for(int trk=0;trk<m_maxtrk;trk++)
      {
        ITrackInfo *ti;
        int delay = (int)CONFIG["INFO_DELAY"];
        ti = dynamic_cast<ITrackInfo *>(pm->pl->GetInfo(time_in_ms-delay, m_trkmap[trk]));

        if(ti)
        {
          int key = ti->GetNote(ti->GetFreqHz());
          if(ti->GetKeyStatus()||m_lastkey[m_trkmap[trk]]!=key)
          {
            //m_keepkey[m_trkmap[trk]] = 120/(int)CONFIG["INFO_FREQ"];
            m_keepkey[m_trkmap[trk]] = 0; // BS disabling m_keepkey
            // causes erroneous flicker when freq changes even though channel is muted
            // esp. noticable on VRC6 square (enable shares with high freq register)
            m_lastkey[m_trkmap[trk]] = key;
          }
          else if(m_keepkey[m_trkmap[trk]])
            m_keepkey[m_trkmap[trk]]--;
        }
      }
    }
    else if(nIDEvent==1)
    {
      m_keydlg.Reset();
      for(int trk=0;trk<m_maxtrk;trk++)
      {
        ITrackInfo *ti;
        int delay = (int)CONFIG["INFO_DELAY"];
        ti = dynamic_cast<ITrackInfo *>(pm->pl->GetInfo(time_in_ms-delay, m_trkmap[trk]));

        if(ti)
        {
          int i;
          CString str;
          CRect rect;

          // 音量表示
          int vol = ti->GetVolume();
          switch(m_trkmap[trk])
          {
          case NSFPlayer::APU1_TRK0:
          case NSFPlayer::APU1_TRK1:
          case NSFPlayer::APU2_TRK1:
          case NSFPlayer::FME7_TRK0:
          case NSFPlayer::FME7_TRK1:
          case NSFPlayer::FME7_TRK2:
            if(vol & 0x10)
                str.Format("%s%2d",((vol&0x20)?"L":"E"),(vol&=0xF));
            else if(vol>=0)
              str.Format("%2d",(vol&0xF)); 
            else
              str="-";
            break;
          default:
            if(vol>=0)
              str.Format("%2d",ti->GetVolume()); 
            else
              str="-";
            break;
          }
          
          if(1)
          {
            m_trkinfo.SetItem(trk,1,LVIF_TEXT,str,0,0,0,0);
          }
          else if(m_trkinfo.GetTopIndex()<=trk)
          {
            m_trkinfo.GetSubItemRect(trk,1,LVIR_BOUNDS,rect);
            rect.top +=1; rect.bottom -=1; rect.left ++; rect.right --;
            m_pDCtrk->FillSolidRect(rect,RGB(0,0,0));
            rect.top +=3; rect.bottom -=3; rect.left +=1; rect.right -=1;
            rect.right = rect.left + rect.Width()*max(vol,0)/ti->GetMaxVolume();
            m_pDCtrk->FillSolidRect(rect,RGB(0,212,0));
          }

          // 周波数表示
          if(!(int)CONFIG["FREQ_MODE"])
          {
            str.Format("%3X",ti->GetFreq());
            m_trkinfo.SetItem(trk,2,LVIF_TEXT,str,0,0,0,0);
          }
          else
          {
            str.Format("%03.1f",ti->GetFreqHz());
            m_trkinfo.SetItem(trk,2,LVIF_TEXT,str,0,0,0,0);
          }
          
          int key = ti->GetNote(ti->GetFreqHz());

          // キー表示
          if(ti->GetKeyStatus()||m_keepkey[m_trkmap[trk]])
            m_trkinfo.SetItem(trk,3,LVIF_TEXT,note2string(key),0,0,0,0);
          else
            m_trkinfo.SetItem(trk,3,LVIF_TEXT,"",0,0,0,0);

          // オクターブ表示
          if(ti->GetFreqHz()!=0.0)
          {
            str.Format("%d",key/12-4);
            m_trkinfo.SetItem(trk,4,LVIF_TEXT,str,0,0,0,0);
          }
          else
          {
            m_trkinfo.SetItem(trk,4,LVIF_TEXT,"",0,0,0,0);
          }

          // 鍵盤表示
          if(m_trkinfo.GetCheck(trk)&&(ti->GetKeyStatus()||m_keepkey[m_trkmap[trk]]))
          {
            if(m_trkmap[trk]==NSFPlayer::APU2_TRK1)
              m_keydlg.m_keyheader.m_nNoiseStatus = (ti->GetFreq()&0xF)+1;
            else if(m_trkmap[trk]==NSFPlayer::APU2_TRK2)
              m_keydlg.m_keyheader.m_nDPCMStatus = (ti->GetFreq()&0xF)+1;
            else
            m_keydlg.m_keywindow.KeyOn(key-12*4, keyColMap[m_trkmap[trk]]);
          }

          // 音色表示
          if(ti->GetTone()>=0)
          {
            switch(m_trkmap[trk])
            {
            case NSFPlayer::APU2_TRK1:
              str = ti->GetTone()?"P":"N";
              m_trkinfo.SetItem(trk,5,LVIF_TEXT,str,0,0,0,0);
              break;

            case NSFPlayer::APU2_TRK2:
              str.Format("%04X",ti->GetTone());
              m_trkinfo.SetItem(trk,5,LVIF_TEXT,str,0,0,0,0);
              break;

            case NSFPlayer::FME7_TRK0:
            case NSFPlayer::FME7_TRK1:
            case NSFPlayer::FME7_TRK2:
              if(ti->GetTone()==0)
                str.Format("TN");
              else if(ti->GetTone()==1)
                str.Format("T-");
              else if(ti->GetTone()==2)
                str.Format("-N");
              else
                str.Format("--");
              m_trkinfo.SetItem(trk,5,LVIF_TEXT,str,0,0,0,0);
              break;

            case NSFPlayer::FME7_TRK3: // envelope
              str.Format("%1X",ti->GetTone());
              m_trkinfo.SetItem(trk,5,LVIF_TEXT,str,0,0,0,0);
              break;

            default:
              str.Format("%d",ti->GetTone());
              m_trkinfo.SetItem(trk,5,LVIF_TEXT,str,0,0,0,0);
              break;
            }
          }
          else 
            m_trkinfo.SetItem(trk,5,LVIF_TEXT,"-",0,0,0,0);

          switch(m_trkmap[trk])
          {
            case NSFPlayer::FDS_TRK0:
              m_tonestr[NSFPlayer::FDS_TRK0]="";
              for(i=0;i<64;i++)
                m_tonestr[NSFPlayer::FDS_TRK0].AppendFormat("%02d ", ((char)((dynamic_cast<TrackInfoFDS *>(ti)->wave[i])&0xFF))+32);

              if(!(int)CONFIG["GRAPHIC_MODE"])
              {
                m_trkinfo.SetItem(trk,6,LVIF_TEXT,m_tonestr[NSFPlayer::FDS_TRK0],0,0,0,0);
              }
              else if(m_trkinfo.GetTopIndex()<=trk)
              {
                CPen *oldpen = m_pDCtrk->SelectObject(&green_pen);
                m_trkinfo.GetSubItemRect(trk,6,LVIR_BOUNDS,rect);
                rect.top +=1; // rect.bottom -=1;
                rect.left +=1; rect.right -=4;
                m_pDCtrk->FillSolidRect(rect,RGB(0,0,0));
                rect.bottom -=1;
                int length = min(64, rect.Width());
                //int mid = (rect.top + rect.bottom)/2;

                m_pDCtrk->MoveTo(rect.left,rect.bottom);
                m_pDCtrk->LineTo(rect.left+length,rect.bottom);            
                for(i=0;i<length;i++)
                {
                  m_pDCtrk->MoveTo(rect.left+i, rect.bottom); 
                  m_pDCtrk->LineTo(rect.left+i, rect.bottom-
                     (static_cast<TrackInfoFDS *>(ti)->wave[i] * rect.Height()/64));
                }
                m_pDCtrk->SelectObject(oldpen);
              }
              break;

            case NSFPlayer::N106_TRK0:
            case NSFPlayer::N106_TRK1:
            case NSFPlayer::N106_TRK2:
            case NSFPlayer::N106_TRK3:
            case NSFPlayer::N106_TRK4:
            case NSFPlayer::N106_TRK5:
            case NSFPlayer::N106_TRK6:
            case NSFPlayer::N106_TRK7:
              
              m_tonestr[m_trkmap[trk]]="";
              for(i=0;i<dynamic_cast<TrackInfoN106 *>(ti)->wavelen;i++)
                m_tonestr[m_trkmap[trk]].AppendFormat("%02d ", (static_cast<TrackInfoN106 *>(ti)->wave[i]+8)&0xF);

              if(!(int)CONFIG["GRAPHIC_MODE"])
              {
                m_trkinfo.SetItem(trk,6,LVIF_TEXT,m_tonestr[m_trkmap[trk]],0,0,0,0);
              }
              else if(m_trkinfo.GetTopIndex()<=trk)
              {
                CPen *oldpen = m_pDCtrk->SelectObject(&green_pen);
                m_trkinfo.GetSubItemRect(trk,6,LVIR_BOUNDS,rect);
                rect.top +=1; // rect.bottom -=1;
                rect.left +=1; rect.right -=4;
                m_pDCtrk->FillSolidRect(rect,RGB(0,0,0));
                rect.bottom -=1;
                int length = min(dynamic_cast<TrackInfoN106 *>(ti)->wavelen, rect.Width());
                if (ti->GetVolume() == 0 && length >= 128)
                {
                    // hide waves that are muted and long
                    // (engines frequently wipe most or all of the length register when muted)
                    length = 0;
                }
                //int mid = (rect.top + rect.bottom)/2;
                m_pDCtrk->MoveTo(rect.left,rect.bottom);
                m_pDCtrk->LineTo(rect.left+length,rect.bottom);            
                for(i=0;i<length;i++)
                {
                  m_pDCtrk->MoveTo(rect.left+i, rect.bottom); 
                  m_pDCtrk->LineTo(rect.left+i, rect.bottom -
                     (dynamic_cast<TrackInfoN106 *>(ti)->wave[i] * rect.Height() / 16));
                }
                m_pDCtrk->SelectObject(oldpen);
              }
              break;
            default:
              break;
          }
        }
        else
        {
          for(int i=1;i<7;i++)
            m_trkinfo.SetItem(trk,i,LVIF_TEXT,"",0,0,0,0);
        }
      }
    }
  }

  LeaveCriticalSection(&parent->cso);
}

void NSFTrackDialog::InitList()
{
  NSF *nsf = pm->pl->nsf;
  int i,j; 
  char *tname[][16] = { 
    {"SQR0","SQR1","TRI","NOISE","DMC",NULL},
    {"FDS",NULL},
    {"MMC5:0","MMC5:1","MMC5:P",NULL},
    {"5B:0","5B:1","5B:2","5B:E","5B:N",NULL},
    {"VRC6:0","VRC6:1","VRC6:2",NULL},
    {"VRC7:0", "VRC7:1", "VRC7:2", "VRC7:3", "VRC7:4", "VRC7:5", NULL},
    {"N163:0", "N163:1", "N163:2", "N163:3","N163:4", "N163:5", "N163:6", "N163:7",NULL},
    NULL 
  };

  m_initializing_list = true;
  m_trkinfo.DeleteAllItems();
  int idx=0, num=0;
  for(i=0;tname[i][0]!=NULL;i++)
  {
    for(j=0;tname[i][j]!=NULL;j++)
    {
      m_trkmap[idx]=num++;
      switch(i)
      {
      case 0:
        break;
      case 1:
        if(!nsf->use_fds) continue;
        break;
      case 2:
        if(!nsf->use_mmc5) continue;
        break;
      case 3:
        if(!nsf->use_fme7) continue;
        break;
      case 4:
        if(!nsf->use_vrc6) continue;
        break;
      case 5:
        if(!nsf->use_vrc7) continue;
        break;
      case 6:
        if(!nsf->use_n106) continue;
        break;
      default:
        continue;
      }
      m_trkinfo.InsertItem(idx,tname[i][j]);
      m_trkinfo.SetCheck(idx,m_showtrk[num-1]);
      idx++;
    }
  }
  m_maxtrk = idx;
  m_initializing_list = false;

}

void NSFTrackDialog::LocateDialogItems()
{  
  CRect rect_c, rect1, rect2;

  if(m_keydlg.m_hWnd&&m_trkinfo.m_hWnd)
  {
    GetClientRect(&rect_c);
    rect2.SetRect(0, 0, rect_c.Width(), (rect_c.Width()/14)+m_keydlg.m_keyheader.MinHeight());
    rect2.MoveToXY(0, rect_c.bottom-rect2.Height());
    m_keydlg.MoveWindow(&rect2);

    m_trkinfo.GetWindowRect(&rect1);
    rect1.right = rect_c.right;
    rect1.left = 0;
    rect1.top = 32;
    rect1.bottom = rect2.top - 1;
    m_trkinfo.MoveWindow(&rect1);
  }
}

BOOL NSFTrackDialog::OnInitDialog()
{
  __super::OnInitDialog();

  HICON hIcon = AfxGetApp()->LoadIcon(IDI_NSF);
  SetIcon(hIcon, TRUE);

  char *cname[] = { "Track  ", "Vol", "     Freq", "Key", "Oct", "Tone", "Wave", NULL };

  m_pDCtrk = m_trkinfo.GetDC();
  for(int i=0; cname[i]!=NULL; i++)
    m_trkinfo.InsertColumn(i,cname[i],(i==0||i==6)?LVCFMT_LEFT:LVCFMT_RIGHT,i==6?68:m_pDCtrk->GetTextExtent(cname[i]).cx+12+(i?0:12),i);
  m_trkinfo.SetExtendedStyle(LVS_EX_CHECKBOXES|LVS_EX_HEADERDRAGDROP|LVS_EX_DOUBLEBUFFER);
  m_trkinfo.SetBkColor(RGB(0,0,0));
  m_trkinfo.SetTextColor(RGB(0,212,0));
  m_trkinfo.SetTextBkColor(RGB(0,0,0));
  
  m_keydlg.Create(m_keydlg.IDD,this);
  m_keydlg.ShowWindow(SW_SHOW);

  m_speed.SetRange(1,8);
  m_speed.SetTicFreq(1);
  m_speed.SetPageSize(1);
  m_speed.SetLineSize(1);

  CRect rect, rect2;
  GetClientRect(&rect);
  GetWindowRect(&rect2);
  int min_height = rect2.Height()-rect.Height();
  int min_width = rect2.Width()-rect.Width();
  m_keydlg.GetWindowRect(&rect);
  min_height += rect.Height();
  min_width += m_keydlg.MinWidth();
  SetWindowPos(NULL,0,0,min_width,256,SWP_NOMOVE|SWP_NOZORDER);
  LocateDialogItems();

  // setup colors
  for (int i=0; i < NES_CHANNEL_MAX; ++i)
  {
      int t = pm->cf->channel_track[i];
      std::string sc = pm->cf->GetChannelConfig(i,"COL").GetStr();
      int c = read_color(sc.c_str());
      //DEBUG_OUT("Channel %02d colour: %06X (%s)\n", i, c, pm->cf->channel_name[i]);
      if (c >= 0 && c <= 0xFFFFFF) keyColMap[t] = c;
  }
  {
      std::string sc = pm->cf->GetValue("5B_ENVELOPE_COL").GetStr();
      int c = read_color(sc.c_str());
      //DEBUG_OUT("5B envelope colour: %06X\n", c);
      if (c >= 0 && c <= 0xFFFFFF) keyColMap[NSFPlayer::FME7_TRK3] = c;
  }
  {
      std::string sc = pm->cf->GetValue("5B_NOISE_COL").GetStr();
      int c = read_color(sc.c_str());
      //DEBUG_OUT("5B noise colour: %06X\n", c);
      if (c >= 0 && c <= 0xFFFFFF) keyColMap[NSFPlayer::FME7_TRK4] = c;
  }

  return TRUE;  // return TRUE unless you set the focus to a control
  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void NSFTrackDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
  __super::OnShowWindow(bShow, nStatus);

  m_trkinfo.SetFocus(); // in case someone uses mouse scroll before clicking on it,
  // prevents the scroll from doing time expansion instead

  if(bShow==TRUE&&m_nTimer==0)
  {
    m_nTimer = SetTimer(1,1000/(int)CONFIG["INFO_FREQ"], NULL);
    m_nTimer2 = SetTimer(2,1000/120,NULL);
    m_keydlg.Start(1000/(int)CONFIG["INFO_FREQ"]);
  }
  else if(bShow==FALSE&&m_nTimer!=0)
  {
    m_keydlg.Stop();
    KillTimer(m_nTimer);
    KillTimer(m_nTimer2);
    m_nTimer = 0;
  }
}

void NSFTrackDialog::OnLvnItemchangedTrackinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
  if(!m_initializing_list)
    m_showtrk[m_trkmap[pNMLV->iItem]] = m_trkinfo.GetCheck(pNMLV->iItem);
  *pResult = 0;
}


void NSFTrackDialog::OnBnClickedSetup()
{
  if(m_setup.DoModal()==IDOK)
  {
    for(int i=0;i<m_maxtrk;i++)
    {
      for(int j=1;j<7;j++)
        m_trkinfo.SetItem(i,j,LVIF_TEXT,"",0,0,0,0);
    }

    UpdateNSFPlayerConfig(false);
    if(m_nTimer)
    {
      KillTimer(m_nTimer);
      m_nTimer = SetTimer(1,1000/(int)CONFIG["INFO_FREQ"],NULL);
      m_keydlg.Start(1000/(int)CONFIG["INFO_FREQ"].GetInt());
    }
  }
  else
  {
    UpdateNSFPlayerConfig(true);
  }
}

void NSFTrackDialog::OnDropFiles(HDROP hDropInfo)
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

void NSFTrackDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  if((CSliderCtrl *)pScrollBar == &m_speed)
  {
    UpdateNSFPlayerConfig(false);
  }

  __super::OnHScroll(nSBCode, nPos, pScrollBar);
}

void NSFTrackDialog::OnSize(UINT nType, int cx, int cy)
{
  __super::OnSize(nType, cx, cy);
  LocateDialogItems();
}

void NSFTrackDialog::OnSizing(UINT fwSide, LPRECT pRect)
{
  __super::OnSizing(fwSide, pRect);

  CRect rect, rect2;
  GetClientRect(&rect);
  GetWindowRect(&rect2);
  int min_height = rect2.Height()-rect.Height();
  int min_width = rect2.Width()-rect.Width();
  int max_width = rect2.Width()-rect.Width();
  m_keydlg.GetWindowRect(&rect);
  min_height += rect.Height();
  min_width += m_keydlg.MinWidth();
  max_width += m_keydlg.MaxWidth();


  switch(fwSide)
  {
  case WMSZ_LEFT:
  case WMSZ_TOPLEFT:
  case WMSZ_BOTTOMLEFT:
    if(pRect->bottom-pRect->top < min_height)
      pRect->top = pRect->bottom - min_height;
    if(pRect->right-pRect->left < min_width)
      pRect->left = pRect->right - min_width;
    if(pRect->right-pRect->left > max_width)
      pRect->left = pRect->right - max_width;
    break;
  case WMSZ_RIGHT:
  case WMSZ_TOPRIGHT:
  case WMSZ_BOTTOMRIGHT:
    if(pRect->bottom-pRect->top < min_height)
      pRect->bottom = pRect->top + min_height;
    if(pRect->right-pRect->left < min_width)
      pRect->right = pRect->left + min_width;
    if(pRect->right-pRect->left > max_width)
      pRect->right = pRect->left + max_width;
    break;

  case WMSZ_TOP:
    if(pRect->bottom-pRect->top < min_height)
      pRect->top = pRect->bottom - min_height;
    break;

  case WMSZ_BOTTOM:
    if(pRect->bottom-pRect->top < min_height)
      pRect->bottom = pRect->top + min_height;
    break;
  default:
    break;
  }
}

void NSFTrackDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  __super::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL NSFTrackDialog::DestroyWindow()
{
  return __super::DestroyWindow();
}

void NSFTrackDialog::OnCopy()
{
  CString str;

  if ( m_trk_selected < 0 || NSFPlayer::NES_TRACK_MAX <= m_trk_selected) return;

  str = m_tonestr[m_trk_selected] + "\r\n";

  if( !::OpenClipboard(NULL) ) 
  {
    MessageBox("Error to open the clipboard!");
	  return;
  }

  HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, str.GetLength()+1);
	LPTSTR pMem = (LPTSTR)hMem;
	::lstrcpy(pMem, (LPCTSTR)str);

	::EmptyClipboard();
	::SetClipboardData(CF_TEXT, hMem);
	::CloseClipboard();
}

void NSFTrackDialog::OnSettings()
{
  OnBnClickedSetup();
}

void NSFTrackDialog::OnNMRclickTrackinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE) pNMHDR;
  
  CMenu* sub_menu = m_rmenu.GetSubMenu(0);    
  ASSERT(sub_menu);


  if( 0 <= lpnmitem->iItem && lpnmitem->iItem <m_maxtrk )
  {
    m_trk_selected = m_trkmap[lpnmitem->iItem];  
    if( NSFPlayer::FDS_TRK0 == m_trk_selected 
        || (NSFPlayer::N106_TRK0 <= m_trk_selected && m_trk_selected <= NSFPlayer::N106_TRK7))
    {
      CPoint point(lpnmitem->ptAction);
      m_trkinfo.ClientToScreen(&point);
      sub_menu->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
    }
  }
  
  *pResult = 0;
}
