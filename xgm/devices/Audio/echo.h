#ifndef _ECHO_H_
#define _ECHO_H_
#include "filter.h"

namespace xgm {

  class EchoUnit : public IRenderable {
  protected:
    int rate;
    INT32 *echo_buf;
    int h[32];
    int eidx, edelay;
    Filter lpf;
    DCFilter hpf;

  public:
    EchoUnit(){ echo_buf = new INT32[1<<17]; }
    ~EchoUnit(){ delete [] echo_buf; }
    void Reset();
    void SetRate(double r);
    virtual void Tick(int clocks);
    virtual UINT32 Render(INT32 b[2]);
    inline UINT32 FastRender(INT32 b[2]);
  };

} // xgm

#endif
