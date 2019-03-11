#ifndef _FADER_H_
#define _FADER_H_
#include "../device.h"
#include <vector>

namespace xgm
{
  class Fader : virtual public IRenderable
  {
  protected:
    UINT32 fade_pos, fade_end;
    IRenderable* d;

  public:
    Fader ()
    {
      d = NULL;
      Reset();
    }

     ~Fader ()
    {
    }

    void Attach (IRenderable * dev)
    {
      d = dev;
    }

    void Reset ()
    {
      fade_pos = 0;
      fade_end = 1;
    }

    bool IsFadeEnd ()
    {
      return (fade_pos >= fade_end);
    }

    bool IsFading ()
    {
      return (fade_pos > 0);
    }

    void FadeStart (double rate, int fade_in_ms)
    {
      if (fade_in_ms)
      {
        const UINT32 MAX_SAMPLES = ~0UL;
        double samples = (double)fade_in_ms * rate / 1000.0;
        if (samples < (double)MAX_SAMPLES)
        {
          fade_end = UINT32(samples);
        }
        else
        {
          fade_end = MAX_SAMPLES;
        }
      }
      else
      {
        fade_end = 1;
      }
      fade_pos = 1; // begin fade
    }

    virtual void Skip ()
    {
      if (fade_pos > 0)
      {
        if (fade_pos < fade_end) ++fade_pos;
        else fade_pos = fade_end;
      }
      d->Skip();
    }

    virtual void Tick (UINT32 clocks)
    {
      d->Tick(clocks);
    }

    virtual UINT32 Render (INT32 b[2])
    {
      d->Render (b);
      if (fade_pos > 0)
      {
        double fade_amount = double(fade_end - fade_pos) / double(fade_end);
        b[0] = INT32(fade_amount * b[0]);
        b[1] = INT32(fade_amount * b[1]);

        if (fade_pos < fade_end) ++fade_pos;
        else fade_pos = fade_end;
      }
      return 2;
    }
  };

}                               // namespace

#endif
