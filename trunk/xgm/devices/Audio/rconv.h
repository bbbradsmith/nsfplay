#ifndef _RCONV_H_
#define _RCONV_H_
#include "../device.h"
#include "filter.h"

namespace xgm
{
  /**
   * サンプリングレートコンバータ
   */
  class RateConverter : public IRenderable
  {
  protected:
    IRenderable * target;
    double clock ,rate;
    int mult;          // オーバーサンプリング倍率(奇数)
    INT32  tap[128][2];
    double hr[128];    // H(z)

    SimpleFIR *fir;

  public:
    RateConverter ();
    virtual ~ RateConverter ();
    void Attach (IRenderable * t);
    void Reset ();
    void SetClock (double clock);
    void SetRate (double rate);
    UINT32 Render (INT32 b[2]);
    inline UINT32 FastRender(INT32 b[2]);
  };

} // namespace
#endif
