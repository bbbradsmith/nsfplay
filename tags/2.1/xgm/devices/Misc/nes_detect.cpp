#include "nes_detect.h"

namespace xgm
{
  static UINT8 maskAPU[16] = 
  { 
#if 1
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0x00, 0xff, 0xff,
    0x3f, 0x00, 0x8f, 0xf8
#else
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
#endif
  };

  bool NESDetector::Write (UINT32 adr, UINT32 val, UINT32 id)
  {
    if (
        (0x4000 <= adr && adr <= 0x4013) // APU / DMC
      ||(0x4015 == adr)
      ||(0x4017 == adr)
      ||(0x9000 <= adr && adr <= 0x9002) // VRC6
      ||(0xA000 <= adr && adr <= 0xA002)
      ||(0xB000 <= adr && adr <= 0xB002)
      ||(0x9010 == adr)                  // VRC7
      ||(0x9030 == adr)
      ||(0x4040 <= adr && adr <= 0x4092) // FDS
      ||(0x4800 == adr)                  // N163
      ||(0xF800 == adr)
      ||(0x5000 <= adr && adr <= 0x5007) // MMC5
      ||(0x5010 == adr)
      ||(0x5011 == adr)
      ||(0xC000 == adr)                  // 5B
      ||(0xE000 == adr)
      )
    {
        return BasicDetector::Write(adr, val, id);
    }

    return false;
  }

  NESDetectorEx::NESDetectorEx()
  {
    int bufsize_table[]={ 
      15, 15, 15, 15, 15, // SQR0, SQR1, TRI, NOIZ, DPCM
      14, 14, 14, 14,// N106[0-3]
      14, 14, 14, 14 // N106[4-7]
    };
    for(int i=0;i<MAX_CH;i++)
      m_LD[i] = new BasicDetector(bufsize_table[i]);
  }

  NESDetectorEx::~NESDetectorEx()
  {
    for(int i=0;i<MAX_CH;i++)
      delete m_LD[i];
  }

  void NESDetectorEx::Reset()
  {
    for(int i=0;i<MAX_CH;i++) 
    {
      m_LD[i]->Reset();
      m_looped[i] = false;
    }
  }

  bool NESDetectorEx::IsLooped(int time_in_ms, int match_second, int match_interval)
  {
    bool all_empty = true, all_looped = true;
    for(int i=0;i<MAX_CH;i++)
    {
      if(!m_looped[i])
      {
        m_looped[i] = m_LD[i]->IsLooped(time_in_ms, match_second, match_interval);
        if(m_looped[i])
        {
          m_loop_start = m_LD[i]->GetLoopStart();
          m_loop_end = m_LD[i]->GetLoopEnd();
        }
      }
      all_looped &= m_looped[i]|m_LD[i]->IsEmpty();
      all_empty &= m_LD[i]->IsEmpty();
    }

    return !all_empty&all_looped;
  }

  bool NESDetectorEx::Write (UINT32 adr, UINT32 val, UINT32 id)
  {
    if (0x4000 <= adr && adr < 0x4004)
      m_LD[SQR_0]->Write(adr,val&maskAPU[adr-0x4000]);
    else if (0x4004 <= adr && adr < 0x4008)
      m_LD[SQR_1]->Write(adr,val&maskAPU[adr-0x4000]);
    else if (0x4008 <= adr && adr < 0x400C)
      m_LD[TRI]->Write(adr,val&maskAPU[adr-0x4000]);
    else if (0x400C <= adr && adr < 0x4010)
      m_LD[NOIZ]->Write(adr,val&maskAPU[adr-0x4000]);
    else if (adr==0x4012||adr==0x4013)
      m_LD[DPCM]->Write(adr,val); 
    else if (0xF800 == adr)
      m_n106_addr = val;
    else if (0x4800 == adr)
    {
      if(0x40<=m_n106_addr)
      {
        m_LD[N106_0+((m_n106_addr>>3)&7)]->Write(m_n106_addr,val);

      }
      if (m_n106_addr & 0x80) m_n106_addr++;
    }
    return false;
  }

} // namespace xgm