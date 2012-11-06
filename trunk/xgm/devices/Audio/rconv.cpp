#include "rconv.h"

namespace xgm{

#define GETA 16
#define PI (3.14159265358979)

static double hamming_window(int n, int M)
{
  return 0.54 + 0.46 * cos(PI*n/M);
}

RateConverter::RateConverter () : clock(0.0), rate(0.0), mult(0), clocks(0)
{
}

RateConverter::~RateConverter ()
{
}

void RateConverter::Attach (IRenderable * t)
{ 
  target = t;
}


void RateConverter::Reset ()
{
  clocks = 0; // cancel any pending ticks

  if(clock&&rate)
  {
    mult = (int)(clock/rate);
    if(mult<2) return ;

    double wc = 2.0 * PI * (rate/2.0) / clock ;
    int m = (mult*2+1)/2;    

    hr[0] = wc / PI * hamming_window(0,m);
    double gain = hr[0];
    for(int i=1; i<=m; i++)
    {
      hr[i]= (1.0/(PI*i)) * sin(wc*i) * hamming_window(i,m);
      gain += hr[i] * 2;
    }

    for(int i=0; i<=m; i++)
      hr[i] /= gain;

	  for(int i=0; i<=mult*2; i++) 
		  tap[i][0] = tap[i][1] = 0;
  }

}

// ”{—¦‚ÍŠï””{‚Ì‚Ý
void RateConverter::SetClock (double c)
{
  clock = c;
}

void RateConverter::SetRate (double r)
{
  rate = r;
}

void RateConverter::Tick (UINT32 clocks_)
{
  assert (target);
  clocks += clocks_;
}

UINT32 RateConverter::Render (INT32 b[2])
{
  return FastRender(b);
}

// “ü—Í‚Í-32768`+32767‚Ü‚Å
inline UINT32 RateConverter::FastRender (INT32 b[2])
{
  assert (target);

  double out[2];

  for(int i=0; i<=mult; i++)
  {
    tap[i][0] = tap[i+mult][0];
    tap[i][1] = tap[i+mult][1];
  }

  int mclocks = 0;
  for(int i=1; i<=mult; i++)
  {
    mclocks += clocks;
    if (mclocks >= mult)
    {
      int sub_clocks = mclocks / mult;
      target->Tick(sub_clocks);
      mclocks -= (sub_clocks * mult);
    }
    target->Render(tap[mult+i]);
  }
  assert (mclocks == 0);
  clocks = 0;

  out[0] = hr[0] * tap[mult][0];
  out[1] = hr[0] * tap[mult][1];

  for(int i=1; i<=mult; i++)
  {
    out[0] += hr[i] * (tap[mult+i][0]+tap[mult-i][0]);
    out[1] += hr[i] * (tap[mult+i][1]+tap[mult-i][1]);
  }

  b[0] = (INT32)out[0];
  b[1] = (INT32)out[1];

  return 2;
}

}//namespace xgm