#pragma once
#include <map>
#include <set>

class PresetManager
{
private:
  CString path;
  static const char *preset_range[];
  std::map<CString, CString> presetName2Sect;
  std::map<CString, CString> presetSect2Name;
public:
  void Reset( const CString &path );
  bool CreatePreset(xgm::NSFPlayerConfig *, const CString &name );
  bool LoadPreset(xgm::NSFPlayerConfig *, const CString &name);
  const std::set<CString> GetPresetNames();
  const std::set<CString> GetPresetSects();
  bool SavePreset(xgm::NSFPlayerConfig *, const CString &name);
  bool DeletePreset(const CString &name);
};



