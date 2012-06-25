#include "z80.h"

namespace xgm {
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Callback functions for KMZ80 
//
static
UINT32 memread(void *user, UINT32 a)
{
  Z80_CPU *z80 = static_cast<Z80_CPU *>(user);
  UINT32 val;

  z80->Read(a, val, Z80_CPU::DEVICE_MEM);
  return val;
}

static
void memwrite(void *user, UINT32 a, UINT32 d)
{
  Z80_CPU *z80 = static_cast<Z80_CPU *>(user);
  z80->Write(a, d, Z80_CPU::DEVICE_MEM);
}

static
UINT32 ioread(void *user, UINT32 a)
{
  Z80_CPU *z80 = static_cast<Z80_CPU *>(user);
  UINT32 val;
  
  z80->Read(a, val, Z80_CPU::DEVICE_IO);
  return val;
}

static
void iowrite(void *user, UINT32 a, UINT32 d)
{
  Z80_CPU *z80 = static_cast<Z80_CPU *>(user);
  z80->Read(a, d, Z80_CPU::DEVICE_IO);
}

static
UINT32 busread(void *user, UINT32 mode)
{
  return 0x38 ;
}

/////////////////////////////////////////////////////////////////////////////
//
// Callback function for KMEVENT
//
static void
kmevent_handler(KMEVENT *event, KMEVENT_ITEM_ID curid, void *user)
{
  Z80_CPU *z80 = static_cast<Z80_CPU *>(user);
  z80->Interrupt(curid);
}


/////////////////////////////////////////////////////////////////////////////
//
// CREATE&DESTROY Z80_CPU
//
Z80_CPU::Z80_CPU()
{
  int i;
  kmevent_init(&kme);
  context.kmevent = &kme;
  for(i=0;i<INT_TYPE_MAX;i++)
    int_id[i] = KMEVENT_ITEM_MAX+1;
  for(i=0;i<DEVICE_MAX;i++)
    device[i] = NULL;
}

Z80_CPU::~Z80_CPU()
{
}

/////////////////////////////////////////////////////////////////////////////
//
// Main Features
//
void
Z80_CPU::Reset()
{
  /* Reset KMZ80 */
  kmr800_reset(&context) ;

  context.user = this ;
	context.memread = memread ;
	context.memwrite = memwrite ;
	context.ioread = ioread ;
	context.iowrite = iowrite ;
	context.busread = busread ;

	context.regs8[REGID_M1CYCLE] = 2 ;
	context.regs8[REGID_A] = (UINT8) 0;
	context.regs8[REGID_HALTED] = 0;
	context.exflag = 3;
	context.regs8[REGID_IFF1] = 0;
  context.regs8[REGID_IFF2] = 0;
	context.regs8[REGID_INTREQ] = 0;
	context.regs8[REGID_NMIREQ] = 0;
	context.regs8[REGID_IMODE] = 1;
}

void
Z80_CPU::Exec(UINT32 cycles)
{
  kmz80_exec(&context, cycles);
}

bool 
Z80_CPU::Read(UINT32 adr, UINT32 &val, UINT32 id)
{
  assert(0<=id&&id<DEVICE_MAX);
  if(device[id])
    return device[id]->Read(adr, val);
  else
    return false;
}

bool
Z80_CPU::Write(UINT32 adr, UINT32 val, UINT32 id)
{
  assert(0<=id&&id<DEVICE_MAX);
  if(device[id])
    return device[id]->Write(adr, val);
  else
    return false;
}

void
Z80_CPU::SetDevice(IDevice *d, int id)
{
  assert(0<=id&&id<DEVICE_MAX);
  device[id] = d;
}

void
Z80_CPU::Halt(bool flag)
{
  context.regs8[REGID_HALTED] = (UINT8) flag;
}

/////////////////////////////////////////////////////////////////////////////
//
// Interrupt Handler Setup
//
void
Z80_CPU::AddIntHandler(int int_type, UINT32 int_cycles, UINT32 int_address)
{
  assert(0<=int_type&&int_type<INT_TYPE_MAX);

  if(int_id[int_type]>KMEVENT_ITEM_MAX)
    int_id[int_type] = kmevent_alloc(&kme) ;

  int_adr[int_id[int_type]]  = (int_address&0xffff);
  kmevent_setevent(&kme, int_id[int_type], (void (*)(void))kmevent_handler, (void *)this);
  kmevent_settimer(&kme, int_id[int_type], int_cycles);
}

void
Z80_CPU::DelIntHandler(int int_type)
{
  assert(0<=int_type&&int_type<INT_TYPE_MAX);

  if(int_id[int_type]>=0)
  {
    kmevent_free(&kme, int_id[int_type]);
    int_id[int_type] = KMEVENT_ITEM_MAX+1;
  }
}

// 外からは呼んじゃダメ(Cとのインタフェースなので仕方なくpublic)
void
Z80_CPU::Interrupt(KMEVENT_ITEM_ID int_id)
{
  device[DEVICE_MEM]->Write(--context.sp, context.pc>>8);
  device[DEVICE_MEM]->Write(--context.sp, context.pc&0xff);
  context.pc = int_adr[int_id];
}

void
Z80_CPU::GetMemory(UINT8 *buf, UINT32 size, UINT32 adr)
{
  UINT32 i, val;

  for(i=0; i<size; i++)
  {
    device[DEVICE_MEM]->Read(adr+i,val);
    buf[i] = val;
  }
}

void 
Z80_CPU::SetMemory(UINT8 *buf, UINT32 size, UINT32 adr)
{
  UINT32 i;

  for(i=0; i<size; i++)
    device[DEVICE_MEM]->Write(adr+i,buf[i]);
}


/////////////////////////////////////////////////////////////////////////////
//
// Register Controls
//
void
Z80_CPU::SetSP(UINT32 sp)
{
	context.sp = sp&0xFFFF;
}

UINT32
Z80_CPU::GetSP()
{
  return context.sp;
}

void
Z80_CPU::SetPC(UINT32 pc)
{
	context.pc = pc&0xFFFF;
}

UINT32
Z80_CPU::GetPC()
{
  return context.pc;
}

void
Z80_CPU::SetRegs8(int id, UINT32 val)
{
  assert(0<=id&&id<REGID_MAX);
  context.regs8[id] = (UINT8) val&0xFF;
}

UINT32
Z80_CPU::GetRegs8(int id)
{
  assert(0<=id&&id<REGID_MAX);
  return context.regs8[id];
}


/////////////////////////////////////////////////////////////////////////////
} // namespace 

