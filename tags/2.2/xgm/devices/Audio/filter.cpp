#include "filter.h"
using namespace xgm;

static const double pi = (3.14159265358979);

static double HAMMING_window(int n, int M)
{
  return 0.54 + 0.46 * cos(pi*n/M);
}

static double HANNING_window(int n, int M)
{
  return 0.5 * (1.0 + cos(pi*n/M));
}

static double BERTLET_window(int n, int M)
{
  return 1.0 - (double)n/M;
}

static double SQR_window(int n, int M)
{
  return 1.0;
}

SimpleFIR::SimpleFIR(int tap_num) {
  N = tap_num|1;
  tap = new INT32[N];
  M = (N-1)/2 ;
  h = new double[M+1];
}
SimpleFIR::~SimpleFIR()
{
	delete [] tap;
	delete [] h;
}

void SimpleFIR::SetRate(double r)
{
	rate = r;
}

void SimpleFIR::SetCutoff(double c)
{
	cutoff = c;
}

void SimpleFIR::Reset()
{
  int i;

  for(i=0;i<N;i++) tap[i] = 0;

  double Wc = 2.0 * pi * (cutoff / rate);
  double gain = 0.0;

  h[0] = Wc / pi * HAMMING_window(0,M);

  for(i=1; i<=M; i++) {
    h[i] = (1.0/(pi*i)) * sin(Wc*i) * HAMMING_window(i,M);
    gain += h[i];
  }

  // o—ÍƒQƒCƒ“’²®
  for(i=0; i<=M; i++) 
    h[i] /= gain;

}

void SimpleFIR::Put(xgm::INT32 wav) 
{
  for(int i=0;i<N-1;i++) tap[i]=tap[i+1];
  tap[N-1] = wav;
}

xgm::INT32 SimpleFIR::Get() 
{
  double temp = h[0]*tap[M];
  for(int i=1;i<=M;i++)
    temp += h[i]*(tap[M+i]+tap[M-i]);
  return (INT32)temp;
}
