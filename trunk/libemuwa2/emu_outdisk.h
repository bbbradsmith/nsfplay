/*
 * emu_outdisk.h
 */
#pragma once
#include <windows.h>
#include <mmsystem.h>

#include "winamp2/IN2.H"

class EmuOutDisk {
  static Out_Module m_out_mod;
  static unsigned int m_sec_size;
  static unsigned int m_written;
  static bool m_playing;
  static const char* m_filename;
  static HMMIO m_file;
  static MMCKINFO m_ckRIFF;
  static MMCKINFO m_ck;
  static MMIOINFO m_info;
public:
  Out_Module *GetOutputModule();
  static void About(HWND){}
  static void Config(HWND){}
  static void Init();
  static void Quit();
  static int Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms); 
  static void Close();
  static int Write(char *buf, int len);	
  static int CanWrite();
  static int IsPlaying();
  static int Pause(int pause);
  static void SetVolume(int volume);
  static void SetPan(int pan);
  static void Flush(int t); 
  static int GetOutputTime();
  static int GetWrittenTime();
  static void SetFilename(const char* filename);
};