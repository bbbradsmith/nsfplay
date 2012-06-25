#ifndef _MGSCWIN_H_
#define _MGSCWIN_H_
#include <stdlib.h>
#include <iostream>
#include "xgm.h"
#include "bdos.h"

class MGSC
{
protected:
  xgm::Z80_CPU cpu;
  xgm::RAM64K ram;
  EMU_BDOS bdos;
  char errmsg[512];

public:
  enum { OPT_TRACK_STATUS=1 };
  MGSC();
  ~MGSC();
#ifndef EMBED_MGSC
  bool LoadCompiler(std::istream *mgsc);
#endif
  bool Compile(std::iostream *mml, std::iostream *mgs, int option);
  void SetConsole(std::istream *in, std::ostream *out);
  const char *GetErrorMessage();
};

#endif _MGSCWIN_H_