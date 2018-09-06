#ifndef _COMPRESS_5B_H_
#define _COMPRESS_5B_H_
#include "../device.h"

namespace xgm
{
    class Compress5B : public IRenderable
    {
    private:
        double rate;
    public:
        Compress5B();
        ~Compress5B();
        void SetRate(double r);
        void Reset();
        UINT32 FastRender (INT32 b[2]);
        virtual UINT32 Render (INT32 b[2]);
        virtual void Tick (UINT32 clocks);
    };
} // namespace
#endif
