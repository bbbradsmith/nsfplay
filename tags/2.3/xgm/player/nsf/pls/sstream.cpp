/*
 * String Stream 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_MSC_VER)
#include <crtdbg.h>
#if defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC 
#endif
#endif

#include "sstream.h"

SST *SST_new(void)
{
  SST *sst ;

  if(!(sst = (SST *)malloc(sizeof(SST)))) return NULL ;
  sst->index = 0 ;
  sst->length = 0 ;
  sst->size = 0 ;
  sst->str = 0 ;

  return sst ;
}

void SST_delete(SST *sst)
{
  if(sst)
  {
    free(sst->str) ;
    free(sst) ;
  }
}

int SST_set_text(SST *sst, const char *str)
{
  if(sst->str) return -1 ;

  sst->size = strlen(str) + 1 ;
  if(!(sst->str = (unsigned char *)malloc(sst->size))) return -1 ;
  strcpy((char *)sst->str,str) ;
  sst->index = 0 ;
  sst->length = sst->size - 1 ;

  return 0 ;
}

/* Delete SST object but leave the pointer to text. */
char *SST_sublimate(SST *sst)
{
  char *ret = (char *)sst->str ;

  sst->index = 0 ;
  sst->length = 0 ;
  sst->size = 0 ;
  sst->str = NULL ;
  free(sst) ;

  return ret ;
}

int SST_seekoff(SST *sst, long offset)
{
  long pos ;
  
  if(sst)
  {
    pos = sst->index + offset ;
    if(pos<0||sst->size<pos) return -1 ;
    sst->index = pos ;
    return 0 ;
  }
  else return -1 ;
}

int SST_seekpos(SST *sst, long pos)
{
  if(sst)
  {
    if(pos<0||sst->size<pos) return -1 ;
    sst->index = pos ;
    return 0 ;
  }
  else return -1 ;
}

long SST_tell(SST *sst){ return sst?sst->index:-1 ; }

int SST_putc(SST *sst, int c)
{
  int i ;

  if(sst)
  {
    if( (sst->size-1) <= sst->index )
    {
      if(!(sst->str=(unsigned char *)realloc(sst->str,sst->size+SST_BUF_GSIZE))) return -1 ;
      for(i=0;i<SST_BUF_GSIZE;i++) sst->str[sst->size+i] = '\0' ;
      sst->size += SST_BUF_GSIZE ;
    }
    sst->str[sst->index++] = c&0xff ;
    if(sst->length<sst->index) sst->length = sst->index ;
    return c ;
  }
  else return -1 ;
}

int SST_length(SST *sst)
{
  if(sst) return sst->length ; else return -1 ;
}

int SST_ungetc(SST *sst, int c)
{
  if(c == SST_EOF)
    return -1 ;
  else if(sst->index>0)
  {
    sst->str[--sst->index] = c&0xff ;
    return c ;
  }
  else return -1 ;
}

int SST_getc(SST *sst)
{
  int c ;

  if(sst)
  {
    if(sst->size <= sst->index) return SST_EOF ;
    c = sst->str[sst->index++]  ;
    return c ;
  }
  else return -1 ;
}

char *SST_getn(SST *sst)
{
  return sst?(char *)sst->str:NULL ;
}


