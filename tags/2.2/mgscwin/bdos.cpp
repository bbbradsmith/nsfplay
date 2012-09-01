#ifdef _MSC_VER
#pragma warning (disable:4786)
#endif
#include "bdos.h"

using namespace xgm;

/////////////////////////////////////////////////////////////////////////////
// 
// CP/Mファンクションのエミュレータ
//
EMU_BDOS::EMU_BDOS()
{
  cin = &std::cin;
  cout = &std::cout;
}

EMU_BDOS::~EMU_BDOS()
{
}

void 
EMU_BDOS::Dump()
{
  VFS::iterator it;
  for(it=fs.begin();it!=fs.end();it++)
  {
    std::cout << it->first.c_str() << std::endl;
    printf("%p - ", &it->second);
    printf("%p\n", it->second->rdbuf());
  }
}

void
EMU_BDOS::Reset()
{
  DTA = 0x80;
  fs.clear();
}

static char tmp_buf[0x10000];

static void ReadFCB(FCB &fcb, Z80_CPU &cpu)
{
  int DE = (cpu.GetRegs8(REGID_D)<<8)|cpu.GetRegs8(REGID_E);
  BYTE buf[37];
  cpu.GetMemory((xgm::UINT8 *)buf,37,DE);
  fcb.SetBlock(buf);
}

static void WriteFCB(FCB &fcb, Z80_CPU &cpu)
{
  int DE = (cpu.GetRegs8(REGID_D)<<8)|cpu.GetRegs8(REGID_E);
  BYTE buf[37];
  fcb.GetBlock(buf);
  cpu.SetMemory((xgm::UINT8 *)buf, 37, DE);
}

void
EMU_BDOS::SetConsole(std::istream *in, std::ostream *out)
{
  cin = in;
  cout = out;
}

std::iostream *
OnCreateFile(FCB fcb)
{
  return NULL;
}

bool
EMU_BDOS::DoProcess(Z80_CPU &z80)
{
  BYTE buf[37];
  std::pair<VFS::iterator, bool> result;
  VFS::iterator it;
  
  int DE = (z80.GetRegs8(REGID_D)<<8)|z80.GetRegs8(REGID_E);
  int HL = (z80.GetRegs8(REGID_H)<<8)|z80.GetRegs8(REGID_L);

  switch(z80.GetRegs8(REGID_C))
  {
  case 0x00: // System Reset
    return false;

  case 0x02: // Console Out
    *cout << (char)z80.GetRegs8(REGID_E);
    return true;

  case 0x16: // Create File
    ReadFCB(fcb,z80);
    if(fs.find(fcb.GetFilename())==fs.end())
    {
      fs[fcb.GetFilename()] = OnCreateFile(fcb);
    }

    /*
    if(0)
    {
      z80.SetRegs8(REGID_A,0xFF);
      return true;
    }
    */

    fcb.dev = 0x40 + fcb.drv;
    fcb.GetBlock(buf);
    WriteFCB(fcb,z80);
    z80.SetRegs8(REGID_A,0);
    return true;

  case 0x0F: // Open File
    ReadFCB(fcb,z80);
    it = fs.find(std::string(fcb.GetFilename()));
    if(it==fs.end())
    {
      z80.SetRegs8(REGID_A,0xFF);
      return true;
    }
    fcb.drv = 0x01;
    it->second->seekg(0,std::ios::end);
    fcb.fsize = it->second->tellg();
    fcb.rsize = 128;
    fcb.dev = 0x40 + fcb.drv;
    fcb.fclst = 0x01;
    fcb.lclst = 0x20;
    fcb.rclst = 0x00;
    fcb.date = 0x1234;
    fcb.time = 0x1234;
    WriteFCB(fcb,z80);
    z80.SetRegs8(REGID_A,0);
    return true;

  case 0x10: // Close
    z80.SetRegs8(REGID_A,0);
    return true;

  case 0x1A: // DTA setup
    DTA = DE;
    return true;

  case 0x21: // Random Read (CP/M compatible)
    memset(tmp_buf,0,128);
    ReadFCB(fcb,z80);

    it = fs.find(std::string(fcb.GetFilename()));
    if(it==fs.end()||fcb.fsize<128*fcb.rrec)
    {
      z80.SetRegs8(REGID_A,0x01);
      return true;
    }

    it->second->seekg(128*fcb.rrec, std::ios_base::beg);
    it->second->read(tmp_buf,128);

    WriteFCB(fcb,z80);
    z80.SetMemory((xgm::UINT8 *)tmp_buf,128,DTA);
    z80.SetRegs8(REGID_A,0);
    return true;

  case 0x22: // Random Write (CP/M compatible)
    ReadFCB(fcb,z80);
    it = fs.find(std::string(fcb.GetFilename()));
    if(it==fs.end())
    {
      z80.SetRegs8(REGID_A,0x01);
      return true;
    }

    z80.GetMemory((xgm::UINT8 *)tmp_buf, 128, DTA);
    it->second->seekp(128*fcb.rrec, std::ios::beg);
    if(it->second->fail())
    {
      it->second->seekp(0, std::ios::end);
      it->second->clear();
    }
    it->second->write(tmp_buf,128);
    it->second->seekg(0,std::ios::end);
    fcb.fsize = it->second->tellg();
    WriteFCB(fcb,z80);
    z80.SetRegs8(REGID_A,0);
    return true;

  case 0x6f: // MSX-DOS Version
    z80.SetRegs8(REGID_A,0);
    z80.SetRegs8(REGID_B,0);
    return true;

  default: 
    return false;
  }
};