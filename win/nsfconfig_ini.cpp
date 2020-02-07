#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <string>
#include "nsfconfig_ini.h"

using namespace xgm;


NSFPlayerConfigIni::NSFPlayerConfigIni() : NSFPlayerConfig()
{
}

NSFPlayerConfigIni::~NSFPlayerConfigIni()
{
}

// Load one
bool NSFPlayerConfigIni::Load (const char *path, const char *sect, const char *name)
{
  char temp[256];
  GetPrivateProfileString(sect,name,data[name].GetStr().c_str(),temp,255,path);
  data[name] = vcm::Value(temp);
  return true;
}

// Load all
bool NSFPlayerConfigIni::Load (const char *path, const char *sect)
{
  std::map<std::string, vcm::Value>::iterator it;
  for(it=data.begin(); it!=data.end(); it++)
    Load(path, sect, it->first.c_str());
  return true;
}

bool NSFPlayerConfigIni::Save (const char *path, const char *sect, const char *name)
{
  WritePrivateProfileString (sect, name, data[name], path);
  return true;
}

bool NSFPlayerConfigIni::Save (const char *path, const char *sect)
{
  std::map<std::string, vcm::Value>::iterator it;
  for(it=data.begin(); it!=data.end(); it++)
    Save(path, sect, it->first.c_str());

 return true;
}
