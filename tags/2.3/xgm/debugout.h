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
}

#else
#define DEBUG_OUT printf
#endif

#endif
