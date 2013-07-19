#include "nsfmeasure.h"

using namespace xgm;

#define FREQ 60

NSFMeas::NSFMeas()
{
  nsfp = new NSFPlayer();
  nsfc = new NSFPlayerConfig();
  nsfc->detecttime = 60*1000;
  nsfc->detectint = 5000;
  nsfc->loopnum = 1;
  nsfc->autodetect = 1;
  nsfc->autostop = 1;
  nsfc->stopsec = 3;
  nsfc->enable_dcf = 1;
  for(int i=0; i<NES_DEVICE_MAX; i++)
    nsfc->dprop[i].filter_C = 0;
  nsfc->dprop[DMC].option[NES_DMC::OPT_DPCM_HPF]=0;
  nsfc->dprop[DMC].option[NES_DMC::OPT_DPCM_LPF]=0;
  nsfc->dprop[DMC].option[NES_DMC::OPT_DPCM_FEEDBACK]=0;
  nsfp->SetConfig(nsfc);
}

NSFMeas::~NSFMeas()
{
  delete nsfp;
  delete nsfc;
}

bool NSFMeas::Open(NSF *n, int max_time, int min_loop, int detect_mode)
{
  nsf = n;
  progress_time = 0;
  nsf->playtime_unknown = true;
  nsf->time_in_ms = max_time;
  nsf->loop_in_ms = 0;
  nsfc->detecttime = min_loop;
  nsfc->detect_alternative = detect_mode;

  if(!nsfp->Load(nsf)) return false;
  nsfp->SetPlayFreq(FREQ);
  nsfp->Reset();
  return true;
}

bool NSFMeas::Progress()
{
  xgm::INT16 dummy[FREQ];

  nsfp->Render(dummy,FREQ);
  progress_time += 1000;
  return (!nsfp->IsDetected()&&!nsfp->IsStopped());
}

bool NSFMeas::IsDetected(){ return nsfp->IsDetected(); }

int NSFMeas::GetProgressTime(){ return progress_time; }