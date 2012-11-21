#include <assert.h>
#include "nes_cpu.h"

#define DEBUG_RW 0
#define TRACE 1

#if TRACE
const char* OP_NAME[256] = {
/*  0x-0   0x-1   0x-2   0x-3   0x-4   0x-5   0x-6   0x-7   0x-8   0x-9   0x-A   0x-B   0x-C   0x-D   0x-E   0x-F           */
    "BRK", "ORA", "kil", "slo", "nop", "ORA", "ASL", "slo", "PHP", "ORA", "ASL", "anc", "nop", "ORA", "ASL", "slo", /* 0x0- */
    "BPL", "ORA", "kil", "slo", "nop", "ORA", "ASL", "slo", "CLC", "ORA", "nop", "slo", "nop", "ORA", "ASL", "slo", /* 0x1- */
    "JSR", "AND", "kil", "rla", "BIT", "AND", "ROL", "rla", "PLP", "AND", "ROL", "anc", "BIT", "AND", "ROL", "rla", /* 0x2- */
    "BMI", "AND", "kil", "rla", "nop", "AND", "ROL", "rla", "SEC", "AND", "nop", "rla", "nop", "AND", "ROL", "rla", /* 0x3- */
    "RTI", "EOR", "kil", "sre", "nop", "EOR", "LSR", "sre", "PHA", "EOR", "LSR", "alr", "JMP", "EOR", "LSR", "sre", /* 0x4- */
    "BVC", "EOR", "kil", "sre", "nop", "EOR", "LSR", "sre", "CLI", "EOR", "nop", "sre", "nop", "EOR", "LSR", "sre", /* 0x5- */
    "RTS", "ADC", "kil", "rra", "nop", "ADC", "ROR", "rra", "PLA", "ADC", "ROR", "arr", "JMP", "ADC", "ROR", "rra", /* 0x6- */
    "BVS", "ADC", "kil", "rra", "nop", "ADC", "ROR", "rra", "SEI", "ADC", "nop", "rra", "nop", "ADC", "ROR", "rra", /* 0x7- */
    "nop", "STA", "nop", "sax", "STY", "STA", "STX", "sax", "DEY", "nop", "TXA", "xaa", "STY", "STA", "STX", "sax", /* 0x8- */
    "BCC", "STA", "kil", "ahx", "STY", "STA", "STX", "sax", "TYA", "STA", "TXS", "tas", "shy", "STA", "shx", "ahx", /* 0x9- */
    "LDY", "LDA", "LDX", "lax", "LDY", "LDA", "LDX", "lax", "TAY", "LDA", "TAX", "lax", "LDY", "LDA", "LDX", "lax", /* 0xA- */
    "BCS", "LDA", "kil", "lax", "LDY", "LDA", "LDX", "lax", "CLV", "LDA", "TSX", "las", "LDY", "LDA", "LDX", "lax", /* 0xB- */
    "CPY", "CMP", "nop", "dcp", "CPY", "CMP", "DEC", "dcp", "INY", "CMP", "DEX", "axs", "CPY", "CMP", "DEC", "dcp", /* 0xC- */
    "BNE", "CMP", "kil", "dcp", "nop", "CMP", "DEC", "dcp", "CLD", "CMP", "nop", "dcp", "nop", "CMP", "DEC", "dcp", /* 0xD- */
    "CPX", "SBC", "nop", "isc", "CPX", "SBC", "INC", "isc", "INX", "SBC", "NOP", "sbc", "CPX", "SBC", "INC", "isc", /* 0xE- */
    "BEQ", "SBC", "kil", "isc", "nop", "SBC", "INC", "isc", "SED", "SBC", "nop", "isc", "nop", "SBC", "INC", "isc"  /* 0xF- */
};
#endif

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

      #if TRACE
        Uword TPC = context.PC;
        UINT32 tb[3];
        bus->Read((TPC+0) & 0xFFFF, tb[0]);
        bus->Read((TPC+1) & 0xFFFF, tb[1]);
        bus->Read((TPC+2) & 0xFFFF, tb[2]);
        DEBUG_OUT("%04X: A=%02X X=%02X Y=%02X P=%02X S=%02X %c%c > ",
            context.PC,
            context.A, context.X, context.Y, context.P, context.S,
            context.iRequest ? 'I':'i',
            context.iMask ? 'M':'m');
      #endif

      K6502_Exec (&context);

      #if TRACE
        DEBUG_OUT("%s", OP_NAME[context.lastcode]);
        int oplen = context.PC - TPC;
        for (int i=0; i<3; ++i)
        {
            if (i == 0 || i < oplen)
            {
                DEBUG_OUT(" %02X", tb[i]);
            }
        }
        DEBUG_OUT("\n");
      #endif

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

  return context.clock; // return actual number of clocks executed
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
  clock_per_frame = (int)((double)((1<<16) * NES_BASECYCLES) / int_freq );
  clock_of_frame = 0;

  // count clock quarters
  frame_quarter = 3;

  context.A = a;
  context.X = x;
  context.Y = y;
  startup (start_adr);

  for (int i = 0; (i < (NES_BASECYCLES / int_freq )) && !breaked; i++, K6502_Exec (&context))
  {
    if (context.PC == breakpoint) breaked = true;
  }

  clock_of_frame = 0;
}

} // namespace xgm