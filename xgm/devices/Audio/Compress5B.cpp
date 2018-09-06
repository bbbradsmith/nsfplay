#include "Compress5B.h"
#include <cmath>

namespace xgm
{

Compress5B::Compress5B()
{
    Reset();
}

Compress5B::~Compress5B()
{
}

void Compress5B::SetRate(double r)
{
    rate = r;
}

void Compress5B::Reset()
{
}

UINT32 Compress5B::FastRender(INT32 b[2])
{
    // 1058 = volume 12 ? (doesn't seem to be)
    // actually more like 4500?
    const double vol12 = 4500.0;
    const double threshold = 2.24 * vol12; // 7dB above 12
    const double ratio = 2.7;
    for (int i=0; i<2; ++i)
    {
        double x = double(b[i]);
        double c = fabs(x) - double(threshold);
        if (c < 0.0) c = 0.0;
        double g = pow(1+(c/threshold),(1/ratio)-1);
        b[i] = int(g*x);
        //b[1] = c;
    }

    return 2;
}

UINT32 Compress5B::Render (INT32 b[2])
{
    return FastRender(b);
}

void Compress5B::Tick (UINT32 clocks)
{
}

}
