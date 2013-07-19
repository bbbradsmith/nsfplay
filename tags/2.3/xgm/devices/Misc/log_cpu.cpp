#include "log_cpu.h"

namespace xgm
{

// log_level
//   0 - no logging
//   1 - log APU writes only
//   2 - log all writes
//   3 - log all writes and reads

CPULogger::CPULogger()
{
    log_level = 0;
    soundchip = 0;
    file = NULL;
    filename = NULL;
    frame_count = 0;
}

CPULogger::~CPULogger()
{
    if (file)
        ::fclose(file);
    if (filename)
        delete [] filename;
}

void CPULogger::Reset ()
{
}

bool CPULogger::Write (UINT32 adr, UINT32 val, UINT32 id)
{
    if (log_level < 1) return false;
    if (log_level < 2)
    {
        bool apu = false;
        if (adr >= 0x4000 && adr <= 0x4013)                     apu = true;
        if (adr == 0x4015)                                      apu = true;
        if (adr == 0x4017)                                      apu = true;
        if ((soundchip &  1) && adr >= 0x9000 && adr <= 0x9003) apu = true; // vrc6
        if ((soundchip &  1) && adr >= 0xA000 && adr <= 0xA002) apu = true; // vrc6
        if ((soundchip &  1) && adr >= 0xB000 && adr <= 0xB002) apu = true; // vrc6
        if ((soundchip &  2) && adr == 0x9010)                  apu = true; // vrc7
        if ((soundchip &  2) && adr == 0x9030)                  apu = true; // vrc7
        if ((soundchip &  4) && adr >= 0x4040 && adr <= 0x4092) apu = true; // fds
        if ((soundchip &  8) && adr >= 0x5000 && adr <= 0x5013) apu = true; // mmc5
        if ((soundchip &  8) && adr == 0x5015)                  apu = true; // mmc5
        if ((soundchip & 16) && adr == 0x4800)                  apu = true; // n163
        if ((soundchip & 16) && adr == 0xF800)                  apu = true; // n163
        if ((soundchip & 32) && adr == 0xC000)                  apu = true; // 5b
        if ((soundchip & 32) && adr == 0xE000)                  apu = true; // 5b
        if (!apu) return false;
    }

    if (file)
        ::fprintf(file, "WRITE(%04X,%02X)\n", adr, val);

    return false;
}

bool CPULogger::Read (UINT32 adr, UINT32 & val, UINT32 id)
{
    if (log_level > 2 && file)
        ::fprintf(file, "READ(%04X)\n", adr);
    return false;
}

void CPULogger::SetOption (int id, int val)
{
    switch (id)
    {
    case 0:
        log_level = val;
        break;
    default:
        break;
    }
}

int CPULogger::GetLogLevel () const
{
    return log_level;
}

void CPULogger::SetSoundchip (UINT8 soundchip_)
{
    soundchip = soundchip_;
}

void CPULogger::SetFilename (const char * filename_)
{
    if (!filename_)
    {
        delete [] filename;
        filename = NULL;
        return;
    }

    if (filename == NULL || ::strcmp(filename,filename_))
    {
        delete [] filename;
        filename = new char[::strlen(filename_)+1];
        ::strcpy(filename, filename_);
    }
}

void CPULogger::Begin (const char* title)
{
    if (file)
    {
        ::fclose(file);
        file = NULL;
    }
    if (filename)
        file = ::fopen(filename, "at");

    if (file)
        ::fprintf(file, "BEGIN(\"%s\")\n", title);\
    frame_count = 0;
}

void CPULogger::Init (UINT8 reg_a, UINT8 reg_x)
{
    if (file)
        ::fprintf(file, "INIT(%02X,%02X)\n", reg_a, reg_x);
}

void CPULogger::Play ()
{
    if (file)
        ::fprintf(file, "PLAY(%d)\n", frame_count);
    ++frame_count;
}

} // namespace xgm
