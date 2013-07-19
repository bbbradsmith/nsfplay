#ifndef _DETECT_H_
#define _DETECT_H_
#include "../device.h"

namespace xgm
{
  class ILoopDetector : public IDevice
  {
  public:
    virtual ~ILoopDetector(){}
    virtual void Reset ()=0;
    virtual bool Write (UINT32, UINT32, UINT32 id=0)=0;
    virtual bool IsLooped (int time_in_ms, int match_second, int match_interval)=0;
    virtual bool Read (UINT32, UINT32 &, UINT32 id=0)=0;
    virtual int GetLoopStart()=0;
    virtual int GetLoopEnd()=0;
    virtual bool IsEmpty ()=0;
  };

  class BasicDetector : public ILoopDetector
  {
  protected:
    int m_bufsize, m_bufmask;
    int *m_stream_buf;
    int *m_time_buf;
    int m_bidx;
    int m_blast;                    // 前回チェック時のbidx;
    int m_wspeed;
    int m_current_time;
    int m_loop_start, m_loop_end;
    bool m_empty;

  public:
    BasicDetector(int bufbits = 16);
    virtual ~BasicDetector();
    virtual void Reset ();
    virtual bool Write (UINT32, UINT32, UINT32 id=0);
    virtual bool Read (UINT32, UINT32 &, UINT32 id=0){ return false; }
    virtual bool IsLooped (int time_in_ms, int match_secound, int match_interval);
    virtual int GetLoopStart(){ return m_loop_start; }
    virtual int GetLoopEnd(){ return m_loop_end; }
    virtual bool IsEmpty(){ return m_empty; }
  };

} // namespace

#endif