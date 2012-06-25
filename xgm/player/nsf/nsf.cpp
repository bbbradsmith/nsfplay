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
  }

  NSF::~NSF ()
  {
    delete[]body;
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
      format = "%A - %T (%n/%e)";

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
    else if (strstr (fn, ".nsf") || strstr (fn, ".NSF"))
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
    return time_in_ms < 0 ? default_playtime : time_in_ms;
  }

  int NSF::GetLoopTime ()
  {
    return loop_in_ms < 0 ? 0 : loop_in_ms;
  }

  int NSF::GetFadeTime ()
  {
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

  void NSF::SetSong (int s)
  {
    song = s % songs;
  }

  bool NSF::Load (UINT8 * image, UINT32 size)
  {
    if (size < 0x80)
      return false;

    memcpy (magic, image, 5);
    magic[4] = '\0';
    if (strcmp ("NESM", magic))
      return false;

    version = image[0x05];
    songs = image[0x06];
    start = image[0x07];
    load_address = image[0x08] | (image[0x09] << 8);
    init_address = image[0x0a] | (image[0x0B] << 8);
    play_address = image[0x0c] | (image[0x0D] << 8);
    memcpy (title, image + 0x0e, 32);
    title[31] = '\0';
    memcpy (artist, image + 0x2e, 32);
    artist[31] = '\0';
    memcpy (copyright, image + 0x4e, 32);
    artist[31] = '\0';
    speed_ntsc = image[0x6e] | (image[0x6f] << 8);
    memcpy (bankswitch, image + 0x70, 8);
    speed_pal = image[0x78] | (image[0x79] << 8);
    pal_ntsc = image[0x7a];

    if(speed_pal==0)
      speed_pal = 0x4e20;
    if(speed_ntsc==0)
      speed_ntsc = 0x411A;

    soundchip = image[0x7b];

    use_vrc6 = soundchip & 1 ? true : false;
    use_vrc7 = soundchip & 2 ? true : false;
    use_fds = soundchip & 4 ? true : false;
    use_mmc5 = soundchip & 8 ? true : false;
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
