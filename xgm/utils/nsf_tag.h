#ifndef _NSF_TAG_H_
#define _NSF_TAG_H_

#if defined (WIN32)

#include <string>

#include "../player/nsf/nsf.h"

class NSF_TAG
{
public:
  xgm::NSF *sdat;
  std::string m_tagf;
  std::string m_tagl;
  std::string m_sect;
  std::string drv;
  std::string dir;
  std::string fname;
  std::string ext;

  NSF_TAG();
  NSF_TAG(xgm::NSF *nsf);
  ~NSF_TAG();
  void SetNSF(xgm::NSF *);
  void Sync();
  int WriteTagItem(int song, const char *title, int time, int loop, int fade);
  int InitTagItem(int song, const char *title_format);
  int CreateTag(const char *title_format);
  bool IsExistSection(bool local=false);
  int ClearTag();
  int WriteEnable(bool b);
  int ReadTagItem(int song);
  void UpdateTagItem(int song, const char *title_format, int loopnum);
  bool IsWriteEnable();
  bool Export();
  bool Import();
};

#endif

#endif
