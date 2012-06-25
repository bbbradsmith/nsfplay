/*
 * emu_outmod.h written by Mitsutaka Okazaki 2004.
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
#pragma once
#include <windows.h>
#include "winamp2/IN2.H"

class EmuOutMod {
  static Out_Module m_out_mod;
  static HWAVEOUT m_hwo;
  static WAVEHDR *m_wh;
  static HANDLE m_mutex;
  static int m_volume;
  static char **m_buf;
  static int m_buf_idx;
  static int m_buf_idx_played;
  static int m_buf_pos;
  static int m_buf_rest;
  static int m_sec_size;
  static bool m_playing;
public:
  static int m_ring_size;
  static int m_buf_size;
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
  static void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD, DWORD);
};