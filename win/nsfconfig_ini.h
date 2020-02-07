#ifndef _NSFCONFIG_INI_H_
#define _NSFCONFIG_INI_H_
#include "../xgm/player/nsf/nsfconfig.h"


namespace xgm
{
  class NSFPlayerConfigIni : public NSFPlayerConfig
  {
  public:
	NSFPlayerConfigIni();
    ~NSFPlayerConfigIni();
    bool Load(const char * path, const char *sect);
    bool Save(const char * path, const char *sect);
    bool Load(const char * path, const char *sect, const char *name);
    bool Save(const char * path, const char *sect, const char *name);
  };

}// namespace

#endif
