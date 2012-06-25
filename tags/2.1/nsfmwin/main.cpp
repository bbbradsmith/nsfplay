#include <windows.h>
#include "utils/nsf_tag.h"
#include "nsfmeasure.h"

using namespace xgm;

static bool GetLength(xgm::NSF *nsf, int max_time)
{
  NSFMeas nsfm;

  if(nsfm.Open(nsf,max_time))
  {
    while(nsfm.Progress())
    {
      printf("\b\b\b\b\b\b\b\b\b\b\b\b[");
      int i,j=10*nsfm.GetProgressTime()/max_time;
      for(i=0;i<j;i++)
        putchar('#');
      for(i=j;i<10;i++)
        putchar(' ');
      putchar(']');
      fflush(stdout);
    }
    return nsfm.IsDetected();
  }
  return false;
}

static bool NSF2TAG(char *fn, int max_time)
{
  NSF nsf;
  NSF_TAG ntag(&nsf);

  nsf.LoadFile(fn);
  ntag.Sync();
  ntag.InitTag("%T(%n/%e)");

  for(int i=nsf.start-1;i<nsf.songs;i++)
  {
    nsf.SetSong(i);
    printf("\b\b\b\b\b\b\b\b\\b\bb\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b[%03d/%03d][##########]",nsf.song+1,nsf.songs);
    fflush(stdout);
    GetLength(&nsf, max_time);
    ntag.UpdateTagItem(i,"%T(%n/%e)",2);
  }

  return true;
}

static bool NSF2PLS(char *filename, int max_time)
{
  int i;
  FILE *fp;
  char path_buffer[_MAX_PATH];
  char drive[_MAX_DRIVE];
  char dir[_MAX_DIR];
  char fname[_MAX_FNAME];
  char ext[_MAX_EXT];
  NSF nsf;

  if(!nsf.LoadFile(filename))
  {
    MessageBox(NULL,"File Not Found", filename, MB_OK);
    return false;
  }

  _splitpath( nsf.filename, drive, dir, fname, ext );
  _makepath( path_buffer, drive, dir, fname, "pls" );

  fp = fopen(path_buffer,"r");
  if(fp)
  {
    fclose(fp);
    if(IDNO==MessageBox(NULL,"Playlist file already exists. Overwrite?", path_buffer, MB_YESNO|MB_ICONWARNING))
      return false;
  }

  fp = fopen(path_buffer, "w");
  if(fp==NULL)
  {
    MessageBox(NULL,"Playlist Write Error!", "Error", MB_OK|MB_ICONEXCLAMATION );
    return false;
  }

  fprintf(fp, "[playlist]\n");

  for(i=nsf.song-1;i<nsf.songs;i++)
  {
    if(max_time)
    {
      nsf.SetSong(i);
      if(GetLength(&nsf, max_time))
        fprintf(fp, "File%d=%s\n", i+1, nsf.GetPlaylistString(NULL,true));
      else
        fprintf(fp, "File%d=%s\n", i+1, nsf.GetPlaylistString(NULL,false));
    }
    else
    {
      nsf.SetSong(i);
      fprintf(fp, "File%d=%s\n", i+1, nsf.GetPlaylistString(NULL,false));
    }
  }

  fprintf(fp, "NumberOfEntries=%d\n",i);
  fprintf(fp, "Version=2\n");
  fclose(fp);

  return true;
}


void print_help()
{
  printf("Usage: nsfm.exe [-t max_time] [-p] filename.nsf\n");
}

int main(int argc, char *argv[])
{
  int max_time = 60 * 5 * 1000;
  int i,f=1;
  bool pls = false;

  if(argc<2)
  {
    print_help();
    exit(1);
  }

  for(i=1;i<argc;i++)
  {
    if(argv[i][0]=='-')
    {
      if(i+1==argc)
      {
        print_help();
        exit(1);
      }

      switch(argv[i][1])
      {
      case 'p':
        pls = true;
        break;

      case 't':
        max_time = atoi(argv[++i]) * 1000;
        break;

      default:
        print_help();
        exit(1);
        break;
      }
    }
    else
    {
      f = i;
    }
  }

  if(pls)
    NSF2PLS(argv[f], max_time);
  else
    NSF2TAG(argv[f], max_time);

  return 0;
}





