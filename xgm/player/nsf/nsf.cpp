#if defined(_MSC_VER) || defined(__MINGW32__)
#include <windows.h>
#else
#define stricmp strcasecmp
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "nsf.h"
extern "C"
{
#include "pls/ppls.h"
}

namespace xgm
{

UINT8 CONVERT_REGN[4]      = { 0x01, 0x02, 0x03, 0x03 }; // NTSC, PAL, DUAL, DUAL
int   CONVERT_REGN_PREF[4] = {   -1,   -1,    0,    1 }; // Any,  Any, NTSC,  PAL

const int NSFE_ERROR_SIZE = 256;
static char nsfe_error_[NSFE_ERROR_SIZE];

static const char* nsfe_error = "(no NSFe loaded)";
static const char* nsf_error = "(no NSF loaded)";

static int is_sjis_prefix(int c)
{
  if((0x81<=c&&c<=0x9F)||(0xE0<=c&&c<=0xFC)) return 1 ;
  else return 0 ;
}

  NSF::NSF ():SoundDataMSP ()
  {
    body = NULL;
    default_playtime = 5 * 60 * 1000;
    default_fadetime = 5 * 1000;
    default_loopnum = 0;

    title = title_nsf;
    artist = artist_nsf;
    copyright = copyright_nsf;
    ripper = "";
    nsfe_image = NULL;
    nsfe_plst = NULL;
    nsfe_plst_size = 0;
    for (unsigned int i=0; i<NSFE_MIXES; ++i) nsfe_mixe[i] = NSFE_MIXE_DEFAULT;
  }

  NSF::~NSF ()
  {
    delete[]body;
    delete[]nsfe_image;
  }

  void NSF::SetDefaults (int p, int f, int l)
  {
    default_playtime = p;
    default_fadetime = f;
    default_loopnum = l;
  }

  static const char *print_time (int time)
  {
    static char buf[32];
    int h, m, s = 0;

    if (time < 0)
      return "";

    time /= 1000;
    s = time % 60;
    time /= 60;
    m = time % 60;
    time /= 60;
    h = time;

    if (h)
      sprintf (buf, "%02d:%02d:%02d", h, m, s);
    else
      sprintf (buf, "%02d:%02d", m, s);

    return buf;
  }

  const char *NSF::GetPlaylistString (const char *format, bool b)
  {
    static char buf[NSF_MAX_PATH + 128];
    char *p = buf;

    const char *t = GetTitleString (format);

    p += sprintf (p, "%s::NSF,$%02x,", filename, song + 1);

    while (*t != '\0')
    {
      if (*t == '\\' || *t == ',')
        *(p++) = '\\';
      *(p++) = *(t++);
    }

    if (b)
    {
      if (time_in_ms >= 0)
        p += sprintf (p, ",%s", print_time (time_in_ms));
      else
        p += sprintf (p, ",");

      if (loop_in_ms >= 0)
        p += sprintf (p, ",%s", print_time (loop_in_ms));
      else
        p += sprintf (p, ",");

      if (fade_in_ms >= 0)
        p += sprintf (p, ",%s", print_time (fade_in_ms));
    }

    (*p) = '\0';
    return buf;
  }


  const char *NSF::GetTitleString (const char *format, int song)
  {
    int wp=0;

    char fpath[sizeof(this->filename)];
    memcpy(fpath,this->filename,sizeof(this->filename));
    char *fname = strrchr(fpath,'\\');
    if(fname!=NULL) *(fname++) = '\0'; else fname = fpath;

    if(song<0) song = this->song;
    UINT8 nsfe_ei = nsfe_plst ? nsfe_plst[song] : song;

    if (!title_unknown)
    {
      return print_title;
    }
    
    if(format==NULL||strlen(format)>128)
      format = "%L (%n/%e) %T - %A";

    print_title[wp] = '\0';

    while(wp<256&&*format)
    {
      if(is_sjis_prefix(*format))
      {
        print_title[wp++] = *format++;
        print_title[wp++] = *format++;
        continue;
      }
      else if(*format=='%')
      {
        switch(*(++format))
        {
        case 'F':
        case 'f':
          wp += sprintf(print_title+wp, "%s", fname);
          format++;
          break;
        case 'P':
        case 'p':
          wp += sprintf(print_title+wp, "%s", fpath);
          format++;
          break;
        case 'T':
        case 't':
          wp += sprintf(print_title+wp, "%s", this->title);
          format++;
          break;
        case 'A':
        case 'a':
          if (nsfe_entry[nsfe_ei].taut[0] != 0)
            wp += sprintf(print_title+wp, "%s", nsfe_entry[nsfe_ei].taut);
          else
            wp += sprintf(print_title+wp, "%s", this->artist);
          format++;
          break;
        case 'C':
        case 'c':
          wp += sprintf(print_title+wp, "%s", this->copyright);
          format++;
          break;
        case 'L':
        case 'l':
          wp += sprintf(print_title+wp, "%s", nsfe_entry[nsfe_ei].tlbl);
          format++;
          break;
        case 'N':
          wp += sprintf(print_title+wp, "$%02x", song+1);
          format++;
          break;
        case 'n':
          wp += sprintf(print_title+wp, "%03d", song+1);
          format++;
          break;
        case 'S':
          wp += sprintf(print_title+wp, "$%02x", this->start);
          format++;
          break;
        case 's':
          wp += sprintf(print_title+wp, "%03d", this->start);
          format++;
          break;
        case 'E':
          wp += sprintf(print_title+wp, "$%02x", this->songs);
          format++;
          break;
        case 'e':
          wp += sprintf(print_title+wp, "%03d", this->songs);
          format++;
          break;
        default:
          break;
        }
      }
      else
      {
        print_title[wp++] = *format++;
      }
    }
    
    print_title[wp]='\0';

    // strip trailing whitespace
    for (--wp; wp >= 0 && print_title[wp] == ' '; --wp)
    {
      print_title[wp] = '\0';
    }

    // strip leading whitespace
    for (wp = 0; print_title[wp] == ' '; ++wp) {}
    if (wp > 0)
    {
      int i=0;
      do
      {
        print_title[i] = print_title[i+wp];
        if (print_title[i] == '\0')
            break;
        ++i;
      } while (true);
    }

    title_unknown = false;
    return print_title;
  }

  void NSF::SetTitleString (char *str)
  {
    strncpy (print_title, str, 255);
    print_title[255] = '\0';
  }

  bool NSF::LoadFile (const char *fn)
  {
    FILE *fp = NULL;
    UINT8 *buf = NULL;          //MAX 256KB
    PLSITEM *pls = NULL;
    int size, rsize;
    const char *ext;
    const char *ext_next;
    nsf_error = "";
    nsfe_error = "";

    pls = PLSITEM_new (fn);
    if (!pls)
    {
      nsf_error = "Could not create playlist?";
      goto Error_Exit;
    }

    // find last . in filename
    ext = strchr(fn,'.');
    if (ext)
    {
      while ((ext_next = strchr(ext+1,'.'))) ext = ext_next;
    }
    else ext = "";

    if (pls->type == 3)
    {
      strncpy (filename, pls->filename, NSF_MAX_PATH);
    }
    else if (ext && (!stricmp(ext, ".nsf") || !stricmp(ext, ".nsfe")))
    {
      strncpy (filename, fn, NSF_MAX_PATH);
    }
    else
    {
      nsf_error = "File extension not recognized.";
      goto Error_Exit;
    }

    filename[NSF_MAX_PATH - 1] = '\0';
    fp = fopen (filename, "rb");
    if (fp == NULL)
    {
      nsf_error = "Could not open file.";
      goto Error_Exit;
    }

    fseek (fp, 0L, SEEK_END);
    size = ftell (fp);
    fseek (fp, 0L, SEEK_SET);
    buf = new UINT8[size];
    rsize = fread (buf, 1, size, fp);

    if (size != rsize)
    {
      nsf_error = "File size mismatch? Corrupt file?";
      goto Error_Exit;
    }
    if (Load (buf, size) == false)
    {
      //nsf_error set by Load
      goto Error_Exit;
    }

    if (pls->type == 3)
    {
      SetTitleString (pls->title);
      song = pls->song;
      playlist_mode = true;
      title_unknown = false;
      enable_multi_tracks = false;
    }
    else
    {
      playlist_mode = false;
      title_unknown = true;
      enable_multi_tracks = true;
    }

    time_in_ms = pls->time_in_ms;
    loop_in_ms = pls->loop_in_ms;
    fade_in_ms = pls->fade_in_ms;
    loop_num = pls->loop_num;

    if (time_in_ms < 0)
      playtime_unknown = true;
    else
      playtime_unknown = false;

    fclose (fp);
    delete[]buf;
    PLSITEM_delete (pls);

    nsf_error = "";
    return true;

  Error_Exit:
    if (pls)
      PLSITEM_delete (pls);
    if (buf)
      delete[]buf;
    if (fp)
      fclose (fp);
    //nsf_error set above
    return false;
  }

  void NSF::SetLength (int t)
  {
    time_in_ms = t;
    playtime_unknown = false;
  }

  int NSF::GetPlayTime ()
  {
    int s = song;
    if (nsfe_plst) s = nsfe_plst[song];
    if (nsfe_entry[s].time >= 0)
    {
      return nsfe_entry[s].time;
    }

    return time_in_ms < 0 ? default_playtime : time_in_ms;
  }

  int NSF::GetLoopTime ()
  {
    return loop_in_ms < 0 ? 0 : loop_in_ms;
  }

  int NSF::GetFadeTime ()
  {
    int s = song;
    if (nsfe_plst) s = nsfe_plst[song];
    if (nsfe_entry[s].fade >= 0)
    {
      return nsfe_entry[s].fade;
    }

    if (fade_in_ms < 0)
      return default_fadetime;
    else if (fade_in_ms == 0)
      return 50;
    else
      return fade_in_ms;
  }

  int NSF::GetLoopNum ()
  {
    return loop_num > 0 ? loop_num - 1 : default_loopnum - 1;
  }

  int NSF::GetLength ()
  {
    return GetPlayTime () + GetLoopTime () * GetLoopNum () + GetFadeTime ();
  }

  int NSF::GetSong ()
  {
    return song;
  }
  
  int NSF::GetSongNum ()
  {
    return songs;
  }

  bool NSF::UseNSFePlaytime()
  {
    if(!nsfe_plst) return false;
    return nsfe_entry[nsfe_plst[song]].time >= 0;
  }

  void NSF::SetSong (int s)
  {
    song = s % songs;
  }

  bool NSF::Load (UINT8 * image, UINT32 size)
  {
    nsf_error = "";
    nsfe_error = "";

    if (size < 4) // no FourCC
    {
      nsf_error = "File too small for FourCC ID.";
      return false;
    }

    nsf2_bits = 0;
    vrc7_type = -1; // default
    vrc7_patches = NULL; // none

    // fill NSFe values with defaults

    // 'plst'
    nsfe_plst = NULL;
    nsfe_plst_size = 0;

    // entries 'tlbl', 'taut', 'time', 'fade', 'psfx'
    for (unsigned int i=0; i < NSFE_ENTRIES; ++i)
    {
      nsfe_entry[i].tlbl = "";
      nsfe_entry[i].taut = "";
      nsfe_entry[i].time = -1;
      nsfe_entry[i].fade = -1;
      nsfe_entry[i].psfx = false;
    }

    // 'mixe'
    for (unsigned int i=0; i<NSFE_MIXES; ++i) nsfe_mixe[i] = NSFE_MIXE_DEFAULT;

    // load the NSF or NSFe

    memcpy (magic, image, 4);
    magic[4] = '\0';

    if (strcmp ("NESM", magic))
    {
      bool result = LoadNSFe(image, size, false);
      nsf_error = nsfe_error;
      return result;
    }

    if (size < 0x80) // no header?
    {
      nsf_error = "File too small for NSF header.";
      return false;
    }

    version = image[0x05];
    total_songs = songs = image[0x06];
    start = image[0x07];
    load_address = image[0x08] | (image[0x09] << 8);
    init_address = image[0x0a] | (image[0x0B] << 8);
    play_address = image[0x0c] | (image[0x0D] << 8);
    memcpy (title_nsf, image + 0x0e, 32);
    title_nsf[31] = '\0';
    title = title_nsf;
    memcpy (artist_nsf, image + 0x2e, 32);
    artist_nsf[31] = '\0';
    artist = artist_nsf;
    memcpy (copyright_nsf, image + 0x4e, 32);
    copyright_nsf[31] = '\0';
    copyright = copyright_nsf;
    ripper = ""; // NSFe only
    text = NULL; // NSFe only
    text_len = 0; // NSFe only
    speed_ntsc = image[0x6e] | (image[0x6f] << 8);
    memcpy (bankswitch, image + 0x70, 8);
    speed_pal = image[0x78] | (image[0x79] << 8);
    pal_ntsc = image[0x7a];
    soundchip = image[0x7b];
    nsf2_bits = image[0x7c];
    unsigned int suffix =
        (image[0x7d] <<  0) |
        (image[0x7e] <<  8) |
        (image[0x7f] << 16) ;

    if (version < 2) nsf2_bits = 0;

    regn      = CONVERT_REGN[     pal_ntsc & 3];
    regn_pref = CONVERT_REGN_PREF[pal_ntsc & 3];

    if(speed_ntsc ==0) speed_ntsc  = 16639;
    if(speed_pal  ==0) speed_pal   = 19997;
    if(speed_dendy==0) speed_dendy = speed_pal;

    use_vrc6 = soundchip &  1 ? true : false;
    use_vrc7 = soundchip &  2 ? true : false;
    use_fds  = soundchip &  4 ? true : false;
    use_mmc5 = soundchip &  8 ? true : false;
    use_n106 = soundchip & 16 ? true : false;
    use_fme7 = soundchip & 32 ? true : false;

    memcpy (extra, image + 0x7c, 4);

    delete[]body;
    body = new UINT8[size - 0x80];
    memcpy (body, image + 0x80, size - 0x80);
    bodysize = size - 0x80;

    song = start - 1;

    if (suffix != 0)
    {
        suffix += 0x80; // add header to suffix location
        int suffix_size = size - suffix;
        if (suffix_size < 0) suffix_size = 0; // shouldn't happen?
        bool result = LoadNSFe(image + suffix, UINT32(suffix_size), true);
        if ((nsf2_bits & 0x80) && !result)
        {
            nsf_error = nsfe_error;
            return false; // NSF2 bit 7 indicates metadata parsing is mandatory
        }
    }

    nsf_error = "";
    return true;
  }

  bool NSF::LoadNSFe (UINT8 * image, UINT32 size, bool nsf2)
  {
    // helper for parsing strings
    #define NSFE_STRING(p) \
      if (n >= chunk_size) break; \
      p = reinterpret_cast<char*>(chunk+n); \
      while (n < chunk_size && chunk[n] != 0) ++n; \
      if(chunk[n] == 0) ++n;

    // store entire file for string references, etc.
    delete[] nsfe_image;
    nsfe_image = new UINT8[size+1];
    ::memcpy(nsfe_image, image, size);
    nsfe_image[size] = 0; // null terminator for safety
    image = nsfe_image;

    bool info = false;
    bool data = false;
    UINT32 chunk_offset = 0;
    if (!nsf2)
    {
        if (size < 4) // no FourCC
        {
          nsfe_error = "File too small for FourCC ID.";
          return false;
        }

        memcpy (magic, image, 4);
        magic[4] = '\0';

        if (strcmp ("NSFE", magic))
        {
            // note anything that's not NESM (NSF) ends up here, not just NSFE
            nsfe_error = "Unknown FourCC ID at start of file.";
            return false;
        }
        chunk_offset = 4; // skip 'NSFE'

        // NSFe has no speed specification by default (see RATE chunk)
        speed_ntsc  = 16639; // 60.09Hz
        speed_pal   = 19997; // 50.00Hz
        speed_dendy = speed_pal;
    }
    else
    {
        info = true;
        data = true;
    }

    while (true)
    {
        if ((size-chunk_offset) == 0) break; // end of file without NEND, acceptable.

        if ((size-chunk_offset) < 8) // not enough data for chunk size + FourCC
        {
          nsfe_error = "Incomplete chunk header at end of file? Less than 8 bytes remain.";
          return false;
        }

        UINT8* chunk = image + chunk_offset;

        unsigned int chunk_size =
            (chunk[0]      )
          + (chunk[1] <<  8)
          + (chunk[2] << 16)
          + (chunk[3] << 24);

        if ((size-chunk_offset) < (chunk_size+8)) // not enough data for chunk
        {
          nsfe_error = "Incomplete NSFe chunk at end of file? Not enough data.";
          return false;
        }

        char cid[5];
        memcpy (cid, chunk+4, 4);
        cid[4] = 0;

        chunk_offset += 8;
        chunk += 8;

        if (!strcmp(cid, "NEND")) // end of chunks
        {
          break;
        }

        if (!strcmp(cid, "INFO"))
        {
          if (info == true)
          {
            nsfe_error = "Duplicate 'INFO' chunk.";
            return false;
          }

          if (chunk_size < 0x0A)
          {
            nsfe_error = "'INFO' chunk too small.";
            return false;
          }

          version = 1;
          load_address = chunk[0x00] | (chunk[0x01] << 8);
          init_address = chunk[0x02] | (chunk[0x03] << 8);
          play_address = chunk[0x04] | (chunk[0x05] << 8);
          pal_ntsc     = chunk[0x06];
          soundchip    = chunk[0x07];
          songs        = chunk[0x08];
          start        = chunk[0x09] + 1; // note NSFe is 0 based, unlike NSF
          total_songs  = songs;

          regn      = CONVERT_REGN[     pal_ntsc & 3];
          regn_pref = CONVERT_REGN_PREF[pal_ntsc & 3];

          // other variables contained in other banks
          memset (bankswitch, 0, 8);
          memset (extra, 0, 4);

          // setup derived variables
          use_vrc6 = soundchip &  1 ? true : false;
          use_vrc7 = soundchip &  2 ? true : false;
          use_fds  = soundchip &  4 ? true : false;
          use_mmc5 = soundchip &  8 ? true : false;
          use_n106 = soundchip & 16 ? true : false;
          use_fme7 = soundchip & 32 ? true : false;
          song = start - 1;

          // body should follow in 'DATA' chunk
          delete[] body;
          body = NULL;
          bodysize = 0;

          // description strings should follow in 'auth' chunk
          title_nsf[0]     = 0;
          artist_nsf[0]    = 0;
          copyright_nsf[0] = 0;
          title     = title_nsf;
          artist    = artist_nsf;
          copyright = copyright_nsf;
          ripper    = "";
          text      = NULL;
          text_len  = 0; // NSFe only

          // INFO chunk read
          info = true;
        }
        else if (!strcmp(cid, "DATA"))
        {
          if (!info)
          {
            nsfe_error = "'DATA' chunk appears before 'INFO' chunk.";
            return false;
          }
          if (data)
          {
            nsfe_error = "Duplicate 'DATA' chunk found.";
            return false;
          }

          delete[]body;
          body = new  UINT8[chunk_size];
          memcpy (body, chunk, chunk_size);
          bodysize = chunk_size;

          // DATA chunk read
          data = true;
        }
        else if (!strcmp(cid, "BANK"))
        {
          for (unsigned int i=0; i < 8 && i < chunk_size; ++i)
          {
            bankswitch[i] = chunk[i];
          }
        }
        else if (!strcmp(cid, "RATE"))
        {
          if (chunk_size >= 2) speed_ntsc  = chunk[0] | (chunk[1] << 8);
          if (chunk_size >= 4) speed_pal   = chunk[2] | (chunk[3] << 8);
          if (chunk_size >= 6) speed_dendy = chunk[4] | (chunk[5] << 8);
          if (chunk_size <  6) speed_dendy = speed_pal;
        }
        else if (!strcmp(cid, "NSF2"))
        {
          version = 2;
          if (chunk_size < 1)
          {
              nsfe_error = "'NSF2' chunk size too small.";
              return false;
          }
          nsf2_bits = chunk[0];
        }
        else if (!strcmp(cid, "VRC7"))
        {
          if (chunk_size < 1)
          {
              nsfe_error = "'VRC7' chunk size too small.";
              return false;
          }
          else if (chunk_size > 1 && chunk_size < (1+16*8))
          {
              nsfe_error = "'VRC7' chunk patch set incomplete.";
              return false;
          }
          vrc7_type = chunk[0];
          if (vrc7_type > 1)
          {
              nsfe_error = "'VRC7' variant unknown.";
              return false;
          }
          if (chunk_size >= (1+16*8))
          {
              vrc7_patches = chunk+1;
          }
        }
        else if (!strcmp(cid, "auth"))
        {
          unsigned int n=0;
          while (true)
          {
            NSFE_STRING(title);
            NSFE_STRING(artist);
            NSFE_STRING(copyright);
            NSFE_STRING(ripper);
            break;
          }
        }
        else if (!strcmp(cid, "plst"))
        {
          nsfe_plst = chunk;
          nsfe_plst_size = chunk_size;
        }
        else if (!strcmp(cid, "time"))
        {
          unsigned int i = 0;
          unsigned int n = 0;
          while (i < NSFE_ENTRIES && (n+3)<chunk_size)
          {
            UINT32 value =
                (chunk[n+0]      )
              + (chunk[n+1] <<  8)
              + (chunk[n+2] << 16)
              + (chunk[n+3] << 24);
            nsfe_entry[i].time = INT32(value);
            ++i;
            n += 4;
          }
        }
        else if (!strcmp(cid, "fade"))
        {
          unsigned int i = 0;
          unsigned int n = 0;
          while (i < NSFE_ENTRIES && (n+3)<chunk_size)
          {
            UINT32 value =
                (chunk[n+0]      )
              + (chunk[n+1] <<  8)
              + (chunk[n+2] << 16)
              + (chunk[n+3] << 24);
            nsfe_entry[i].fade = INT32(value);
            ++i;
            n += 4;
          }
        }
        else if (!strcmp(cid, "tlbl"))
        {
          unsigned int n=0;
          for (unsigned int i=0; i < NSFE_ENTRIES; ++i)
          {
            NSFE_STRING(nsfe_entry[i].tlbl);
          }
        }
        else if (!strcmp(cid, "taut"))
        {
          unsigned int n=0;
          for (unsigned int i=0; i < NSFE_ENTRIES; ++i)
          {
            NSFE_STRING(nsfe_entry[i].taut);
          }
        }
        else if (!strcmp(cid, "psfx"))
        {
          for (unsigned int i=0; i<chunk_size; ++i)
          {
            UINT8 track = chunk[i];
            nsfe_entry[track].psfx = true;
          }
        }
        else if (!strcmp(cid, "text"))
        {
          text = reinterpret_cast<char*>(chunk);
          text_len = chunk_size;
        }
        else if (!strcmp(cid, "mixe"))
        {
          unsigned int pos = 0;
          while (pos + 3 <= chunk_size)
          {
            unsigned int mixe_index = chunk[pos+0];
            INT16 mixe_value = UINT16(chunk[pos+1] + (chunk[pos+2] << 8));

            if (mixe_index >= NSFE_MIXES)
            {
                nsfe_error = "'MIXE' device index out of range.";
                return false; // invalid mixe index
            }

            // max value should never be used, but just in case, fake it with max-1
            // (was using max value to specify "default" instead)
            if (mixe_value == NSFE_MIXE_DEFAULT) mixe_value = NSFE_MIXE_DEFAULT-1;

            nsfe_mixe[mixe_index] = mixe_value;
            pos += 3;
          }
        }
        else if (!strcmp(cid, "regn"))
        {
            if (chunk_size >= 1) regn = chunk[0];
            if (chunk_size >= 2) regn_pref = chunk[1];
        }
        else // unknown chunk
        {
          if (cid[0] >= 'A' && cid[0] <= 'Z')
          {
            snprintf(nsfe_error_,NSFE_ERROR_SIZE,"Unknown mandatory NSFe chunk: '%s'",cid);
            nsfe_error_[NSFE_ERROR_SIZE-1] = 0;
            nsfe_error = nsfe_error_;
            return false;
          }
        }

        // next chunk
        chunk_offset += chunk_size;
    }

    nsfe_error = "";
    return true;
  }

  const char* NSF::LoadError()
  {
    return nsf_error;
  }

  void NSF::DebugOut ()
  {
    int i;

    DEBUG_OUT ("Magic:    %4s\n", magic);
    DEBUG_OUT ("Version:  %d\n", version);
    DEBUG_OUT ("Songs:    %d\n", songs);
    DEBUG_OUT ("Load:     %04x\n", load_address);
    DEBUG_OUT ("Init:     %04x\n", init_address);
    DEBUG_OUT ("Play:     %04x\n", play_address);
    DEBUG_OUT ("Title:    %s\n", title);
    DEBUG_OUT ("Artist:   %s\n", artist);
    DEBUG_OUT ("Copyright:%s\n", copyright);
    DEBUG_OUT ("Speed(N): %d\n", speed_ntsc);
    DEBUG_OUT ("Speed(P): %d\n", speed_pal);
    DEBUG_OUT ("Speed(D): %d\n", speed_dendy);

    DEBUG_OUT ("Bank :");
    for (i = 0; i < 8; i++)
    {
      DEBUG_OUT ("[%02x]", bankswitch[i]);
    }
    DEBUG_OUT ("\n");

    if (regn & 1) DEBUG_OUT ("PAL\n");
    if (regn & 2) DEBUG_OUT ("NTSC\n");
    if (regn & 4) DEBUG_OUT ("Dendy\n");
    if (regn == 0) DEBUG_OUT ("No region?\n");

    if (soundchip & 1)
      DEBUG_OUT ("VRC6 ");
    if (soundchip & 2)
      DEBUG_OUT ("VRC7 ");
    if (soundchip & 4)
      DEBUG_OUT ("FDS ");
    if (soundchip & 8)
      DEBUG_OUT ("MMC5 ");
    if (soundchip & 16)
      DEBUG_OUT ("Namco 106 ");
    if (soundchip & 32)
      DEBUG_OUT ("FME-07 ");

    DEBUG_OUT ("\n");

    DEBUG_OUT ("Extra:     ");
    for (i = 0; i < 4; i++)
    {
      DEBUG_OUT ("[%02x]", extra[i]);
    }
    DEBUG_OUT ("\n");
    DEBUG_OUT ("DataSize: %d\n", bodysize);
  }

}                               // namespace
