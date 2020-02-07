#ifndef _NSF_TAG_H_
#define _NSF_TAG_H_
#include "../xgm/player/nsf/nsf.h"
#include "../xgm/player/player.h"

class NSF_TAG
{
public:
  xgm::NSF *sdat;
  char m_tagf[_MAX_PATH]; //�^�O�t�@�C����
  char m_tagl[_MAX_PATH]; //���[�J���^�O�t�@�C����
  char m_sect[_MAX_PATH]; //�Z�N�V������
  char drv[_MAX_DRIVE];
  char dir[_MAX_DIR];
  char fname[_MAX_FNAME];
  char ext[_MAX_EXT];

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
  bool Export(); // taginfo.tag -> ���[�J���^�O���
  bool Import(); // ���[�J���^�O��� -> taginfo.tag
};

#endif
