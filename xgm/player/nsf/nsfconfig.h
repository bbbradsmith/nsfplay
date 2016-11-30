#ifndef _NSFCONFIG_H_
#define _NSFCONFIG_H_
#include "../player.h"

namespace xgm
{
  /** foCXÌÊµÔ */
  enum DeviceCode
  { APU = 0, DMC, FME7, MMC5, N106, VRC6, VRC7, FDS, NES_DEVICE_MAX };

  const int NES_CHANNEL_MAX = 29;
  
  class NSFPlayerConfig : public PlayerConfig
  {
  public:
    NSFPlayerConfig ();
    virtual ~NSFPlayerConfig();
    virtual bool Load(const char * path, const char *sect);
    virtual bool Save(const char * path, const char *sect);
    virtual bool Load(const char * path, const char *sect, const char *name);
    virtual bool Save(const char * path, const char *sect, const char *name);

    vcm::Value& GetDeviceConfig(int i, std::string key)
    {
      std::lock_guard<std::mutex> mg_(mutex);
      return data[(std::string)dname[i]+"_"+key];
    }

    vcm::Value& GetDeviceOption(int id, int opt)
    {
      std::lock_guard<std::mutex> mg_(mutex);
      static char itoa[] = "0123456789ABCDEF";
      return data[(std::string)dname[id]+"_OPTION"+itoa[opt]];
    }

    // channel mix/pan config
    vcm::Value& GetChannelConfig(int id, std::string key)
    {
      std::lock_guard<std::mutex> mg_(mutex);
      if (id < 0) id = 0;
      if (id >= NES_CHANNEL_MAX) id = NES_CHANNEL_MAX-1;
      auto num = std::to_string(id);
      std::string str;
      str = "CHANNEL_";
      if (id < 10) str += "0";
      str += num;
      str += "_";
      str += key;
      return data[str];
    }

    /** foCXÌ¼O */
    static char *dname[NES_DEVICE_MAX];

    // for channel/pan/mix
    static char *channel_name[NES_CHANNEL_MAX]; // name of channel
    static int channel_device[NES_CHANNEL_MAX]; // device enum of channel
    static int channel_device_index[NES_CHANNEL_MAX]; // device channel index
    static int channel_track[NES_CHANNEL_MAX]; // trackinfo for channel
  };

}// namespace

#endif
