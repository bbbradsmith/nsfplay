#include "stdafx.h"
#include "nsf_setup.h"
#include "PresetManager.h"
#include <vector>

const char *
PresetManager::preset_range[] = 
{
  "APU1_VOLUME", "APU2_VOLUME", "FDS_VOLUME",  "MMC5_VOLUME",
  "5B_VOLUME",   "VRC6_VOLUME", "VRC7_VOLUME", "N163_VOLUME",
  "APU1_FC", "APU2_FC", "FDS_FC",  "MMC5_FC",
  "5B_FC",   "VRC6_FC", "VRC7_FC", "N163_FC",
  "APU1_FR", "APU2_FR", "FDS_FR",  "MMC5_FR",
  "5B_FR",   "VRC6_FR", "VRC7_FR", "N163_FR",
  "COMP_LIMIT", "COMP_THRESHOLD", "COMP_VELOCITY",
  "HPF", "ENABLE_DCF", "LPF",

  NULL
};

void PresetManager::Reset(const CString &filepath)
{
  path = filepath;

  char buf[128*64+2];
  GetPrivateProfileSectionNames(buf,128*64+2,path);

  CString name;
  char *p = buf;
  std::set<CString> sects;

  while(1)
  {
    name = "";
    
    while(*p!='\0')
      name.AppendChar(*p++);
    p++;

    if(name == "") break;

    if(name.Left(6)=="PRESET")
      sects.insert(name);
  }

  presetSect2Name.clear();
  presetName2Sect.clear();

  std::set<CString>::iterator it;
  for(it=sects.begin();it!=sects.end();it++)
  {
    GetPrivateProfileString((*it),"NAME","ERROR",buf,128,path);
    presetName2Sect[(CString)buf]=(*it);
    presetSect2Name[(*it)] = (CString)buf;
  }
}

bool
PresetManager::CreatePreset( xgm::NSFPlayerConfig *cfg, const CString &name )
{
  CString sect;

  for(int i=0;i<128;i++)
  {
    sect = "PRESET";
    sect.AppendFormat("%04d",i);

    if(presetSect2Name.find(sect)==presetSect2Name.end())
    {
      presetName2Sect[name] = sect;
      presetSect2Name[sect] = name;
      WritePrivateProfileString( sect, "NAME", name, path );
      return SavePreset(cfg,name);
    }

  }

  return false;
}

bool
PresetManager::DeletePreset( const CString &name )
{
  if(presetName2Sect.find(name)==presetName2Sect.end())
    return false;

  CString sect = presetName2Sect[name];
  WritePrivateProfileSection( sect,NULL,path);
  presetName2Sect.erase( name );
  presetSect2Name.erase( sect );
  return true;
}

bool
PresetManager::LoadPreset(xgm::NSFPlayerConfig *cfg, const CString &name)
{
  for(int i=0;preset_range[i]!=NULL;i++)
    cfg->Load(path, presetName2Sect[name], preset_range[i]);

  cfg->Notify(-1);
  return true;
}

bool
PresetManager::SavePreset(xgm::NSFPlayerConfig *cfg, const CString &name)
{
  for(int i=0;preset_range[i]!=NULL;i++)
    cfg->Save(path, presetName2Sect[name], preset_range[i]);
  return true;
}

const std::set<CString> 
PresetManager::GetPresetNames()
{
  std::set<CString> ret;
  std::map<CString,CString>::iterator it;
  for(it=presetName2Sect.begin();it!=presetName2Sect.end();it++)
    ret.insert(it->first);

  return ret;
}

const std::set<CString> 
PresetManager::GetPresetSects()
{
  std::set<CString> ret;
  std::map<CString,CString>::iterator it;
  for(it=presetName2Sect.begin();it!=presetName2Sect.end();it++)
    ret.insert(it->second);

  return ret;
}

