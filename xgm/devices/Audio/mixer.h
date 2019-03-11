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
    DeviceList dlist;

  public:
      Mixer ()
    {
      Reset();
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
    }

    virtual void Skip ()
    {
      // Should pass Skip on to all attached devices, but nothing currently requires it.

      //DeviceList::iterator it;
      //for (it = dlist.begin (); it != dlist.end (); it++)
      //{
      //  (*it)->Skip ();
      //}
    }

    virtual void Tick (UINT32 clocks)
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

      return 2;
    }
  };

}                               // namespace

#endif
