#include "echo.h"

using namespace xgm;

void EchoUnit::Reset()
{
  int hdef[16] = { 
      0,  0,  0,  0,
     64, 32, 16,  8,
     32, 16,  8,  4,
     16,  8,  4,  2,
  };
  eidx = 0;
  memset(echo_buf,0,sizeof(INT32)*(1<<17));
  for(int i=0; i<16; i++) h[i] = hdef[i];
  lpf.SetParam(4700,100);
  lpf.Reset();
  hpf.SetParam(270,100);
  hpf.Reset();
}

void EchoUnit::SetRate(double rate)
{
  edelay = ((int)rate)/16;
  lpf.SetRate(rate);
  hpf.SetRate(rate);
}

inline xgm::UINT32 EchoUnit::FastRender(xgm::INT32 b[2])
{
  INT32 buf[2];

  int tmp = eidx;
  for(int i=0; i<16; i++) 
  {
    echo_buf[tmp&((1<<17)-1)] += (b[0]*h[i])>>8;
    tmp += edelay;
  }
  buf[0] = buf[1] = echo_buf[eidx];
  lpf.FastRender(buf);
  hpf.FastRender(buf);
  echo_buf[eidx] = 0;
  eidx = (eidx + 1)&((1<<17)-1);

  b[0] += buf[0];
  b[1] += buf[1];

  return 2;
}

void xgm::EchoUnit::Tick(UINT32 clocks)
{
  lpf.Tick(clocks);
  //hpf.Tick(clocks); // hpf has no sub-renderable
}

xgm::UINT32 EchoUnit::Render(xgm::INT32 b[2])
{
  return FastRender(b);
}