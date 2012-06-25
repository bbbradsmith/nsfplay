#include <stdlib.h>
#include "MedianFilter.h"

using namespace xgm;

static int INT32CMP(const INT32 *a, const INT32 *b) {
  if (*a < *b)
    return -1;
  else if (*a> *b)
    return 1;
  return 0;
}

MedianFilter::MedianFilter(int tapSize) {
  tapSize_ = tapSize;
  tap_ = new INT32[tapSize_];
}

MedianFilter::~MedianFilter() {
  delete [] tap_;
}

void MedianFilter::Reset() {
  for(int i=0; i<tapSize_; i++)
    tap_[i] = 0;
  dirty_ = false;
  tapIndex_ = 0;
}

void MedianFilter::Put(INT32 data) {
  tap_[tapIndex_++%tapSize_] = data;
  dirty_ = true;
}

INT32 MedianFilter::Get() {
  if(dirty_) {
    qsort(tap_, tapSize_, sizeof(INT32), (int(*)(const void*, const void*))INT32CMP);
    dirty_ = false;
  }
  return tap_[tapSize_>>1];
}

