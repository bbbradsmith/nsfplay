/*
 * emu_winamp.h written by Mitsutaka Okazaki 2004.
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
#include "emu_outmod.h"
#include "emu_outdisk.h"
#include "winamp2/FRONTEND.h"

class EmuWinamp {
  HMODULE m_dll;
  EmuOutMod m_eom;
  EmuOutDisk m_eod;
  In_Module *m_in_mod;
  char m_fn[2048];
  char m_wo[2048]; // waveout filename
  bool m_playing;
  int m_volume;
public:
  static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
  EmuWinamp(char *dll_name);
  ~EmuWinamp();
  int Play(char *fn);
  void Stop();
  void Pause();
  void SetOutputTime(int time_in_ms);
  int GetOutputTime();
  int GetWrittenTime();
  int IsPlaying();
  int IsPaused();
  void GetFileInfo(char *file, char *title, int *length_in_ms);
  void Info(HWND hWnd);
  void Config(HWND hWnd);
  void About(HWND hWnd);
  void Next();
  void Prev();
  int GetBufferUsage();
  void SetVolume(int volume);
  void Waveout(const char* wavefile);
};


