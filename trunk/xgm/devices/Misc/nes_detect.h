#ifndef _NES_DETECT_H_
#define _NES_DETECT_H_
#include "detect.h"

namespace xgm
{

class NESDetector : public BasicDetector
{
public:
  virtual bool Write (UINT32, UINT32, UINT32 id=0);
};

class NESDetectorEx : public ILoopDetector
{
protected:
  enum 
  {
    SQR_0=0,SQR_1,TRI,NOIZ,DPCM,
    N106_0,N106_1,N106_2,N106_3,
    N106_4,N106_5,N106_6,N106_7,
    MAX_CH
  };
  BasicDetector *m_LD[MAX_CH];
  bool m_looped[MAX_CH];
  UINT32 m_n106_addr;
  int m_loop_start, m_loop_end;
public:
  NESDetectorEx();
  virtual ~NESDetectorEx();
  virtual bool IsLooped (int time_in_ms, int match_second, int match_interval);
  virtual void Reset ();
  virtual bool Write (UINT32, UINT32, UINT32 id=0);
  virtual bool Read (UINT32, UINT32 &, UINT32 id=0){ return false; }
  virtual int GetLoopStart(){ return m_loop_start; }
  virtual int GetLoopEnd(){ return m_loop_end; }
  virtual bool IsEmpty()
  {
    bool ret = true;
    for(int i=0;i<MAX_CH;i++)
      ret &= m_LD[i]->IsEmpty();
    return ret;
  }
};

} // namespace

#endif
