#ifndef _Z80_CPU_H_
#define _Z80_CPU_H_
#include "../device.h"

#define External __inline
#include "kmz80/kmz80.h"

namespace xgm
{
class Z80_CPU : public IDevice
{
public:
  enum { DEVICE_MEM, DEVICE_IO, DEVICE_MAX };
  enum { INT_NMI, INT_VSYNC, INT_TYPE_MAX };

protected:
  KMZ80_CONTEXT context ;
  KMEVENT kme ;
  KMEVENT_ITEM_ID int_id[INT_TYPE_MAX];
  UINT32 int_adr[KMEVENT_ITEM_MAX+1];

  IDevice *device[DEVICE_MAX];

public:
    Z80_CPU ();
   ~Z80_CPU ();
  void Reset ();
  void Exec (UINT32 cycles);
  void AddIntHandler(int type, UINT32 cycles, UINT32 address);
  void DelIntHandler(int type);
  void Halt(bool b);
  bool Read (UINT32 adr, UINT32 & val, UINT32 id=0);
  bool Write (UINT32 adr, UINT32 val, UINT32 id=0);
  void SetSP(UINT32 sp);
  UINT32 GetSP();
  void SetPC(UINT32 pc);
  UINT32 GetPC();
  void SetRegs8(int type, UINT32 d);
  UINT32 GetRegs8(int type);
  void SetDevice (IDevice *, int id);
  void GetMemory(UINT8 *buf, UINT32 size, UINT32 adr);
  void SetMemory(UINT8 *buf, UINT32 size, UINT32 adr);

  // KMZ80とのインタフェースなのでpublicだが外部からは呼ばないこと。
  // Interruptを掛けるときは SetRegs8(REGID_INTREQ, 1);を実行。
  void Interrupt(KMEVENT_ITEM_ID curid);
};

} // namespace xgm
#endif
