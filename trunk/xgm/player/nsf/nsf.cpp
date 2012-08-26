#include <windows.h>
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

  static char *print_time (int time)
  {
    static char buf[32];
    int h, m, s, ss = 0;

    if (time < 0)
      return "";

    ss = (time % 1000) / 10;
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

  char *NSF::GetPlaylistString (const char *format, bool b)
  {
    static char buf[NSF_MAX_PATH + 128];
    char *p = buf;

    char *t = GetTitleString (format);

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


  char *NSF::GetTitleString (const char *format, int song)
  {
    int wp=0;

    char fpath[sizeof(this->filename)];
    memcpy(fpath,this->filename,sizeof(this->filename));
    char *fname = strrchr(fpath,'\\');
    if(fname!=NULL) *(fname++) = '\0'; else fname = fpath;

    if(song<0) song = this->song;

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
          wp += sprintf(print_title+wp, "%s", nsfe_entry[song].tlbl);
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

    pls = PLSITEM_new (fn);
    if (!pls)
      goto Error_Exit;

    if (pls->type == 3)
      strncpy (filename, pls->filename, NSF_MAX_PATH);
    else if (
         strstr (fn, ".nsf")
      || strstr (fn, ".NSF")
      || strstr (fn, ".nsfe")
      || strstr (fn, ".NSFE")
      )
      strncpy (filename, fn, NSF_MAX_PATH);
    else
      goto Error_Exit;

    filename[NSF_MAX_PATH] = '\0';
    fp = fopen (filename, "rb");
    if (fp == NULL)
      goto Error_Exit;

    fseek (fp, 0L, SEEK_END);
    size = ftell (fp);
    fseek (fp, 0L, SEEK_SET);
    buf = new UINT8[size];
    rsize = fread (buf, 1, size, fp);

    if (size != rsize)
      goto Error_Exit;
    if (Load (buf, size) == false)
      goto Error_Exit;

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

    return true;

  Error_Exit:
    if (pls)
      PLSITEM_delete (pls);
    if (buf)
      delete[]buf;
    if (fp)
      fclose (fp);
    return false;
  }

  void NSF::SetLength (int t)
  {
    time_in_ms = t;
    playtime_unknown = false;
  }

  int NSF::GetPlayTime ()
  {
    if (nsfe_entry[song].time >= 0)
    {
      return nsfe_entry[song].time;
    }

    return time_in_ms < 0 ? default_playtime : time_in_ms;
  }

  int NSF::GetLoopTime ()
  {
    return loop_in_ms < 0 ? 0 : loop_in_ms;
  }

  int NSF::GetFadeTime ()
  {
    if (nsfe_entry[song].fade >= 0)
    {
      return nsfe_entry[song].fade;
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
    if (size < 4) // no FourCC
      return false;

    // fill NSFe values with defaults

    // 'plst'
    nsfe_plst = NULL;
    nsfe_plst_size = 0;

    // entries 'tlbl', 'time', 'fade'
    for (int i=0; i < NSFE_ENTRIES; ++i)
    {
      nsfe_entry[i].tlbl = "";
      nsfe_entry[i].time = -1;
      nsfe_entry[i].fade = -1;
    }

    // load the NSF or NSFe

    memcpy (magic, image, 4);
    magic[4] = '\0';

    if (strcmp ("NESM", magic))
    {
      return LoadNSFe(image, size, false);
    }

    if (size < 0x80) // no header?
      return false;

    version = image[0x05];
    songs = image[0x06];
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
    speed_ntsc = image[0x6e] | (image[0x6f] << 8);
    memcpy (bankswitch, image + 0x70, 8);
    speed_pal = image[0x78] | (image[0x79] << 8);
    pal_ntsc = image[0x7a];

    if(speed_pal==0)
      speed_pal = 0x4e20;
    if(speed_ntsc==0)
      speed_ntsc = 0x411A;

    soundchip = image[0x7b];

    use_vrc6 = soundchip &  1 ? true : false;
    use_vrc7 = soundchip &  2 ? true : false;
    use_fds  = soundchip &  4 ? true : false;
    use_mmc5 = soundchip &  8 ? true : false;
    use_n106 = soundchip & 16 ? true : false;
    use_fme7 = soundchip & 32 ? true : false;

    memcpy (extra, image + 0x7c, 4);

    if (body)
      delete[]body;
    body = new UINT8[size - 0x80];
    memcpy (body, image + 0x80, size - 0x80);
    bodysize = size - 0x80;

    song = start - 1;

    return true;
  }

  bool NSF::LoadNSFe (UINT8 * image, UINT32 size, bool info)
  {
    // store entire file for string references, etc.
    if (nsfe_image)
      delete[] nsfe_image;
    nsfe_image = new UINT8[size+1];
    ::memcpy(nsfe_image, image, size);
    nsfe_image[size] = 0; // null terminator for safety
    image = nsfe_image;

    if (size < 4) // no FourCC
      return false;

    memcpy (magic, image, 4);
    magic[4] = '\0';

    if (strcmp ("NSFE", magic))
    {
      return false;
    }

    UINT32 chunk_offset = 4; // skip 'NSFE'
    while (true)
    {
        if ((size-chunk_offset) < 8) // not enough data for chunk size + FourCC
          return false;

        UINT8* chunk = image + chunk_offset;

        unsigned int chunk_size =
            (chunk[0]      )
          + (chunk[1] <<  8)
          + (chunk[2] << 16)
          + (chunk[3] << 24);

        if ((size-chunk_offset) < (chunk_size+8)) // not enough data for chunk
          return false;

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
          if (chunk_size < 0x0A)
            return false;

          version = 1;
          load_address = chunk[0x00] | (chunk[0x01] << 8);
          init_address = chunk[0x02] | (chunk[0x03] << 8);
          play_address = chunk[0x04] | (chunk[0x05] << 8);
          pal_ntsc     = chunk[0x06];
          soundchip    = chunk[0x07];
          songs        = chunk[0x08];
          start        = chunk[0x09] + 1; // note NSFe is 0 based, unlike NSF

          // NSFe doesn't allow custom speeds
          speed_ntsc = 0x4100; // 60.09Hz
          speed_pal  = 0x4E1D; // 50.00Hz

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
          if (body)
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

          // INFO chunk read
          info = true;
        }
        else if (!strcmp(cid, "DATA"))
        {
          if (!info)
            return false;

          if (body)
            delete[]body;
          body = new  UINT8[chunk_size];
          memcpy (body, chunk, chunk_size);
          bodysize = chunk_size;
        }
        else if (!strcmp(cid, "BANK"))
        {
          if (!info)
            return false;

          for (unsigned int i=0; i < 8 && i < chunk_size; ++i)
          {
            bankswitch[i] = chunk[i];
          }
        }
        else if (!strcmp(cid, "auth"))
        {
          #define NSFE_STRING(p) \
            if (n >= chunk_size) break; \
            p = reinterpret_cast<char*>(chunk+n); \
            while (n < chunk_size && chunk[n] != 0) ++n; \
            if(chunk[n] == 0) ++n;

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
          #define NSFE_STRING(p) \
            if (n >= chunk_size) break; \
            p = reinterpret_cast<char*>(chunk+n); \
            while (n < chunk_size && chunk[n] != 0) ++n; \
            if(chunk[n] == 0) ++n;

          unsigned int n=0;
          for (unsigned int i=0; i < NSFE_ENTRIES; ++i)
          {
            NSFE_STRING(nsfe_entry[i].tlbl);
          }
        }
        else if (!strcmp(cid, "text"))
        {
          text = reinterpret_cast<char*>(chunk);
        }
        else // unknown chunk
        {
          if (cid[0] >= 'A' && cid[0] <= 'Z')
          {
            return false;
          }
        }

        // next chunk
        chunk_offset += chunk_size;
    }

    return true;
  }

  void NSF::DebugOut ()
  {
    int i;
    char buf[256] = "";

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

    DEBUG_OUT ("Bank :");
    for (i = 0; i < 8; i++)
    {
      DEBUG_OUT ("[%02x]", bankswitch[i]);
    }
    DEBUG_OUT ("\n");

    if (pal_ntsc & 1)
      DEBUG_OUT ("PAL mode.\n");
    else
      DEBUG_OUT ("NTSC mode.\n");
    if (pal_ntsc & 2)
      DEBUG_OUT ("Dual PAL and NTSC mode.\n");

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
      DEBUG_OUT (buf, "[%02x]", extra[i]);
      DEBUG_OUT (buf);
    }
    DEBUG_OUT ("\n");
    DEBUG_OUT ("DataSize: %d\n", bodysize);
  }

}                               // namespace
