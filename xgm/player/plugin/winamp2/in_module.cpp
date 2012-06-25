#include "in_module.h"

using namespace xgm;

WA2InputModule::WA2InputModule(Player *p, PlayerConfig *c, SoundData *s) 
  : pl(p), cf(c), sdat(s)
{
  pl->SetConfig(cf);
  seek_flag = false;
  pause_flag = false;
  thread_id = 0;
  hPlayThread = NULL;
  killPlayThread = 0;
}

WA2InputModule::~WA2InputModule()
{
  cf->DetachObserver(pl);
}

void WA2InputModule::SetModule(In_Module *m)
{ 
  pMod = m; 
}

void WA2InputModule::PreAutoStop()
{
  PostMessage(pMod->hMainWindow, WM_WA_MPEG_EOF, 0, 0);
}

DWORD WINAPI __stdcall WA2InputModule::PlayThread(WA2InputModule *_t)
{
  xgm::INT16 *packet_buf;
  int packet_size = 2048 * _t->nch * _t->bps / 8; // バッファサイズ2048bytes固定
  int blank_time = 10 * _t->rate / 48000; // 最初の数パケットは無音にする(DirectX plugin対策)
  int wsize; // dsp処理後の書き込みサイズ
  int sample_size = _t->nch * _t->bps / 8;

  packet_buf = new xgm::INT16[packet_size]; // dspのために2倍の領域を確保
  memset(packet_buf,0,packet_size*sizeof(xgm::INT16));

  while(!_t->killPlayThread)
  {
    if(_t->reset_flag)
    {
      _t->pl->Reset();
      _t->decode_pos = 0;
      _t->reset_flag = false;
    }
    
    if(_t->pl->IsStopped())
    {
      if(_t->pMod->outMod->IsPlaying())
        continue;
      else
      {
        _t->PreAutoStop();
        break;
      }
    }

    if(_t->seek_flag)
    {
      int to_skip = _t->seek_pos - _t->decode_pos;
      int packet_samples = packet_size / sample_size;

      if(to_skip > packet_samples)
      {
          _t->decode_pos += _t->pl->Skip(packet_samples);
      }
      else
      {
          _t->decode_pos += _t->pl->Skip(to_skip);
          _t->pMod->outMod->Flush(POS2MS(_t->decode_pos,_t->rate));
          _t->seek_flag = false;
      }
      continue;
    }
    
    if(_t->pMod->outMod->CanWrite() >= (packet_size<<(_t->pMod->dsp_isactive()?1:0)))
    {
      if(blank_time)
      {
        memset(packet_buf,0,packet_size*sizeof(xgm::INT16));
        blank_time--;
      }
      else
      {
        _t->decode_pos += _t->pl->Render(packet_buf,packet_size/sample_size);
      }

      if(_t->pMod->dsp_isactive())
        wsize = _t->pMod->dsp_dosamples((short *)packet_buf,2048,_t->bps,_t->nch,_t->rate) * (_t->nch*_t->bps/8);
      else 
        wsize = packet_size;

      _t->pMod->outMod->Write((char *)packet_buf, wsize);
      //_t->decode_pos += packet_size / sample_size; // BS using Render return instead
      // (marking blank time as encoded results in inaccurate seek times being reported to keyboard view)
      _t->pMod->SAAddPCMData(packet_buf,_t->nch,_t->bps,POS2MS(_t->decode_pos,_t->rate));
      _t->pMod->VSAAddPCMData(packet_buf,_t->nch,_t->bps,POS2MS(_t->decode_pos,_t->rate));
    }
    else Sleep(10);
  }

  
  // 最後の出力値を必ず0にする
  /*
  while(_t->pMod->outMod->CanWrite()<16);
  memset(packet_buf,0,16);
  _t->pMod->outMod->Write((char *)packet_buf,16);
  */

  delete [] packet_buf;

  return 0;
}

void WA2InputModule::StartPlayThread()
{
  assert(hPlayThread==INVALID_HANDLE_VALUE);
  killPlayThread = 0;

  hPlayThread = (HANDLE)CreateThread(
    NULL,
    0,
    (LPTHREAD_START_ROUTINE)PlayThread,
    this,
    0,
    &thread_id
    );
}

void WA2InputModule::StopPlayThread()
{
  if(hPlayThread==INVALID_HANDLE_VALUE) return;
  killPlayThread = 1;
  if(WaitForSingleObject(hPlayThread,2000)==WAIT_TIMEOUT){
    MessageBox(NULL,"Error asking thread to die!\n", "Error", MB_OK);
    TerminateThread(hPlayThread,0);
  }
  CloseHandle(hPlayThread);
  hPlayThread = INVALID_HANDLE_VALUE;
}

void WA2InputModule::Config(HWND hParent)
{
  MessageBox(hParent, "No Configuration", "Sorry", MB_OK);
}

void WA2InputModule::About(HWND hParent)
{
  MessageBox(hParent, "No Information", "About", MB_OK);
}

void WA2InputModule::Init()
{
  hPlayThread = INVALID_HANDLE_VALUE;
}

void WA2InputModule::Quit()
{
}

void WA2InputModule::GetFileInfo(char *file, char *title, int *length_in_ms)
{
}

int WA2InputModule::InfoBox(char *file, HWND hParent)
{
  MessageBox(hParent, "No info box", "Info", MB_OK);
  return 0;
}

int WA2InputModule::IsOurFile(char *fn)
{
  return 0;
}

bool WA2InputModule::LoadFile(char *fn)
{
  return sdat->LoadFile(fn)&&pl->Load(sdat);
}

int WA2InputModule::Play(char *fn)
{
  int maxlatency;

  if(LoadFile(fn)==false)
    return 1;

  rate = (*cf)["RATE"];
  bps  = (*cf)["BPS"];
  nch  = (*cf)["NCH"];

  pl->SetPlayFreq(rate);
  pl->SetChannels(nch);
  //pl->Reset();

  maxlatency = pMod->outMod->Open(rate,nch,bps, -1,-1);
	if(maxlatency < 0) return 1;

  pMod->SetInfo(rate*bps*nch/1000, rate/1000, nch, 1);
  pMod->SAVSAInit(maxlatency, rate);
	pMod->VSASetInfo(rate,nch);
  pMod->outMod->SetVolume(-666);
  pMod->outMod->Flush(0);
  pause_flag = false;
  seek_flag = false;
  reset_flag = true;
  decode_pos = 0;

  StartPlayThread();
  return 0;
}

void WA2InputModule::Pause()
{
  pause_flag = true;
  pMod->outMod->Pause(1);
}

void WA2InputModule::UnPause()
{
  pause_flag = false;
  pMod->outMod->Pause(0);
}

int WA2InputModule::IsPaused()
{
  return pause_flag;
}

void WA2InputModule::Stop()
{
  StopPlayThread();
  pMod->outMod->Close();
  pMod->SAVSADeInit();
}

int WA2InputModule::GetLength()
{
  return pl->GetLength();
}

int WA2InputModule::GetOutputTime()
{

  if(!pMod->outMod||!pMod->outMod->GetOutputTime)
  {
    return 0;
  }
  else
  {
    //DEBUG_OUT("d/w/o: %08d / %08d / %08d\n",POS2MS(decode_pos,rate),pMod->outMod->GetWrittenTime(),pMod->outMod->GetOutputTime());
    return seek_flag?POS2MS(decode_pos,rate):pMod->outMod->GetOutputTime();
  }
}

void WA2InputModule::SetOutputTime(int time_in_ms)
{
  StopPlayThread();
  seek_flag = true;
  seek_pos = MS2POS(time_in_ms,rate);
  if(time_in_ms < POS2MS(decode_pos,rate))
  {
    pl->Reset();
    decode_pos = 0;
  }
  StartPlayThread();
}

void WA2InputModule::SetVolume(int volume)
{
  pMod->outMod->SetVolume(volume);
}

void WA2InputModule::SetPan(int pan)
{
  pMod->outMod->SetPan(pan);
}

void WA2InputModule::EQSet(int on, char data[10], int preamp)
{
}

void WA2InputModule::SetPlayer(Player *p){ pl = p; }

void WA2InputModule::SetPlayerConfig(PlayerConfig *p){ cf = p; }

void WA2InputModule::SetSoundData(SoundData *s){ sdat = s; }

Player *WA2InputModule::GetPlayer(){ return pl; }

PlayerConfig *WA2InputModule::GetConfig(){ return cf; }

SoundData *WA2InputModule::GetSoundData(){ return sdat; }

void WA2InputModule::QueueFile(const char *file)
{
  COPYDATASTRUCT cds;
  cds.dwData = IPC_PLAYFILE;
  cds.lpData = (void *) file;
  cds.cbData = strlen(file)+1; // include space for null char
  SendMessage(pMod->hMainWindow, WM_COPYDATA,(WPARAM)NULL,(LPARAM)&cds);
}

void WA2InputModule::PlayStart()
{
  SendMessage(pMod->hMainWindow,WM_WA_IPC,0,IPC_STARTPLAY);
}

void WA2InputModule::ClearList()
{
  SendMessage(pMod->hMainWindow,WM_WA_IPC,0,IPC_DELETE);
}
