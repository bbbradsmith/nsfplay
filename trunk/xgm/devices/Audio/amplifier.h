#ifndef _AMPLIFIER_H_
#define _AMPLIFIER_H_
#include <math.h>

namespace xgm
{

  class Amplifier : virtual public IRenderable
  {
  protected:
    IRenderable * target;
    int mute, volume, threshold, weight, pan;
    double th;

    INT32 compress (INT32 d)
    {
      if (weight < 0)
        return d;
/*
      if (weight == 0)
      {
        double val = 20 * log10 (d > 0 ? d : -d);

        if (val <= th)
            return d;

          val = th + (val - th) / 16;

        if (d > 0)
            return (INT32) pow (10, val / 20);
        else
            return -(INT32) pow (10, val / 20);
      }

      if (d > threshold)
          return threshold + (d - threshold) / weight;
      else if (d < -threshold)
          return -threshold + (d + threshold) / weight;
      else
          return d;
*/
      if ( d > threshold)
        return threshold;
      else if (d < -threshold)
        return -threshold;
      else
        return d;
    }

  public:
    Amplifier ()
    {
      target = NULL;
      mute = false;
      volume = 64;
      threshold = 32767;
      th = 20 * log10 ((double)threshold);
      weight = -1;
      pan = 0;
    }

    ~Amplifier ()
    {
    }

    void Attach (IRenderable * p)
    {
      target = p;
    }

    void Tick (int clocks)
    {
      assert (target);
      target->Tick(clocks);
    }

    UINT32 Render (INT32 b[2])
    {
      assert (target);
      if (mute)
      {
        b[0] = b[1] = 0;
        return 2;
      }
      target->Render (b);
      b[0] = compress ((b[0] * volume) / 16);
      b[1] = compress ((b[1] * volume) / 16);

      return 2;
    }

    void SetVolume (int v)
    {
      volume = v;
      SetCompress (threshold, weight);
    }
    int GetVolume ()
    {
      return volume;
    }
    void SetMute (int m)
    {
      mute = m;
    }
    int GetMute ()
    {
      return mute;
    }
    void SetCompress (int t, int w)
    {
      threshold = 32768 * t / 100;
      if (threshold < 32768)
        weight = 0;
      else
        weight = -1;
      th = 20 * log10 ((double)threshold);
    }
    void SetPan (int p)
    {
      pan = p;
    }
    int GetPan ()
    {
      return pan;
    }
  };

}                               // namespace

#endif
