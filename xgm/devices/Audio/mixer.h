#ifndef _MIXER_H_
#define _MIXER_H_
#include "../device.h"
#include <vector>

namespace xgm
{
  class Mixer : virtual public IRenderable
  {
  protected:
    typedef std::vector < IRenderable * >DeviceList;
    const UINT32 FADE_AMOUNT_MAX;
    UINT32 fade_speed, fade_amount;
    DeviceList dlist;

  public:
      Mixer ():FADE_AMOUNT_MAX (1 << 31)
    {
    }

     ~Mixer ()
    {
    }

    void DetachAll ()
    {
      dlist.clear ();
    }

    void Attach (IRenderable * dev)
    {
      dlist.push_back (dev);
    }

    void Reset ()
    {
      fade_speed = 0;
      fade_amount = 0;
    }

    bool IsFadeEnd ()
    {
      return (fade_speed && !fade_amount);
    }

    bool IsFading ()
    {
      return (fade_speed != 0);
    }

    void FadeStart (double rate, int fade_in_ms)
    {
      if (fade_in_ms)
        fade_speed = (UINT32)(FADE_AMOUNT_MAX / (fade_in_ms * rate / 1000));
      else
        fade_speed = FADE_AMOUNT_MAX;

      if (fade_speed == 0)
        fade_speed = 1;

      fade_amount = FADE_AMOUNT_MAX;
    }

    void Skip (int length)
    {
      if (fade_speed)
        fade_amount =
          (fade_amount <
           fade_speed * length) ? 0 : fade_amount - (fade_speed * length);
    }

    virtual void Tick (int clocks)
    {
      DeviceList::iterator it;
      for (it = dlist.begin (); it != dlist.end (); it++)
      {
        (*it)->Tick (clocks);
      }
    }

    virtual UINT32 Render (INT32 b[2])
    {
      INT32 tmp[2];
      DeviceList::iterator it;

      b[0] = b[1] = 0;

      for (it = dlist.begin (); it != dlist.end (); it++)
      {
        (*it)->Render (tmp);
        b[0] += tmp[0];
        b[1] += tmp[1];
      }

      if (fade_speed)
      {
        fade_amount = fade_amount < fade_speed ? 0 : fade_amount - fade_speed;
        b[0] = (INT32) (b[0] * (fade_amount >> 23)) >> 8;
        b[1] = (INT32) (b[1] * (fade_amount >> 23)) >> 8;
      }
      return 2;
    }
  };

}                               // namespace

#endif
