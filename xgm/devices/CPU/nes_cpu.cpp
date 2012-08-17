#include <assert.h>
#include "nes_cpu.h"

#define DEBUG_RW 0

namespace xgm
{

NES_CPU::NES_CPU (double clock)
{
  NES_BASECYCLES = clock;
  bus = NULL;
}

NES_CPU::~NES_CPU ()
{
}

void Callback writeByte (void *__THIS, UINT32 adr, UINT32 val)
{
  static_cast < NES_CPU * >(__THIS)->Write (adr, val);
}

UINT32 Callback readByte (void *__THIS, UINT32 adr)
{
  UINT32 val = 0;
  static_cast < NES_CPU * >(__THIS)->Read (adr, val);
  return val;
}

void NES_CPU::startup (UINT32 address)
{
  breaked = false;
  context.PC = 0x4100;
  breakpoint = context.PC+3;
  context.P = 0x26;                 // IRZ
  assert (bus);
  bus->Write (context.PC+0, 0x20);  // JSR 
  bus->Write (context.PC+1, address & 0xff);
  bus->Write (context.PC+2, address>>8);
  bus->Write (context.PC+3, 0x4c);  // JMP 04103H 
  bus->Write (context.PC+4, breakpoint & 0xff);
  bus->Write (context.PC+5, breakpoint>>8);
}

UINT32 NES_CPU::Exec (UINT32 clock)
{
  context.clock = 0;

  while ( context.clock < clock )
  {
    if (!breaked)
    {
      //DEBUG_OUT("PC: 0x%04X\n", context.PC);
      K6502_Exec (&context);
      if (context.PC == breakpoint)
        breaked = true;
    }
    else 
    {
      if ( (clock_of_frame>>16) < clock )
        context.clock = (clock_of_frame>>16)+1;
      else
        context.clock = clock;
    }

    // フレームクロックに到達
    if ( (clock_of_frame>>16) < context.clock)
    {
      if (breaked) 
        startup (int_address);
      clock_of_frame += clock_per_frame;
      //DEBUG_OUT("NMI\n");
    }
  }

  clock_of_frame -= (context.clock<<16);

  // BS frame sequencer
  int old_quarter = frame_quarter;
  frame_quarter = 3 - (clock_of_frame / (clock_per_frame >> 2));
  frame_quarter = frame_quarter < 0 ? 0 : frame_quarter;
  if (frame_quarter != old_quarter)
  {
      FrameSequence(frame_quarter);
  }

  return context.clock; // BS return actual number of clocks executed
}

void NES_CPU::SetMemory (IDevice * b)
{
  bus = b;
}

bool NES_CPU::Write (UINT32 adr, UINT32 val, UINT32 id)
{
  #if DEBUG_RW
    DEBUG_OUT("Write: 0x%04X = 0x%02X\n", adr, val);
  #endif

  if (bus)
    return bus->Write (adr, val, id);
  else
    return false;
}

bool NES_CPU::Read (UINT32 adr, UINT32 & val, UINT32 id)
{
  if (bus)
  {
    bool result = bus->Read (adr, val, id);

    #if DEBUG_RW
      DEBUG_OUT(" Read: 0x%04X = 0x%02X\n", adr, val);
    #endif

    return result;
  }
  else
    return false;
}

void NES_CPU::FrameSequence(int s) // BS frame sequencer
{
  if (bus)
    bus->FrameSequence(s);
}

void NES_CPU::Reset ()
{
  // KM6502のリセット
  memset (&context, 0, sizeof (K6502_Context));
  context.ReadByte = readByte;
  context.WriteByte = writeByte;
  context.iRequest = K6502_INIT;
  context.clock = 0;
  context.user = this;
  context.A = 0;
  context.X = 0;
  context.Y = 0;
  context.S = 0xff;
  context.PC = breakpoint = 0xffff;
  breaked = false;
  K6502_Exec (&context);
}

void NES_CPU::Start (int start_adr, int int_adr, double int_freq, int a, int x, int y)
{
  // 割り込みアドレス設定
      int_address = int_adr;
  clock_per_frame = (int)((double)((1<<16) * (NES_BASECYCLES) / 12) / int_freq );
  clock_of_frame = 0;

  // count clock quarters
  frame_quarter = 3;

  context.A = a;
  context.X = x;
  context.Y = y;
  startup (start_adr);

  for (int i = 0; (i < (NES_BASECYCLES / 12 / int_freq )) && !breaked; i++, K6502_Exec (&context))
  {
    if (context.PC == breakpoint) breaked = true;
  }

  clock_of_frame = 0;
}

} // namespace xgm