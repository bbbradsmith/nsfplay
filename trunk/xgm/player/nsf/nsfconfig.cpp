#include <stdio.h>
#include <string.h>
#include "nsfconfig.h"
#include "nsfplay.h"

#include "../../devices/sound/nes_apu.h"
#include "../../devices/sound/nes_dmc.h"
#include "../../devices/sound/nes_fds.h"
#include "../../devices/sound/nes_mmc5.h"

using namespace xgm;

char *NSFPlayerConfig::dname[NES_DEVICE_MAX] =
  { "APU1", "APU2", "5B", "MMC5", "N163", "VRC6", "VRC7", "FDS" };

char *NSFPlayerConfig::channel_name[NES_CHANNEL_MAX] =
  {
      "SQR0", "SQR1", "TRI", "NOISE", "DMC",
      "FDS",
      "MMC5:S0", "MMC5:S1", "MMC5:PCM",
      "5B:0", "5B:1", "5B:2",
      "VRC6:S0", "VRC6:S1", "VRC6:SAW",
      "VRC7:0", "VRC7:1", "VRC7:2", "VRC7:3", "VRC7:4", "VRC7:5",
      "N163:0", "N163:1", "N163:2", "N163:3", "N163:4", "N163:5", "N163:6", "N163:7"
  };

int NSFPlayerConfig::channel_device[NES_CHANNEL_MAX] =
  {
      APU, APU, DMC, DMC, DMC,
      FDS,
      MMC5, MMC5, MMC5,
      FME7, FME7, FME7,
      VRC6, VRC6, VRC6,
      VRC7, VRC7, VRC7, VRC7, VRC7, VRC7,
      N106, N106, N106, N106, N106, N106, N106, N106
  };

int NSFPlayerConfig::channel_device_index[NES_CHANNEL_MAX] =
  {
      0, 1, 0, 1, 2,
      0,
      0, 1, 2,
      0, 1, 2,
      0, 1, 2,
      0, 1, 2, 3, 4, 5,
      0, 1, 2, 3, 4, 5, 6, 7
  };

int NSFPlayerConfig::channel_track[NES_CHANNEL_MAX] =
  {
    NSFPlayer::APU1_TRK0, NSFPlayer::APU1_TRK1, NSFPlayer::APU2_TRK0,
    NSFPlayer::APU2_TRK1, NSFPlayer::APU2_TRK2,
    NSFPlayer::FDS_TRK0,
    NSFPlayer::MMC5_TRK0, NSFPlayer::MMC5_TRK1, NSFPlayer::MMC5_TRK2,
    NSFPlayer::FME7_TRK0, NSFPlayer::FME7_TRK1, NSFPlayer::FME7_TRK2,
    NSFPlayer::VRC6_TRK0, NSFPlayer::VRC6_TRK1, NSFPlayer::VRC6_TRK2,
    NSFPlayer::VRC7_TRK0, NSFPlayer::VRC7_TRK1, NSFPlayer::VRC7_TRK2,
    NSFPlayer::VRC7_TRK3, NSFPlayer::VRC7_TRK4, NSFPlayer::VRC7_TRK5,
    NSFPlayer::N106_TRK0, NSFPlayer::N106_TRK1, NSFPlayer::N106_TRK2, NSFPlayer::N106_TRK3,
    NSFPlayer::N106_TRK4, NSFPlayer::N106_TRK5, NSFPlayer::N106_TRK6, NSFPlayer::N106_TRK7
  };

static const char* DEFAULT_CHANNEL_COL[NES_CHANNEL_MAX] = {
  "FF0000", "FF0000", //APU1
  "00FF00", "00FF00", "000000", //APU2
  "0080FF", //FDS
  "FFC000", "FFC000", "000000", //MMC5
  "0000FF", "0000FF", "0000FF", // 0000FF, 000000, //FME7
  "FF8000", "FF8000", "FF8000", //VRC6
  "8000FF", "8000FF", "8000FF", "8000FF", "8000FF", "8000FF",//VRC7
  "FF0080", "FF0080", "FF0080", "FF0080", "FF0080", "FF0080", "FF0080", "FF0080"  //N106
  };
static const char* DEFAULT_5B_ENVELOPE_COL = "0000FF";
static const char* DEFAULT_5B_NOISE_COL = "000000";

NSFPlayerConfig::NSFPlayerConfig () : PlayerConfig ()
{
  int i, j;

  CreateValue("RATE", 44100);
  CreateValue("NCH",  2);
  CreateValue("BPS",  16);
  CreateValue("MASK", 0);
  CreateValue("PLAY_TIME", 60*5*1000);
  CreateValue("FADE_TIME", 5*1000);
  CreateValue("STOP_SEC", 3);
  CreateValue("LOOP_NUM", 2);
  CreateValue("AUTO_STOP", 1);
  CreateValue("AUTO_DETECT", 1);
  CreateValue("DETECT_TIME", 30*1000);
  CreateValue("DETECT_INT", 5000);
  CreateValue("LPF", 112);
  CreateValue("HPF", 164);
  CreateValue("TITLE_FORMAT", "%L (%n/%e) %T - %A");
  CreateValue("DETECT_ALT", 0);
  CreateValue("VSYNC_ADJUST", 0);
  CreateValue("MULT_SPEED", 256); // clock speed multiplier
  CreateValue("VRC7_PATCH", 0); // VRC7 patch set
  CreateValue("NSFE_PLAYLIST", 1); // use NSFe playlist

  CreateValue("COMP_LIMIT",    100);
  CreateValue("COMP_THRESHOLD",100);
  CreateValue("COMP_VELOCITY", 100);

  CreateValue("NTSC_BASECYCLES", 1789773);
  CreateValue("PAL_BASECYCLES", 1662607);
  CreateValue("DENDY_BASECYCLES", 1773448);
  CreateValue("REGION", 0);
  CreateValue("LOG_CPU", 0);
  CreateValue("LOG_CPU_FILE", "nsf_write.log");

  CreateValue("MASTER_VOLUME", 128);

  for (i = 0; i < NES_CHANNEL_MAX; ++i)
  {
      std::string str;
      char num[5];
      ::itoa(i, num, 10);
      str = "CHANNEL_";
      if (i < 10) str += "0";
      str += num;
      str += "_PAN";
      CreateValue(str, 128);
      str = "CHANNEL_";
      if (i < 10) str += "0";
      str += num;
      str += "_VOL";
      CreateValue(str, 128);
      str = "CHANNEL_";
      if (i < 10) str += "0";
      str += num;
      str += "_COL";
      CreateValue(str, DEFAULT_CHANNEL_COL[i]);
  }
  CreateValue("5B_ENVELOPE_COL", DEFAULT_5B_ENVELOPE_COL);
  CreateValue("5B_NOISE_COL", DEFAULT_5B_NOISE_COL);

  for (i = 0; i < NES_DEVICE_MAX; i++)
  {
    static const int DEVICE_VOLUME[NES_DEVICE_MAX] = {
        128, 128, 112, 128, 128,  90,  96, 128 };

    std::string str;
    str = dname[i];
    CreateValue((std::string)dname[i]+"_VOLUME", DEVICE_VOLUME[i]);
    CreateValue((std::string)dname[i]+"_QUALITY", 3);
    CreateValue((std::string)dname[i]+"_FILTER", 0);
    CreateValue((std::string)dname[i]+"_MUTE", 0);
    CreateValue((std::string)dname[i]+"_THRESHOLD", 100);

    static const int DEVICE_OPTION_MAX[NES_DEVICE_MAX] =
    {
        NES_APU::OPT_END,
        NES_DMC::OPT_END,
        0, // 5B
        NES_MMC5::OPT_END,
        0, // N163
        0, // VRC6
        0, // VRC7
        0, // FDS
    };

    static const int DEFAULT_DEVICE_OPTION[NES_DEVICE_MAX][16] =
    {
        { 1, 1, 1, 1, 0 },
        { 1, 1, 1, 0, 1, 1 },
        {},
        { 1, 1 },
        {},
        {},
        {},
        { 1, 0 }
    };

    for (j = 0; j < DEVICE_OPTION_MAX[i]; j++)
    {
      static char itoa[] = "0123456789ABCDEF";
      CreateValue((std::string)dname[i]+"_OPTION"+itoa[j], DEFAULT_DEVICE_OPTION[i][j]);
    }
  }
}

NSFPlayerConfig::~NSFPlayerConfig ()
{
}

// Load one
bool NSFPlayerConfig::Load (const char *path, const char *sect, const char *name)
{
  char temp[256];
  GetPrivateProfileString(sect,name,data[name].GetStr().c_str(),temp,255,path);
  data[name] = vcm::Value(temp);
  return true;
}

// Load all
bool NSFPlayerConfig::Load (const char *path, const char *sect)
{
  std::map<std::string, vcm::Value>::iterator it;
  for(it=data.begin(); it!=data.end(); it++)
    Load(path, sect, it->first.c_str());
  return true;
}

bool NSFPlayerConfig::Save (const char *path, const char *sect, const char *name)
{
  WritePrivateProfileString (sect, name, data[name], path);
  return true;
}

bool NSFPlayerConfig::Save (const char *path, const char *sect)
{
  std::map<std::string, vcm::Value>::iterator it;
  for(it=data.begin(); it!=data.end(); it++)
    Save(path, sect, it->first.c_str());

 return true;
}