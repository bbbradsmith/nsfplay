/*
 * emu_outdisk.cpp
 */

#include "emu_outdisk.h"

Out_Module EmuOutDisk::m_out_mod;
unsigned int EmuOutDisk::m_sec_size;
unsigned int EmuOutDisk::m_written;
bool EmuOutDisk::m_playing;
const char* EmuOutDisk::m_filename = NULL;
HMMIO EmuOutDisk::m_file = NULL;
MMCKINFO EmuOutDisk::m_ckRIFF;
MMCKINFO EmuOutDisk::m_ck;
MMIOINFO EmuOutDisk::m_info;

Out_Module *
EmuOutDisk::GetOutputModule() {
  m_out_mod.description = "OutDiskEmulator";
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

void EmuOutDisk::Init()
{
  m_filename = NULL;
  m_file = NULL;
}

void EmuOutDisk::Quit()
{
}

int EmuOutDisk::Open(int rate, int nch, int bps, int buflen, int prebuf) {

  if (!m_filename)
  {
      return -1;
  }

  // open MMIO file

  PCMWAVEFORMAT pwf;
  pwf.wf.wFormatTag = WAVE_FORMAT_PCM;
  pwf.wf.nChannels = nch;
  pwf.wf.nSamplesPerSec  = rate;
  pwf.wf.nBlockAlign = nch * (bps / 8);
  pwf.wf.nAvgBytesPerSec = nch * rate * (bps / 8);
  pwf.wBitsPerSample = bps;

  m_file = mmioOpen((LPSTR)m_filename, NULL, MMIO_ALLOCBUF | MMIO_READWRITE | MMIO_CREATE);
  if (m_file == NULL)
  {
    return -1;
  }

  m_ckRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');
  m_ckRIFF.cksize  = 0;

  MMRESULT merr;
  merr = mmioCreateChunk(m_file, &m_ckRIFF, MMIO_CREATERIFF);
  if (merr != MMSYSERR_NOERROR)
  {
    return -1;
  }

  m_ck.ckid = mmioFOURCC('f', 'm', 't', ' ');
  m_ck.cksize = sizeof(PCMWAVEFORMAT);
  merr = mmioCreateChunk(m_file, &m_ck, 0);
  if (merr != MMSYSERR_NOERROR)
  {
    return -1;
  }

  mmioWrite(m_file, (HPSTR)&pwf, sizeof(PCMWAVEFORMAT));
  mmioAscend(m_file, &m_ck, 0);

  m_ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
  m_ck.cksize = 0;

  merr = mmioCreateChunk(m_file, &m_ck, 0);
  if (merr != MMSYSERR_NOERROR)
  {
    return -1;
  }

  mmioGetInfo(m_file, &m_info, 0);

  // reset playing data

  m_sec_size = (rate*nch*bps/8); // wave size for 1 second.
  m_playing = true;
  m_written = 0;

  return 0;
}

void EmuOutDisk::Close()
{
  // close the file

  m_info.dwFlags |= MMIO_DIRTY;
  mmioSetInfo(m_file, &m_info, 0);

  mmioAscend(m_file, &m_ck, 0);
  mmioAscend(m_file, &m_ckRIFF, 0);

  mmioSeek(m_file, 0, SEEK_SET);
  mmioDescend(m_file, &m_ckRIFF, NULL, 0);

  mmioClose(m_file, 0);
}

int EmuOutDisk::GetOutputTime() {
  return GetWrittenTime();
}

int EmuOutDisk::GetWrittenTime() {
  if(m_sec_size)
  {
    __int64 r = __int64(1000) * __int64(m_written) / __int64(m_sec_size);
    return int(r);
    //return (1000 * m_written) / m_sec_size;
  }
  return 0;
}

int EmuOutDisk::IsPlaying() {
  //return m_playing;
  // is used to determine if has reached the end of buffer yet (always yes)
  return false;
}

int EmuOutDisk::CanWrite() {
  return 1024 * 64;
}

int EmuOutDisk::Write(char *buf, int len) {

  for(int i=0; i < len; ++i)
  {
    if (m_info.pchNext == m_info.pchEndWrite)
    {
      m_info.dwFlags |= MMIO_DIRTY; 
      mmioAdvance(m_file, &m_info, MMIO_WRITE);
    }
    *((BYTE*)m_info.pchNext) = *((BYTE*)buf + i); 
    m_info.pchNext++;
  }

  m_written += len;

  return 0;
}

void EmuOutDisk::Flush(int t) {
}

int EmuOutDisk::Pause(int pause) {
  if(pause) {
    m_playing = false;
  } else {
    m_playing = true;
  }
  return !pause;
}

void EmuOutDisk::SetPan(int pan)
{
}

void EmuOutDisk::SetVolume(int volume) 
{
}

void EmuOutDisk::SetFilename(const char* filename)
{
    m_filename = filename;
}
