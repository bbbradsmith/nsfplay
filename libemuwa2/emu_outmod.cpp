/*
 * emu_outmod.cpp written by Mitsutaka Okazaki 2004.
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
#pragma warning (disable:4311)

#include "emu_outmod.h"

Out_Module EmuOutMod::m_out_mod;
HWAVEOUT EmuOutMod::m_hwo;
WAVEHDR *EmuOutMod::m_wh;
HANDLE EmuOutMod::m_mutex = 0;
int EmuOutMod::m_volume;
char **EmuOutMod::m_buf;
int EmuOutMod::m_buf_idx;
int EmuOutMod::m_buf_idx_played;
int EmuOutMod::m_buf_pos;
int EmuOutMod::m_buf_rest;
int EmuOutMod::m_sec_size;
bool EmuOutMod::m_playing;
int EmuOutMod::m_buf_size=1024 * 4; // ?
int EmuOutMod::m_ring_size=32;   // >= 4

Out_Module *
EmuOutMod::GetOutputModule() {
  m_out_mod.description = "OutModuleEmulator";
  m_out_mod.version = 0x100;
  m_out_mod.About = About;
  m_out_mod.CanWrite = CanWrite;
  m_out_mod.Close = Close;
  m_out_mod.Config = Config;
  m_out_mod.Flush = Flush;
  m_out_mod.GetOutputTime = GetOutputTime;
  m_out_mod.GetWrittenTime = GetWrittenTime;
  m_out_mod.Init = Init;
  m_out_mod.IsPlaying = IsPlaying;
  m_out_mod.Open = Open;
  m_out_mod.Pause = Pause;
  m_out_mod.Quit = Quit;
  m_out_mod.SetPan = SetPan;
  m_out_mod.SetVolume = SetVolume;
  m_out_mod.Write = Write;
  return &m_out_mod;
}

void EmuOutMod::Init() {

  m_mutex = ::CreateMutex( NULL, false, NULL );

  m_buf = new char * [m_ring_size];
  m_wh  = new WAVEHDR [m_ring_size];
  ::memset(m_wh, 0, sizeof(WAVEHDR)*m_ring_size);

  for(int i=0;i<m_ring_size;i++) {
    m_buf[i] = new char [m_buf_size];
    memset(m_buf[i],0,m_buf_size);
    m_wh[i].lpData = m_buf[i] ;
    m_wh[i].dwBufferLength = m_buf_size;
    m_wh[i].dwFlags = WHDR_DONE ;
  }

  m_sec_size = 0;
  m_buf_idx_played = 0;
}

void EmuOutMod::Quit() {
  for(int i=0;i<m_ring_size;i++) {
    waveOutUnprepareHeader(m_hwo,&m_wh[i],sizeof(WAVEHDR));
    delete [] m_buf[i];
  }
  delete [] m_buf;
  delete [] m_wh;

  ::CloseHandle(m_mutex);
  m_mutex = 0;
}

void CALLBACK EmuOutMod::waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD, DWORD) {
  if(uMsg==WOM_DONE) {
    ::WaitForSingleObject(m_mutex, INFINITE);
    m_buf_rest += m_buf_size;
    m_buf_idx_played++;
    ::ReleaseMutex(m_mutex);
  }
}

int EmuOutMod::Open(int rate, int nch, int bps, int buflen, int prebuf) {
  
  WAVEFORMATEX wfx;
  
  wfx.wFormatTag = WAVE_FORMAT_PCM ;
  wfx.nChannels = nch ;
  wfx.nSamplesPerSec = rate ;
  wfx.nAvgBytesPerSec = wfx.nChannels * wfx.nSamplesPerSec * (bps / 8);
  wfx.wBitsPerSample = bps ;
  wfx.nBlockAlign = wfx.nChannels * (bps / 8);
  wfx.cbSize = 0 ;
  MMRESULT result = waveOutOpen(&m_hwo, WAVE_MAPPER, &wfx, (DWORD)waveOutProc, (DWORD)0, CALLBACK_FUNCTION);
  if (result !=MMSYSERR_NOERROR)
  {
    return -1;
  }

  waveOutSetVolume(m_hwo, (DWORD)(m_volume<<16)|m_volume);

  m_buf_pos = 0;
  m_buf_idx = 0;

  ::WaitForSingleObject(m_mutex, INFINITE);
  m_buf_idx_played = 0;
  m_buf_rest = m_buf_size * (m_ring_size-1);
  ::ReleaseMutex(m_mutex);

  m_sec_size = (rate*nch*bps/8); // wave size for 1 second.
  m_playing = true;

  return 0;
}

void EmuOutMod::Close() {
  waveOutReset(m_hwo);
  for(int i=0;i<m_ring_size;i++)
    waveOutUnprepareHeader(m_hwo,&m_wh[i],sizeof(WAVEHDR));
  waveOutClose(m_hwo);
}

int EmuOutMod::GetOutputTime() {
  int result = 0;
  if(m_sec_size)
  {
    ::WaitForSingleObject(m_mutex, INFINITE);
    result = (int)((1000.0 * m_buf_idx_played * m_buf_size) / m_sec_size) ;
    ::ReleaseMutex(m_mutex);
  }
  return result;
}

int EmuOutMod::GetWrittenTime() {
  if(m_sec_size)
    return (int)((1000.0 * m_buf_idx * m_buf_size) / m_sec_size);
  else 
    return 0;
}

int EmuOutMod::IsPlaying() {

  int result = false;

  ::WaitForSingleObject(m_mutex, INFINITE);
  if(m_buf_idx_played < m_buf_idx) 
  {
    result = m_playing;
  }
  ::ReleaseMutex(m_mutex);

  return result;
}

int EmuOutMod::CanWrite() {
  ::WaitForSingleObject(m_mutex, INFINITE);
  int result = m_buf_rest;
  ::ReleaseMutex(m_mutex);
  return result;
}

int EmuOutMod::Write(char *buf, int len) {

  ::WaitForSingleObject(m_mutex, INFINITE);
  int rest = m_buf_rest;
  ::ReleaseMutex(m_mutex);

  if(rest<len) return 1;

  waveOutSetVolume(m_hwo, (DWORD)(m_volume<<16)|m_volume);

  for(int i=0;i<len;i++) {
    m_buf[m_buf_idx%m_ring_size][m_buf_pos++] = buf[i];
    rest--;
    if(m_buf_pos==m_buf_size) {
      waveOutPrepareHeader(m_hwo, &m_wh[m_buf_idx%m_ring_size], sizeof(WAVEHDR)) ;
      waveOutWrite(m_hwo, &m_wh[m_buf_idx%m_ring_size], sizeof(WAVEHDR)) ;
      if(m_playing) waveOutRestart(m_hwo);
      m_buf_pos = 0;
      m_buf_idx ++;
      //waveOutUnprepareHeader(m_hwo, &m_wh[m_buf_idx%m_ring_size], sizeof(WAVEHDR));
    }
  }

  ::WaitForSingleObject(m_mutex, INFINITE);
  m_buf_rest -= len;
  ::ReleaseMutex(m_mutex);

  return 0;
}

void EmuOutMod::Flush(int t) {
  waveOutReset(m_hwo);
  m_buf_pos = 0;

  ::WaitForSingleObject(m_mutex, INFINITE);
  m_buf_rest = m_buf_size * (m_ring_size-1);
  m_buf_idx_played = m_buf_idx = (int)((1.0/1000) * t * m_sec_size / m_buf_size);
  ::ReleaseMutex(m_mutex);
}

int EmuOutMod::Pause(int pause) {
  if(pause) {
    waveOutPause(m_hwo);
    m_playing = false;
  } else {
    waveOutRestart(m_hwo);
    m_playing = true;
  }
  return !pause;
}

void EmuOutMod::SetPan(int pan) {}

void EmuOutMod::SetVolume(int volume) 
{
  m_volume = (volume<<8)&0xFFFF;
}