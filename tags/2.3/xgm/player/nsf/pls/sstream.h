#ifndef _SSTREAM_H_
#define _SSTREAM_H_

extern "C" {

typedef struct tagSST
{
  long size ;
  long length ;
  long index ;
  unsigned char *str ;
} SST ;

SST *SST_new(void) ;
int SST_set_text(SST *sst, const char *str) ;
char *SST_sublimate(SST *sst) ;
void  SST_delete(SST *sst) ;
int SST_seekoff(SST *sst, long offset) ;
int SST_seekpos(SST *sst, long pos) ;
long SST_tell(SST *sst) ;
int SST_getc(SST *sst) ;
int SST_putc(SST *sst, int c) ;
int SST_ungetc(SST *sst, int c) ;

#define SST_BUF_GSIZE 32
#define SST_EOF (-1)

} // extern "C"

#endif