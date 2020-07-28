#ifndef _DEBUGOUT_H_
#define _DEBUGOUT_H_

#ifdef _MSC_VER
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

namespace xgm
{
  class __DebugOut
  {
  public:
    static void printf (const char *format, ...)
    {
      static char buf[1024];
      va_list argl;

        va_start (argl, format);
        _vsnprintf (buf, 1024, format, argl);
        OutputDebugString (buf);
        va_end (argl);
    }
  };
#define DEBUG_OUT __DebugOut::printf

  // overrides the next output sample (nsfplay.cpp)
  // use to mark events in WAV output, turn quality high
  // only works in _DEBUG build
  extern int debug_mark;
}

#else
#ifdef NDEBUG
#define DEBUG_OUT(...)
#else
#define DEBUG_OUT printf
#endif
#endif

#endif
