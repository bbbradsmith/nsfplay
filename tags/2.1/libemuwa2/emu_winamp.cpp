/*
 * emu_winamp.cpp written by Mitsutaka Okazaki 2004.
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
#include <stdio.h>
#include <windows.h>
#include "emu_winamp.h"

//DUMMY FUNCTIONS
static void SAVSAInit(int maxlatency_in_ms, int srate){}
static void SAVSADeInit(){}
static void SAAddPCMData(void *PCMData, int nch, int bps, int timestamp){}
static int  SAGetMode(){ return 1; }
static int SAAdd(void *data, int timestamp, int csa){ return 0; }
static void VSAAddPCMData(void *PCMData, int nch, int bps, int timestamp){}
static int VSAGetMode(int *specNch, int *waveNch){ return 1; }
static int VSAAdd(void *data, int timestamp){ return 0; }
static void VSASetInfo(int nch, int srate){}
static int dsp_isactive(){ return 0; }
static int dsp_dosamples(short int *samples, int numsamples, int bps, int nch, int srate)
{ return numsamples*2;}
static void SetInfo(int bitrate, int srate, int stereo, int synched){}

typedef In_Module *(*WINAMP_GET_IN_MODULE)() ;

LRESULT CALLBACK EmuWinamp::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {

  EmuWinamp *__this = (EmuWinamp *)GetProp(hWnd,"WinampEmu");

  if(__this) {

    if(uMsg==WM_WA_IPC) {
      //printf("WM_WA_IPC: %d %d\n", wParam, lParam);
    } else if(uMsg==WM_COPYDATA) {
      COPYDATASTRUCT *cds = (COPYDATASTRUCT *)lParam;
      if(cds->dwData == IPC_PLAYFILE) {
        __this->Stop();
        __this->Play((char *)cds->lpData);
      }

    } else if(uMsg==WM_COMMAND) {
      OutputDebugString("WM_COMMAND\n");
      //printf("WM_COMMAND: %d %d\n", wParam, lParam);
      if(wParam==WINAMP_BUTTON2) {
        __this->Stop();
        __this->Play(__this->m_fn);
      }
    } else if(uMsg == (WM_USER+2) ) {
      __this->Next();
      //MessageBox(NULL,"WM_WA_MPEG_EOF","EOF",MB_OK);
    }

  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

EmuWinamp::EmuWinamp(char *dll_name) {

  m_dll = LoadLibrary(dll_name);
  if(m_dll==NULL)
  {
      MessageBox(NULL,"Can't load the plugin!","ERROR",MB_OK);
      throw 1;
  }

  WINAMP_GET_IN_MODULE f = (WINAMP_GET_IN_MODULE)GetProcAddress(m_dll,"winampGetInModule2"); 
  if(f==NULL) {
    MessageBox(NULL,"Can't load winampGetInModule2!","ERROR",MB_OK);
    throw 1;
  }
  m_in_mod = f();
  m_in_mod->SAAdd = SAAdd;
  m_in_mod->SAAddPCMData = SAAddPCMData;
  m_in_mod->SAGetMode = SAGetMode;
  m_in_mod->SAVSADeInit = SAVSADeInit;
  m_in_mod->SAVSAInit = SAVSAInit;
  m_in_mod->dsp_dosamples = dsp_dosamples;
  m_in_mod->dsp_isactive = dsp_isactive;
  m_in_mod->VSAAdd = VSAAdd;
  m_in_mod->VSAAddPCMData = VSAAddPCMData;
  m_in_mod->VSAGetMode = VSAGetMode;
  m_in_mod->VSASetInfo = VSASetInfo;
  m_in_mod->SetInfo = SetInfo;
  m_in_mod->outMod = m_eom.GetOutputModule();

  WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, 
                    GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                    "WinampEmu", NULL };

  RegisterClassEx( &wc );
  m_in_mod->hMainWindow = CreateWindow(
    "WinampEmu","Winamp Emulation",WS_POPUP,0,0,32,32,NULL,NULL,GetModuleHandle(NULL),this);
  SetProp(m_in_mod->hMainWindow,"WinampEmu",(HANDLE)this);
  printf("hMainWindow :%p\n",m_in_mod->hMainWindow);
  m_in_mod->hDllInstance = m_dll;

  m_in_mod->Init();
  m_in_mod->outMod->Init();
  m_playing = false;
  m_fn[0] = '\0';
  m_wo[0] = 0;
  m_volume = 255;
}

EmuWinamp::~EmuWinamp() {
  if(m_playing) {
    m_in_mod->Stop();
  }
  m_in_mod->outMod->Quit();
  m_in_mod->Quit();
  RemoveProp(m_in_mod->hMainWindow,"WinampEmu");
  DestroyWindow(m_in_mod->hMainWindow);
  UnregisterClass("WinampEmu",GetModuleHandle(NULL));
  FreeLibrary(m_dll);
}

int EmuWinamp::Play(char *fn) {

  if(m_playing) Stop();

  if(fn) strncpy(m_fn,fn,2048);

  if (m_wo[0]) // waveout
  {
    // temporarily replace outMod, will get restored at Stop() or Play()
    m_in_mod->outMod = m_eod.GetOutputModule();
    m_eod.SetFilename(m_wo);
  }
  else
  {
    // restore outMod in case it has changed
    m_in_mod->outMod = m_eom.GetOutputModule();
  }
  
  int retcode = m_in_mod->Play(m_fn);

  m_wo[0] = 0; // 

  if(!retcode) 
    m_playing = true;
  else
    m_playing = false;

  SetVolume(m_volume);
  return retcode;
}

void EmuWinamp::SetVolume(int volume) {
  m_in_mod->outMod->SetVolume(volume);
  m_volume = volume;
}

void EmuWinamp::About(HWND hWnd) {
  if(hWnd==NULL)
    m_in_mod->About(m_in_mod->hMainWindow);
  else
    m_in_mod->About(hWnd);
}

void EmuWinamp::Config(HWND hWnd) {
  if(hWnd)
    m_in_mod->Config(hWnd);
  else
    m_in_mod->Config(m_in_mod->hMainWindow);
}

int EmuWinamp::GetOutputTime() {
  return m_in_mod->GetOutputTime();
}

void EmuWinamp::SetOutputTime(int time_in_ms) {
  m_in_mod->SetOutputTime(time_in_ms);
}

void EmuWinamp::GetFileInfo(char *file, char *title, int *length_in_ms) {
  if(m_playing)
    m_in_mod->GetFileInfo(NULL,title,length_in_ms);
  else {
    m_in_mod->GetFileInfo(file,title,length_in_ms);
  }
  if(*length_in_ms<=0) *length_in_ms = 300000;
}

int EmuWinamp::IsPlaying() {
  return m_playing&&!m_in_mod->IsPaused();
}

int EmuWinamp::IsPaused() {
  return m_in_mod->IsPaused();
}

int EmuWinamp::GetWrittenTime() {
  return m_in_mod->outMod->GetWrittenTime();
}

void EmuWinamp::Stop() {
  if(m_playing) {
    m_in_mod->Stop();
    m_playing = false;
  }
  // restore outMod in case it has changed
  m_in_mod->outMod = m_eom.GetOutputModule();
}

void EmuWinamp::Pause() {
  if(m_in_mod->IsPaused())
    m_in_mod->UnPause();
  else
    m_in_mod->Pause();
}

int EmuWinamp::GetBufferUsage() {
  int bufmax = m_eom.m_buf_size * (m_eom.m_ring_size-1);  
  return (bufmax - m_in_mod->outMod->CanWrite() ) * 100 / bufmax;
}

void EmuWinamp::Info(HWND hWnd) {
  if(hWnd)
    m_in_mod->InfoBox(m_fn,hWnd);
  else
    m_in_mod->InfoBox(m_fn,m_in_mod->hMainWindow);
}

void EmuWinamp::Next() {
  SendMessage(m_in_mod->hMainWindow,WM_COMMAND,WINAMP_BUTTON5,0);
}

void EmuWinamp::Prev() {
  SendMessage(m_in_mod->hMainWindow,WM_COMMAND,WINAMP_BUTTON1,0);
}

void EmuWinamp::Waveout(const char* filename) {
  ::strcpy_s(m_wo, sizeof(m_wo), filename);
}
