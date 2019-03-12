#include "rconv.h"
#include "../CPU/nes_cpu.h"
#include "../Sound/nes_dmc.h"
#include "../Sound/nes_mmc5.h"

namespace xgm{

#define GETA 16
#define PI (3.14159265358979)
#define PRECISION 16

static double window(int n, int M)
{
  // rectangular window
  //return 1.0;

  // hanning window
  //return 0.5 + 0.5 * cos(PI*double(n)/double(M));

  // hamming window
  return 0.54 + 0.46 * cos(PI*double(n)/double(M));
}

RateConverter::RateConverter () : clock(0.0), rate(0.0), mult(0), clocks(0),
	cpu(NULL), dmc(NULL), mmc5(NULL), cpu_clocks(0), cpu_rest(0),
	fast_skip(true)
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
  cpu_clocks = 0;
  cpu_rest = 0;

  if(clock&&rate)
  {
    mult = (int)(clock/rate);
    if(mult<2) mult=1;
    if(mult>63) mult=63;

    int m = (mult*2+1)/2;

    // generate resampling window
    hr[0] = window(0,m);
    double gain = hr[0];
    for(int i=1; i<=m; i++)
    {
      hr[i]= window(i,m);
      gain += hr[i] * 2;
    }

    // normalize window
    for(int i=0; i<=m; i++)
    {
      hr[i] /= gain;
      hri[i] = INT64(hr[i] * (1<<PRECISION));
    }

    for(int i=0; i<=mult*2; i++) 
      tap[0][i] = tap[1][i] = 0;
  }

}

// î{ó¶ÇÕäÔêîî{ÇÃÇ›
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

void RateConverter::ClockCPU(int c)
{
	int real_cpu_clocks = 0;
	if (cpu)
	{
		cpu_rest += c;
		if (cpu_rest > 0)
		{
			real_cpu_clocks = cpu->Exec(cpu_rest);
			cpu_rest -= real_cpu_clocks;
		}
	}
	if (dmc) dmc->TickFrameSequence(real_cpu_clocks);
	if (mmc5) mmc5->TickFrameSequence(real_cpu_clocks);
}

void RateConverter::Skip ()
{
	if (fast_skip) // behaves like quality=1, fine except for rare cases that need sub-sample synchronization
	{
		ClockCPU(cpu_clocks);
		target->Tick(clocks);
		cpu_clocks = 0;
		clocks = 0;
	}
	else // divide clock ticks among samples evenly
	{
		int mclocks = 0;
		int mcclocks = 0;
		for(int i=1; i<=mult; i++)
		{
			// CPU first
			mcclocks += cpu_clocks;
			if (mcclocks >= mult)
			{
				int sub_clocks = mcclocks / mult;
				ClockCPU(sub_clocks);
				mcclocks -= (sub_clocks * mult);
			}

			// Audio devices second
			mclocks += clocks;
			if (mclocks >= mult)
			{
				int sub_clocks = mclocks / mult;
				target->Tick(sub_clocks);
				mclocks -= (sub_clocks * mult);
			}

			// Render is skipped
			//target->Skip(); // Should do this, but nothing currently requires it (see also: mixer.h)
		}
		assert (mclocks == 0); // all clocks must be used
		assert (mcclocks == 0);
		clocks = 0;
		cpu_clocks = 0;
	}
}

// ì¸óÕÇÕ-32768Å`+32767Ç‹Ç≈
inline UINT32 RateConverter::FastRender (INT32 b[2])
{
  assert (target);

  //double out[2];
  INT64 out[2];
  static INT32 t[2];

  for(int i=0; i<=mult; i++)
  {
    tap[0][i] = tap[0][i+mult];
    tap[1][i] = tap[1][i+mult];
  }

  // divide clock ticks among samples evenly
  int mclocks = 0;
  int mcclocks = 0;
  for(int i=1; i<=mult; i++)
  {
    // CPU first
    mcclocks += cpu_clocks;
    if (mcclocks >= mult)
    {
      int sub_clocks = mcclocks / mult;
      ClockCPU(sub_clocks);
      mcclocks -= (sub_clocks * mult);
    }

    // Audio devices second
    mclocks += clocks;
    if (mclocks >= mult)
    {
      int sub_clocks = mclocks / mult;
      target->Tick(sub_clocks);
      mclocks -= (sub_clocks * mult);
    }

    target->Render(t);
    tap[0][mult+i] = t[0];
    tap[1][mult+i] = t[1];
  }
  assert (mclocks == 0); // all clocks must be used
  assert (mcclocks == 0);
  clocks = 0;
  cpu_clocks = 0;

  //out[0] = hr[0] * tap[0][mult];
  //out[1] = hr[0] * tap[1][mult];
  out[0] = hri[0] * tap[0][mult];
  out[1] = hri[0] * tap[1][mult];

  for(int i=1; i<=mult; i++)
  {
    //out[0] += hr[i] * (tap[0][mult+i]+tap[0][mult-i]);
    //out[1] += hr[i] * (tap[1][mult+i]+tap[1][mult-i]);
    out[0] += hri[i] * (tap[0][mult+i]+tap[0][mult-i]);
    out[1] += hri[i] * (tap[1][mult+i]+tap[1][mult-i]);
  }

  //b[0] = (INT32)out[0];
  //b[1] = (INT32)out[1];
  b[0] = INT32(out[0] >> PRECISION);
  b[1] = INT32(out[1] >> PRECISION);

  return 2;
}

}//namespace xgm