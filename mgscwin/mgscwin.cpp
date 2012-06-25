#ifdef _MSC_VER
#pragma warning (disable:4786)
#endif
#include <iostream>
#include <fstream>
#include "mgscwin.h"

using namespace xgm;

#ifdef EMBED_MGSC
static unsigned char mgsc[]=
{
#include "mgsc.h"
};
static int mgsc_size = sizeof(mgsc);
#else
static unsigned char mgsc[0x8000];
static int mgsc_size = 0;
#endif

#define BDOS_ENTRY 0xF000

static unsigned char dosemu[0x100]=
{
#include "dosemu.h"
};

MGSC::MGSC()
{
  errmsg[0]='\0';
}


MGSC::~MGSC()
{
}

#ifndef EMBED_MGSC
bool
MGSC::LoadCompiler(std::istream *is)
{
  is->read((char *)mgsc,0x8000);
  mgsc_size = is->gcount();
  return (mgsc_size>0);  
}
#endif

void
MGSC::SetConsole(std::istream *in, std::ostream *out)
{
  bdos.SetConsole(in,out);
}

bool
MGSC::Compile(std::iostream *mml, std::iostream *mgs, int option)
{
  xgm::UINT32 sp, rp;

  if(mgsc_size==0)
  {
    sprintf(errmsg,"MGSC.COM is not loaded.");
    return false;  
  }

  bdos.Reset();
  bdos.fs["TEMP.MUS"] = mml;
  bdos.fs["TEMP.MGS"] = mgs;

  cpu.SetDevice(&ram, Z80_CPU::DEVICE_MEM);
  ram.Reset();
  ram.SetImage(dosemu,0,0x100);
  if(option&OPT_TRACK_STATUS)
  {
    ram.Write(0x90,'-');
    ram.Write(0x91,'T');
  }
  ram.SetImage((xgm::UINT8 *)mgsc,0x100,mgsc_size);
  cpu.Reset();

  /* BDOS ENTRY */
  cpu.Write(BDOS_ENTRY+0, 0x76, Z80_CPU::DEVICE_MEM); // HALT
  cpu.Write(BDOS_ENTRY+1, 0xC9, Z80_CPU::DEVICE_MEM); // RET

  sp = 0xf380 ;
  cpu.Write(--sp, 0x00, Z80_CPU::DEVICE_MEM) ;
  cpu.Write(--sp, 0xfe, Z80_CPU::DEVICE_MEM) ;
  cpu.Write(--sp, 0x18, Z80_CPU::DEVICE_MEM) ; /* JR +0 */
  cpu.Write(--sp, 0x76, Z80_CPU::DEVICE_MEM) ; /* HALT */
	rp = sp;
  cpu.Write(--sp, rp>>8, Z80_CPU::DEVICE_MEM) ;
  cpu.Write(--sp, rp&0xff, Z80_CPU::DEVICE_MEM) ;
  cpu.SetSP(sp);
	cpu.SetPC(0x100);
	cpu.SetRegs8(REGID_HALTED,0);

  while(1)
  {
    cpu.Exec(128);
    if(cpu.GetRegs8(REGID_HALTED)==0)
    {
      continue;
    }
     
    if(cpu.GetPC()==0x0000)
    {
      break;
    }
    else if(cpu.GetPC()==0x0038) // H.TIMI
    {
      cpu.SetPC(cpu.GetPC()+1);
      cpu.SetRegs8(REGID_HALTED, 0);        
    }
    else if(cpu.GetPC()==BDOS_ENTRY)
    {
      if(bdos.DoProcess(cpu))
      {
        cpu.SetRegs8(REGID_L,cpu.GetRegs8(REGID_A));
        cpu.SetRegs8(REGID_H,cpu.GetRegs8(REGID_B));
        cpu.SetPC(cpu.GetPC()+1);
        cpu.SetRegs8(REGID_HALTED, 0);  
      }
      else
      {
        sprintf(errmsg, "Unsupported BDOS call : Function %02xH", cpu.GetRegs8(REGID_C));
        return false;
      }
    }
    else
    {
      sprintf(errmsg, "CPU halted at:%04x\n", cpu.GetPC());
      return false;
    }
  }

  bdos.fs["TEMP.MGS"]->flush();
  return true;
}

const char *
MGSC::GetErrorMessage()
{
  return errmsg;
}








