#include "in_module.h"

using namespace xgm;

WA2InputModuleMSP *WA2InputModuleMSP::_this;

void WA2InputModuleMSP::GrabWinamp()
{
  if(!grabbed)
  {
    if(_this!=NULL)
    {
      MessageBox(NULL,"Winamp's window is already hooked.", "Fatal Error.", MB_OK);
    }
    else
    {
      DWORD dwThreadId = GetWindowThreadProcessId(pMod->hMainWindow, NULL);
      assert(dwThreadId);
      hHookCallWndProc = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc, NULL, dwThreadId);
      _this = this;
    }
    grabbed = true;
  }
}

LRESULT CALLBACK WA2InputModuleMSP::CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  CWPSTRUCT *pmsg = (CWPSTRUCT *)lParam ;

  if(nCode == HC_ACTION){
    switch (pmsg->message)
    {
    case WM_USER+2413:
      if(wParam==0xdeadbeef,_this->sdat->enable_multi_tracks)
      {
        _this->pl->SetSong((int)pmsg->lParam);
        _this->keep_song = true;
        PostMessage(_this->pMod->hMainWindow, WM_COMMAND, WINAMP_BUTTON2, 0) ;
      }
      break;

    case WM_COMMAND:
    case WM_SYSCOMMAND:
      switch(pmsg->wParam)
      {

      case WINAMP_BUTTON5_CTRL:
        if(_this->sdat->enable_multi_tracks)
        {
          _this->pl->NextSong(10);
          _this->keep_song = true;
          PostMessage(_this->pMod->hMainWindow, WM_COMMAND, WINAMP_BUTTON2, 0) ;
        }
        break;

      case WINAMP_BUTTON5:
        if(_this->sdat->enable_multi_tracks)
        {
          _this->pl->NextSong(1);
          _this->keep_song = true;
          PostMessage(_this->pMod->hMainWindow, WM_COMMAND, WINAMP_BUTTON2, 0) ;
        }
        break ;

      case WINAMP_BUTTON1_CTRL:
        if(_this->sdat->enable_multi_tracks)
        {
          _this->pl->PrevSong(10);
          _this->keep_song = true;
          PostMessage(_this->pMod->hMainWindow, WM_COMMAND, WINAMP_BUTTON2, 0) ;
        }
        break;

      case WINAMP_BUTTON1:
        if(_this->sdat->enable_multi_tracks)
        {
          _this->pl->PrevSong(1);
          _this->keep_song = true;
          PostMessage(_this->pMod->hMainWindow, WM_COMMAND, WINAMP_BUTTON2, 0) ;
        }
        break ;
      }
      break ;
    }
  }
  return CallNextHookEx(_this->hHookCallWndProc, nCode, wParam, lParam);
}

void WA2InputModuleMSP::UnGrabWinamp()
{
  if(hHookCallWndProc)
  {
    UnhookWindowsHookEx(hHookCallWndProc);
    hHookCallWndProc = NULL;
    _this = NULL;
  }
}

void WA2InputModuleMSP::PreAutoStop()
{
  if(sdat->enable_multi_tracks)
  {
    if(pl->NextSong(1))
    {
      keep_song = true;
      PostMessage(pMod->hMainWindow, WM_COMMAND, WINAMP_BUTTON2, 0); // 曲を一曲進めて，再生ボタンを押す．
      return;
    }
  }
  PostMessage(pMod->hMainWindow, WM_WA_MPEG_EOF, 0, 0);
}

void WA2InputModuleMSP::Init()
{
  grabbed = false;
  hHookCallWndProc = NULL;
  WA2InputModule::Init();
}

void WA2InputModuleMSP::Quit()
{
  if(grabbed)
  {
    UnGrabWinamp();
    grabbed = false;
  }
  WA2InputModule::Quit();
}

bool WA2InputModuleMSP::LoadFile(char *fn)
{
  int song=-1;

  if(strcmp(last_fn,fn))
  {
    strncpy(last_fn,fn,511);
    last_fn[511]='\0';
    keep_song = false;
  }
  else if(keep_song)
  {
    song = sdat->GetSong();
  }

  if(!sdat->LoadFile(fn)||!pl->Load(sdat))
    return false;

  if(song>=0)
    sdat->SetSong(song);

  return true;
}

int WA2InputModuleMSP::Play(char *fn)
{
  if(!grabbed) GrabWinamp();
  return WA2InputModule::Play(fn);
}

void WA2InputModuleMSP::Keep_Song(bool b)
{
  keep_song = b;
}

WA2InputModuleMSP::WA2InputModuleMSP(PlayerMSP *p, PlayerConfig *c, SoundDataMSP *s) 
  : pl(p), sdat(s), WA2InputModule(p,c,s)
{
  memset(last_fn,0,512);
}

WA2InputModuleMSP::~WA2InputModuleMSP(){}
