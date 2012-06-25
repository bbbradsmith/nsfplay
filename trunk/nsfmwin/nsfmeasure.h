#ifndef _NSFMEASURE_H_
#include "xgm.h"

class NSFMeas
{
protected:
  xgm::NSF *nsf;
  xgm::NSFPlayer *nsfp;
  xgm::NSFPlayerConfig *nsfc;
  int progress_time;
public:
  NSFMeas();
  virtual ~NSFMeas();
  virtual bool Open(xgm::NSF *nsf, int max_time=300000, int min_loop = 30000, int mode = 0);
  virtual bool Progress();
  virtual bool IsDetected();
  virtual int GetProgressTime();
};

#endif