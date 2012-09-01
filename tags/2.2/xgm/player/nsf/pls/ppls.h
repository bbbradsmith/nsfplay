#ifndef _PPLS_H_
#define _PPLS_H_

extern "C" {

typedef struct tagPLSITEM
{
  int type ;
  char *filename ; 
  char *title ;
  char *memo ;
  int song ;

  long time_in_ms ;
  long loop_in_ms ;
  long fade_in_ms ;

  int loop_num ;

  int enable_vol[4] ;
  int vol[4] ;

  int extra_code ;
  int opll_ch_pan ;

} PLSITEM ;

PLSITEM *PLSITEM_new(const char *text) ;
void PLSITEM_delete(PLSITEM *elem) ;
char *PLSITEM_print(PLSITEM *elem, char *buf, char *plsfile) ;
void PLSITEM_adjust(PLSITEM *elem, int play_time, int fade_time, int loop_num, int vol[4]) ;
void PLSITEM_set_title(PLSITEM *elem, const char *title) ;

int PPLS_get_time(char *text, int d) ;

#define PLSITEM_PRINT_SIZE (_MAX_PATH*2+1024)

} // extern "C"

#endif
