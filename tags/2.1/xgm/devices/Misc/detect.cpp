#include "detect.h"

namespace xgm {

  BasicDetector::BasicDetector(int bufbits)
  {
    m_bufsize = 1<<bufbits;
    m_bufmask = m_bufsize - 1;
    m_stream_buf = new int [m_bufsize];
    m_time_buf = new int [m_bufsize];
  }

  BasicDetector::~BasicDetector()
  {
    delete [] m_stream_buf;
    delete [] m_time_buf;
  }

  void BasicDetector::Reset ()
  {
    int i;

    for (i = 0; i < m_bufsize; i++)
    {
      m_stream_buf[i] = -i;
      m_time_buf[i] = 0;
    }

    m_current_time = 0;
    m_wspeed = 0;

    m_bidx = 0;
    m_blast = 0;
    m_loop_start = -1;
    m_loop_end = -1;
    m_empty = true;
  }

  bool BasicDetector::Write (UINT32 adr, UINT32 val, UINT32 id)
  {
    m_empty = false;
    m_time_buf[m_bidx] = m_current_time;
    m_stream_buf[m_bidx] = ((adr & 0xffff) << 8) | (val & 0xff);
    m_bidx = (m_bidx + 1) & m_bufmask;
    return false;
  }

  bool BasicDetector::IsLooped (int time_in_ms, int match_second, int match_interval)
  {
    int i, j;
    int match_size, match_length;

    if (time_in_ms - m_current_time < match_interval)
      return false;

    m_current_time = time_in_ms;

    if (m_bidx <= m_blast)
      return false;
    if (m_wspeed)
      m_wspeed = (m_wspeed + m_bidx - m_blast) / 2;
    else
      m_wspeed = m_bidx - m_blast;      // ‰‰ñ
    m_blast = m_bidx;

    match_size = m_wspeed * match_second / match_interval;
    match_length = m_bufsize - match_size;

    if (match_length < 0)
      return false;

    for (i = 0; i < match_length; i++)
    {
      for (j = 0; j < match_size; j++)
      {
        if (m_stream_buf[(m_bidx + j + match_length) & m_bufmask] !=
            m_stream_buf[(m_bidx + i + j) & m_bufmask])
          break;
      }
      if (j == match_size)
      {
        m_loop_start = m_time_buf[(m_bidx + i) & m_bufmask];
        m_loop_end = m_time_buf[(m_bidx + match_length) & m_bufmask];
        return true;
      }
    }
    return false;
  }
} // namespace xgm