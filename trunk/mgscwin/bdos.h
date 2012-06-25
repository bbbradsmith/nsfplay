#ifndef _BDOS_H_
#define _BDOS_H_
// BDOS Emulator
#include <iostream>
#include <strstream>
#include <string>
#include <map>
#include "xgm.h"

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

class FCB
{
public:
  BYTE drv;
  char  name[9];
  char  ext[4];
  BYTE cblk;
  BYTE sys1;
  BYTE rsize;
  BYTE sys2;
  DWORD fsize;
  WORD date;
  WORD time;
  BYTE dev;
  BYTE dloc;
  WORD fclst;
  WORD lclst;
  WORD rclst;
  BYTE crec;
  DWORD rrec;

public:
  const char *GetFilename()
  {
    static char buf[128];
    char *p;

    strcpy(buf,name);
    p = strchr(buf,' ');
    if(p) *p='\0';
    strcat(buf,".");
    strcat(buf,ext);
    p = strchr(buf,' ');
    if(p) *p='\0';
    return buf;
  }

  void SetBlock(BYTE data[36])
  {
    drv = data[0];
    memcpy(name,data+1,8);
    name[8]='\0';
    memcpy(ext,data+9,3);
    ext[3]='\0';
    cblk = data[12];
    sys1 = data[13];
    rsize = data[14];
    sys2 = data[15];
    fsize = data[16]|(data[17]<<8)|(data[18]<<16)|(data[19]<<24);
    date = data[20]|(data[21]<<8);
    time = data[22]|(data[23]<<8);
    dev  = data[24];
    dloc = data[25];
    fclst = data[26]|(data[27]<<8);
    lclst = data[28]|(data[29]<<8);
    rclst = data[30]|(data[31]<<8);
    crec = data[32];
    rrec = data[33]|(data[34]<<8)|(data[35]<<16);
  }
  void GetBlock(BYTE data[36])
  {
    data[0] = drv;
    memcpy(data+1, name,8);
    memcpy(data+9, ext,3);
    data[12] = (BYTE)cblk;
    data[13] = (BYTE)sys1;
    data[14] = (BYTE)rsize;
    data[15] = (BYTE)sys2;
    data[16] = (BYTE)(fsize&0xff);
    data[17] = (BYTE)((fsize>>8)&0xff);
    data[18] = (BYTE)((fsize>>16)&0xff);
    data[19] = (BYTE)((fsize>>24)&0xff);
    data[20] = (BYTE)(date&0xff);
    data[21] = (BYTE)((date>>8)&0xff);
    data[22] = (BYTE)(time&0xff);
    data[23] = (BYTE)((time>>8)&0xff);
    data[24] = (BYTE)dev;
    data[25] = (BYTE)dloc;
    data[26] = (BYTE)(fclst&0xff);
    data[27] = (BYTE)((fclst>>8)&0xff);
    data[28] = (BYTE)(lclst&0xff);
    data[29] = (BYTE)((lclst>>8)&0xff);
    data[30] = (BYTE)rclst&0xff;
    data[31] = (BYTE)((rclst>>8)&0xff);
    data[32] = (BYTE)crec;
    data[33] = (BYTE)(rrec&0xff);
    data[34] = (BYTE)((rrec>>8)&0xff);
    data[35] = (BYTE)((rrec>>16)&0xff); 
    data[36] = (BYTE)0;
  }

  void Dump()
  {
    printf("Drive          : %02x\n", drv);
    printf("Filename       : %8s.%3s\n", name, ext);
    printf("Current Block  : %d\n", cblk);
    printf("Reserved(1)    : %02x\n", sys1);
    printf("Record Size    : %d\n", rsize);
    printf("Reserved(2)    : %02x\n", sys2);
    printf("File Size      : %d\n", fsize);
    printf("Date           : %04x\n", date);
    printf("Time           : %04x\n", time);
    printf("Device Id      : %02x\n", dev);
    printf("Dir Location   : %02x\n", dloc);
    printf("First Cluster  : %02x\n", fclst);
    printf("Last  Cluster  : %02x\n", lclst);
    printf("Relative Pos   : %02x\n", rclst);
    printf("Current Record : %d\n", crec);
    printf("Random Record  : %d\n", rrec);
  }

};

class EMU_BDOS
{
  FCB fcb;
  std::ostream *cout;
  std::istream *cin;
protected:
  UINT32 DTA;

public:
  typedef std::map<std::string, std::iostream *> VFS; // 仮想ファイルシステム
  VFS fs;
  EMU_BDOS();
  void Dump();
  ~EMU_BDOS();
  void Reset();
  void SetConsole(std::istream *in, std::ostream *out);
  bool DoProcess(xgm::Z80_CPU &z80);
};

#endif