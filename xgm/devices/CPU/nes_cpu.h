#ifndef _NES_CPU_H_
#define _NES_CPU_H_
#include "../device.h"

#define ILLEGAL_OPCODES 1
#define DISABLE_DECIMAL 1
#define USE_DIRECT_ZEROPAGE 0
#define USE_CALLBACK	1
#define USE_INLINEMMC 0
#define USE_USERPOINTER	1
#define External __inline
#include "km6502/km6502m.h"

#include "../Misc/log_cpu.h"

namespace xgm
{
class NES_CPU : public IDevice
{
protected:
  int int_address;
  K6502_Context context;
  bool breaked;
  UINT32 clock_per_frame;
  UINT32 clock_of_frame;
  UINT32 frame_quarter;
  UINT32 breakpoint;
  IDevice *bus;
  CPULogger *log_cpu;
  void startup (UINT32 address);

public:
  double NES_BASECYCLES;
  NES_CPU (double clock = DEFAULT_CLOCK);
  ~NES_CPU ();
  void Reset ();
  void Start (int s, int i, double f=60, int a=0, int x=0, int y=0);
  UINT32 Exec (UINT32 clock); // returns number of clocks executed
  void SetMemory (IDevice *);
  bool Read (UINT32 adr, UINT32 & val, UINT32 id=0);
  bool Write (UINT32 adr, UINT32 val, UINT32 id=0);
  void SetLogger (CPULogger *logger);
  unsigned int GetPC() const;
};

} // namespace xgm
#endif
