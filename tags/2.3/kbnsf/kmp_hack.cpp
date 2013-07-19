#include "in_nsf.h"
#include "debugout.h"
#include "nsfplug_ui.h"
#include "resource.h"

using namespace xgm;

extern HINSTANCE ghPlugin;

static HHOOK hHookGetMsgProc ;
static HWND hMainWindow ;
static HMENU hMenuInsert ;
static bool Grabbed;

static NSFplug_UI *ui;

static int FindMenu(HMENU hMenu, const char *string)
{
  char temp[256] ;
  int nItem = GetMenuItemCount(hMenu) ;

  for(int i=0;i<nItem;i++)
  {
    GetMenuString(hMenu, i, temp, 256, MF_BYPOSITION);
    if(strcmp(string,temp)==0) return i;
  }

  return -1 ;
}

static HMENU FindSubMenu(HMENU hMenu, const char *string)
{
  int ret = FindMenu(hMenu, string);

  if(ret>=0)
    return GetSubMenu(hMenu,ret);
  else 
    return NULL;
}

static void enable_tag_menu(bool b)
{
  HMENU hMenu = GetMenu(hMainWindow);
  for(int i=4656;i<=4657;i++)
    EnableMenuItem(hMenu,i,b?MF_ENABLED:MF_GRAYED);
}

static void delete_tag_info(bool all)
{
  KMP_NSF *kmp;
  int error = 0;

  EnterCriticalSection(&cso);

  if(kmp_set.size()==1)
  {
    kmp = *(kmp_set.begin());
    NSF *nsf = kmp->sdat;
    if(all)
    {
      if(kmp->ntag->ClearTag()==0) 
        error = 2;
      else
        enable_tag_menu(false);
    }
    else
    {
      if(kmp->ntag->InitTagItem(nsf->song,(*(kmp->cf))["TITLE_FORMAT"])==0) error = 2;
    }
  }
  else error = 1;

  LeaveCriticalSection(&cso);
  
  if(error==1)
    MessageBox(NULL,"曲の切り替え中であるか、またはNSFが選択されていません。","エラー",MB_OK|MB_ICONEXCLAMATION);
  else if(error==2)
    MessageBox(NULL,"タグ情報が存在しないか、NSFplugが生成したタグ情報ではありません。",kmp->ntag->m_sect,MB_OK|MB_ICONEXCLAMATION);
  else
    MessageBox(NULL,"タグ情報を初期化しました",kmp->ntag->m_sect,MB_OK);
}

static void enable_tag_attr(bool enable)
{
  bool error = false;
  EnterCriticalSection(&cso);

  if(kmp_set.size()==1)
  {
    KMP_NSF *kmp = *(kmp_set.begin());
    if(kmp->ntag->WriteEnable(enable))
    {
      HMENU hMenu = GetMenu(hMainWindow);
      CheckMenuRadioItem(hMenu, 4658, 4659, enable?4658:4659, MF_BYCOMMAND);
    }
  }
  else error = true;

  LeaveCriticalSection(&cso);
  
  if(error)
    MessageBox(NULL,"曲の切り替え中であるか、またはNSFが選択されていません。","エラー",MB_OK|MB_ICONEXCLAMATION);
}

static void update_menu_check()
{
  HMENU hMenu = GetMenu(hMainWindow);
  CheckMenuItem(hMenu, 4660, ((*(ui->GetConfig()))["WRITE_TAGINFO"]?MF_CHECKED:MF_UNCHECKED)|MF_BYCOMMAND);
  CheckMenuItem(hMenu, 4661, ((*(ui->GetConfig()))["UPDATE_PLAYLIST"]?MF_CHECKED:MF_UNCHECKED)|MF_BYCOMMAND);
}

static void toggle_enable_tag()
{
  (*(ui->GetConfig()))["WRITE_TAGINFO"] = !(int)(*(ui->GetConfig()))["WRITE_TAGINFO"];
}

static void toggle_time_detection()
{
  (*(ui->GetConfig()))["UPDATE_PLAYLIST"] = !(int)(*(ui->GetConfig()))["UPDATE_PLAYLIST"];
}


static LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  MSG *pmsg = (MSG *)lParam ;

  if(nCode == HC_ACTION&&ui)
  {
    switch (pmsg->message)
  	{
    case WM_COMMAND:
      switch(LOWORD(pmsg->wParam))
      {
      case 4650:
        ui->OpenDialog(NSFplug_UI::DLG_TRACK);
        break;

      case 4651:
        ui->OpenDialog(NSFplug_UI::DLG_MEMORY);
        break;

      case 4652:
        ui->OpenDialog(NSFplug_UI::DLG_EASY);
        break ;

      case 4653:
        ui->OpenDialog(NSFplug_UI::DLG_MIXER);
        break;

      case 4654:
        ui->OpenDialog(NSFplug_UI::DLG_MASK);
        break;

      case 4655:
        ui->OpenDialog(NSFplug_UI::DLG_CONFIG);
        update_menu_check();
        break;

      case 4656:
        delete_tag_info(false);
        break;

      case 4657:
        delete_tag_info(true);
        break;

      case 4658:
        enable_tag_attr(true);
        break;

      case 4659:
        enable_tag_attr(false);
        break;

      case 4660:
        toggle_enable_tag();
        update_menu_check();
        break;
        
      case 4661:
        toggle_time_detection();
        update_menu_check();
        break;

      case 4662:
        ui->OpenDialog(NSFplug_UI::DLG_INFO);
        break;

      case 4663:
        ui->OpenDialog(NSFplug_UI::DLG_PRESET);
        break;

      default:
        break;
      }
      break ;
    default:
      break;
    }
  }

	return CallNextHookEx(hHookGetMsgProc, nCode, wParam, lParam) ;
}

static void GrabWindow(void)
{
  if(Grabbed)
  {
    MessageBox(NULL,"Internal Error in Grabbing KbMediaPlayer.","Error", MB_OK);
    return;
  }

  hMainWindow = FindWindow("TFrmMIDI", NULL) ;

  if((hMainWindow == NULL)||(GetWindowThreadProcessId(hMainWindow,NULL)!=GetCurrentThreadId()))
  {
    MessageBox(NULL,"Can't hook KbMediaPlayer.\r\n"
                    "NSFplug UI has been disabled.","Warning", MB_OK) ;
    return ;
  }
  else
  { 
    HMENU hMenuInsert = GetMenu(hMainWindow) ;
    HMENU hNSF = LoadMenu((HINSTANCE)ghPlugin,MAKEINTRESOURCE(IDR_EXTRA));
    InsertMenu(hMenuInsert,-1,MF_BYPOSITION|MF_POPUP,(UINT_PTR)hNSF,"NSF(&N)") ;
    DrawMenuBar(hMainWindow);
    hHookGetMsgProc = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, NULL, GetCurrentThreadId()) ;
    Grabbed = true;
  }

}

static void UngrabWindow(void)
{
  int nPos ;

  if(!Grabbed)
  {
    MessageBox(NULL,"Internal Error in Ungrabbing KbMediaPlayer.","Error", MB_OK);
    return ;
  }

  HMENU hMenuInsert = GetMenu(hMainWindow);

  if((nPos = FindMenu(hMenuInsert,"NSF(&N)"))>=0)
  {
    DeleteMenu(hMenuInsert, nPos, MF_BYPOSITION);
    DrawMenuBar(hMainWindow);
    hMenuInsert = NULL;
  }

  if(hHookGetMsgProc) UnhookWindowsHookEx(hHookGetMsgProc) ;

  hHookGetMsgProc = 0 ;
  Grabbed = false;
}

static void change_menu_string(HMENU hMenu, UINT id, bool mode, const char *string)
{
  MENUITEMINFO mii;
  mii.cbSize = sizeof(MENUITEMINFO);
  mii.fMask = MIIM_STRING;
  mii.dwTypeData = (LPSTR)string;
  SetMenuItemInfo(hMenu, id, mode, &mii);
}

/** 注意：この関数はCriticalSection内から呼ばれる **/
void kmp_update_menu(KMP_NSF *kmp)
{
  int enable = kmp->ntag->IsWriteEnable();
  HMENU hMenu = GetMenu(hMainWindow);
  update_menu_check();  
  enable_tag_menu(kmp->ntag->IsExistSection());
}

void kmp_hack(NSFplug_UI *pUI)
{
  ui = pUI;
  GrabWindow();
}

void kmp_unhack()
{
  UngrabWindow();
  ui = NULL;
}