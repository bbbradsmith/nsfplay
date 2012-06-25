/*
 *  The mini paser and lexer for the NEZplug playlist.
 *
 *  filename::type(nsf,kss,gbr,hes),song,title,time(h:m:s),loop(h:m:s/h:m:s-),fade(h:m:s) 
 */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "ppls.h"
#include "sstream.h"
#define BLK_SIZE (32)

static int is_sjis_prefix(int c)
{
  if((0x81<=c&&c<=0x9F)||(0xE0<=c&&c<=0xFC)) return 1 ;
  else return 0 ;
}

static void skip_space(SST *sst)
{ 
  int c ;

  while(1)
  {
    c = SST_getc(sst) ;
    if(c!=' '&&c!='\t') break ;
  } ;

  SST_ungetc(sst,c) ;
}

static char *get_filename(SST *sst)
{
  SST *token ;
  int c ;

  skip_space(sst) ;
  token = SST_new() ;

  while((c=SST_getc(sst))!=SST_EOF)
  {
    if(c==':')
    {
      c = SST_getc(sst) ;
      if(c==':') break ;
      c = SST_ungetc(sst,c) ;
      c = ':' ;
    }
    SST_putc(token, c) ;
    if(is_sjis_prefix(c)) SST_putc(token, SST_getc(sst)) ;
  }
  SST_ungetc(sst,c) ;

  return SST_sublimate(token) ;
}

static char *get_type(SST *sst)
{
  SST *token ;
  int c ;

  skip_space(sst) ;
  token = SST_new() ;

  while((c=SST_getc(sst))!=','&&c!=SST_EOF&&c!=' '&&c!='\t')
  {
    SST_putc(token, c) ;
    if(is_sjis_prefix(c)) SST_putc(token, SST_getc(sst)) ;
  }
  SST_ungetc(sst,c) ;

  return SST_sublimate(token) ;
}

static char *get_title(SST *sst)
{
  SST *token ;
  int c ;

  skip_space(sst) ;
  token = SST_new() ;

  while((c=SST_getc(sst))!=','&&c!=SST_EOF)
  {
    if(c=='\\') c=SST_getc(sst) ;
    SST_putc(token, c) ;
    if(is_sjis_prefix(c)) SST_putc(token, SST_getc(sst)) ;
  }
  SST_ungetc(sst,c) ;

  return SST_sublimate(token) ;
}

static int str2type(const char *str)
{
  if(!strcmp(str,"KSS")) return 1 ;
  if(!strcmp(str,"kss")) return 1 ;
  if(!strcmp(str,"MSX")) return 2 ;
  if(!strcmp(str,"msx")) return 2 ;
  if(!strcmp(str,"NSF")) return 3 ;
  if(!strcmp(str,"nsf")) return 3 ;
  return 0 ;
}

static int is_digit(int c)
{
  if('0'<=c&&c<='9') return 1 ; else return 0 ;
}

static int get_number(SST *sst, int default_value)
{
  int c ;
  int ret = 0 ;
  int raw = 0 ;

  skip_space(sst) ;
  c = SST_getc(sst) ;
  if(c == SST_EOF) return 0 ;

  if(c =='$')
  {
    while(1)
    {
      c = SST_getc(sst) ;
      if(is_digit(c)) 
        ret = (ret<<4) + c - '0' ;
      else if('a'<=tolower(c)&&tolower(c)<='f') 
        ret = (ret<<4) + tolower(c) - 'a' + 10 ;
      else
        break ;
      raw++ ;
    }
  }
  else
  {
    SST_ungetc(sst,c) ;
    while(1)
    {
      c = SST_getc(sst) ;
      if(is_digit(c))
        ret = ret*10 + c - '0' ;
      else
        break ;
      raw++ ;
    }
  }

  SST_ungetc(sst,c) ;

  if(raw==0) ret = default_value ;
  return ret ;
}

static int check_section(SST *sst, int scode)
{
  int c ;

  skip_space(sst) ;
  if((c = SST_getc(sst))==scode) return 1 ;
  SST_ungetc(sst,c) ;

  return 0 ;
}

static int get_time(SST *sst, int default_value)
{
  int time = 0 ;
  int c ;

  skip_space(sst) ;
  c = SST_getc(sst) ;
  SST_ungetc(sst,c) ;
  if(!is_digit(c)) return default_value ;

  time = get_number(sst,0) ;
  if(check_section(sst,':'))
  {
    time = time * 60 + get_number(sst,0) ;
    if(check_section(sst,':'))
      time = time * 60 + get_number(sst,0) ;
  }

  time *= 1000 ;

  if(check_section(sst,'\'')) time += get_number(sst,0) * 10 ;

  return time ;
}

int PPLS_get_time(char *text, int d)
{
  int ret ;
  SST *sst = SST_new() ;

  if(!sst) return d ;
  SST_set_text(sst, text) ;
  ret = get_time(sst,d) ; 
  SST_delete(sst) ;
  return ret ;
}

static int MAX(int val, int max)
{
  if(max<val) return max ; else return val ;
}

void PLSITEM_adjust(PLSITEM *item, int play_time,int fade_time, int loop_num, int vol[4])
{
  int i ;

  if(item->time_in_ms<0)
  {
    item->time_in_ms = play_time ;
    if(item->loop_in_ms<0) item->loop_in_ms = 0 ;
  }
  else if(item->loop_in_ms<0) item->loop_in_ms = item->time_in_ms ;

  if(item->loop_num<0) item->loop_num = loop_num ;
  if(item->fade_in_ms<0) item->fade_in_ms = fade_time ;
  for(i=0;i<4;i++)
    if(!item->enable_vol[i]) item->vol[i] = vol[i] ;

  return ;
}

PLSITEM *PLSITEM_new(const char *text)
{
  PLSITEM *elem ;
  SST *sst ;
  int i, abb_value ;
  char *p ;

  sst = SST_new() ;
  if(!sst) return NULL ;
  SST_set_text(sst, text) ;

  if((elem = (PLSITEM *)malloc(sizeof(PLSITEM)))==NULL) return NULL ;
  memset(elem,0,sizeof(PLSITEM)) ;

  elem->time_in_ms = -1 ;
  elem->fade_in_ms = -1 ;
  elem->loop_in_ms = -1 ;
  elem->loop_num = -1 ;
  for(i=0;i<4;i++) elem->enable_vol[i] = 0 ;

  elem->filename = get_filename(sst) ;
  if(!check_section(sst,':')) goto exit ;

  p = get_type(sst) ;
  elem->type = str2type(p) ;
  free(p) ;
  if(!check_section(sst,',')) goto exit ;

  if(elem->type == 0) goto exit ;
  else if(elem->type == 1) abb_value = -1 ; /* 0 : old NEZplug behavior */ 
  else abb_value = -1 ;

  elem->song = get_number(sst,0) ;
  if(!check_section(sst,',')) goto exit ;
  if(elem->type == 3) elem->song--;

  elem->title = get_title(sst) ;
  if(!check_section(sst,',')) goto exit ;

  elem->time_in_ms = get_time(sst,-1) ;
  if((elem->time_in_ms == -1)&&(elem->type==1)) goto exit ;
  if(!check_section(sst,',')) goto exit ;

  elem->loop_in_ms = get_time(sst,-1) ;
  if(check_section(sst,'-'))
    elem->loop_in_ms = elem->time_in_ms - elem->loop_in_ms ;
  if(!check_section(sst,',')) goto exit ;

  elem->fade_in_ms = get_time(sst,abb_value) ;
  if(!check_section(sst,',')) goto exit ;

  elem->loop_num = get_number(sst,-1) ;
  for(i=0;i<4;i++)
  {
    if(!check_section(sst,',')) goto exit ;
    if(check_section(sst,'-'))
      elem->vol[i] = -MAX(get_number(sst, 128), 128) ;
    else
      elem->vol[i] = MAX(get_number(sst, 128), 128) ;

    if(elem->vol[i]!=128)
      elem->enable_vol[i] = 1 ; 
    else
    {
      elem->enable_vol[i] = 0 ;
      elem->vol[i] = 0 ;
    }
  }

  if(check_section(sst,'?'))
  {
    elem->extra_code = 1 ;
    elem->opll_ch_pan = get_number(sst,0x7EDE79E);
  }

exit:
  SST_delete(sst) ;
  if(elem->filename==NULL)
  {
    elem->filename = (char *)malloc(strlen(text)+1) ;
    if(elem->filename==NULL)
    {
      PLSITEM_delete(elem) ;
      return NULL ;
    }
    strcpy(elem->filename, text) ;
  }

  return elem ;
}

void PLSITEM_delete(PLSITEM *elem)
{
  free(elem->filename) ;
  free(elem->title) ;
  free(elem->memo) ;
  free(elem) ;
}

static char *print_time(int time)
{
  static char buf[32] ;
  int h,m,s,ss = 0 ;

  if(time<0) return "" ;

  ss = (time%1000) / 10 ;
  time /= 1000 ;
  s = time%60 ;
  time /= 60 ;
  m = time%60 ;
  time /= 60 ;
  h = time ;
  
  if(h) sprintf(buf,"%02d:%02d:%02d",h,m,s) ;
  else sprintf(buf,"%02d:%02d",m,s) ;

  return buf ;
}

static void splitpath(char *fn, char *path, char *file)
{
  int i ;
  char *p ; 
  
  p = strrchr(fn, '\\') ;
  p++ ;

  for(i=0;p[i]!='\0';i++) file[i] = p[i] ;
  file[i] = '\0' ;

  for(i=0;(fn+i)<p;i++) path[i]=fn[i] ;
  path[i] = '\0' ;

  return ;
}

static void relative_path(char *src_path, char *dst_path, char *rel_path)
{
  int i = 0 , s , d , r = 0 , max ;

  if(src_path[strlen(src_path)-1]!='\\') return ;
  if(dst_path[strlen(dst_path)-1]!='\\') return ;

  while(src_path[i]!='\0'&&dst_path[i]!='\0')
  {
    if(src_path[i]!=dst_path[i]) break ;
    i++ ;
  }
  
  s = d = i ;

  while(s>0&&src_path[s-1]!='\\') s-- ;
  if(s==0)
  {
    strcpy(rel_path, dst_path) ;
    return ;
  }
  while(d>0&&dst_path[d-1]!='\\') d-- ;

  max = 256 - strlen(dst_path+d) ; 

  while(src_path[s]!='\0')
  {
    if(src_path[s]=='\\')
    {
      if((max-4)<r) break ;
      rel_path[r++] = '.' ;
      rel_path[r++] = '.' ;
      rel_path[r++] = '\\' ;
    }
    s++ ;
  }
  rel_path[r]='\0' ;

  strcat(rel_path, dst_path+d) ;
}

char *PLSITEM_print(PLSITEM *elem, char *buf, char *plsfile)
{
  int i = 0 , length ;
  char *p = buf ;
  char file_path[256], pls_path[256], file_name[256], pls_name[256], rel_path[256] ;

  if(plsfile)
  {
    splitpath(plsfile, pls_path, pls_name) ;
    splitpath(elem->filename, file_path, file_name) ;
    relative_path(pls_path, file_path, rel_path) ;
    p += sprintf(p,"%s%s::MSX", rel_path, file_name) ;
  }
  else
  {
    p += sprintf(p,"%s::MSX", elem->filename) ;
  }

  p += sprintf(p,",%d", elem->song) ;
  
  *(p++) = ',' ;
  if(elem->title)
  {
    length = strlen(elem->title) ;
    for(i=0;i<length;i++)
    {
      if(strchr("\\,",elem->title[i])) *(p++) = '\\' ;
      (*p++) = elem->title[i] ;
    }
  }

  *(p++) = ',' ;
  if(elem->time_in_ms>=0)
    p += sprintf(p,"%s", print_time(elem->time_in_ms)) ;
  
  *(p++) = ',' ;
  if(elem->loop_in_ms>=0)
    p += sprintf(p,"%s", print_time(elem->loop_in_ms)) ;

  *(p++) = ',' ;
  if(elem->fade_in_ms>=0)
    p += sprintf(p,"%s", print_time(elem->fade_in_ms)) ;
  
  *(p++) = ',' ;
  if(elem->loop_num>=0)
    p += sprintf(p,"%d", elem->loop_num) ;

  for(i=0;i<4;i++)
  {
    *(p++) = ',' ;
    if(elem->enable_vol[i]) 
      p += sprintf(p,"%d", elem->vol[i]) ;
  }

  if(elem->extra_code) *(p++) = '?' ;

  *p = '\0' ;

  return buf ;
}

void PLSITEM_set_title(PLSITEM *elem, const char *title)
{
  if(title==NULL)
  {
    free(elem->title) ;
    elem->title = NULL ;
    return ;
  }
  elem->title = (char *)realloc(elem->title,strlen(title)+1) ;
  if(elem->title==NULL) return ;
  strcpy(elem->title,title) ;
}

