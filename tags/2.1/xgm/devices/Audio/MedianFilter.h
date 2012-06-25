#ifndef _MEDIAN_FILTER_H_
#define _MEDIAN_FILTER_H_
#include "../../xtypes.h"

namespace xgm {
  // メディアンフィルタ
  class MedianFilter {
  private:
    int tapSize_;
    int *tap_;
    int tapIndex_;
    bool dirty_;
  public:
    MedianFilter(int tapSize);
    void Reset();
    virtual ~MedianFilter();
    void Put(INT32 wav);
    INT32 Get();
  };

} // namespace
#endif