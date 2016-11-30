#ifndef _NSF_H_
#define _NSF_H_
#include "../soundData.h"

#define NSF_MAX_PATH 512

namespace xgm
{
  struct NSFE_Entry
  {
    char* tlbl;
    INT32 time;
    INT32 fade;
  };
  const unsigned int NSFE_ENTRIES = 256;

  /**
   * NSFt@C^
   */
  class NSF : public SoundDataMSP
  {
  public:
    char magic[5];
    UINT8 version;
    UINT8 songs;
    UINT8 start;
    UINT16 load_address;
    UINT16 init_address;
    UINT16 play_address;
    char filename[NSF_MAX_PATH];
    char print_title[256+64]; // margin 64 chars.
    char title_nsf[32];
    char artist_nsf[32];
    char copyright_nsf[32];
    char* title;
    char* artist;
    char* copyright;
    char* ripper; // NSFe only
    char* text; // NSFe only
    UINT32 text_len; // NSFe only
    UINT16 speed_ntsc;
    UINT8 bankswitch[8];
    UINT16 speed_pal;
    UINT8 pal_ntsc;
    UINT8 soundchip;
    bool use_vrc7;
    bool use_vrc6;
    bool use_fds;
    bool use_fme7;
    bool use_mmc5;
    bool use_n106;
    UINT8 extra[4];
    UINT8 *body;
    int bodysize;
    UINT8* nsfe_image;
    UINT8* nsfe_plst;
    int nsfe_plst_size;
    NSFE_Entry nsfe_entry[NSFE_ENTRIES];

    /** »ÝIðÌÈÔ */
    int song;
    /** vCXg©çÇñ¾f[^Èçtrue */
    bool playlist_mode;
    /** tÔ */
    int time_in_ms, default_playtime;
    /** [vÔ */
    int loop_in_ms;
    /** tF[hÔ */
    int fade_in_ms, default_fadetime;
    /** [vñ */
    int loop_num, default_loopnum;
    /** tÔªs¾ÌtrueiftHgÌtÔðgpj*/
    bool playtime_unknown;
    bool title_unknown;

      NSF ();
     ~NSF ();
    /**
     * NSFt@CÌÇÝÝ
     *
     * @param image oCiC[WÖÌ|C^
     * @param size  C[WÌTCY
     * @return ¬÷ true ¸s false
     */

    // loads file (playlist or NSF or NSFe)
    bool LoadFile (const char *fn);

    // loads NSF (or NSFe via LoadNSFe)
    bool Load (UINT8 * image, UINT32 size);

    // loads NSFe, if info is false INFO chunk is required to initialize
    bool LoadNSFe(UINT8* image, UINT32 size, bool info);

    void DebugOut ();
    /**
     * ^Cg¶ñÌæ¾
     *
     * @return ^Cg¶ñ (ìÈÒ - ^Cg)
     */
    char *GetTitleString (const char *format = nullptr, int song=-1);
    char *GetPlaylistString (const char *format, bool b);
    int GetLength ();
    void SetTitleString (char *);
    void SetDefaults (int playtime, int fadetime, int loopnum);
    void SetLength (int time_in_ms);
    void SetSong (int);
    int GetSong ();
    int GetSongNum();

    int GetPlayTime ();
    int GetLoopTime ();
    int GetFadeTime ();
    int GetLoopNum ();

    bool UseNSFePlaytime();
  };

}                               // namespace 
#endif
