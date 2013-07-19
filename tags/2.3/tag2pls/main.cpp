#include "../xgm/xgm.h"
#include "../xgm/utils/nsf_tag.h"

using namespace xgm;

static const char *help_message = 
"Usage: tag2pls [-f \"format\"] FILENAME.NSF\n\n"\
"  The file TAGINFO.TAG or FILENAME.TAG must be located in the same\n"\
"  folder together with the FILENAME.NSF.\n\n"\
"  Some control characters can be accepted in the format string \n"\
"  and they are replaced with the following data. \n\n"\
"    %T  The title of the song.\n"\
"    %A  The artist name of the song.\n"\
"    %C  The copyright text of the song.\n"\
"    %n  The current number of the song.\n"\
"    %e  The number of songs in the NSF.\n"\
"    %s  The start number of the songs.\n"\
"    %N  The current number of the song in the hex format.\n"\
"    %E  The number of songs in the NSF in the hex format.\n"\
"    %S  The start number of the songs in the hex format.\n"\
"\n"\
"  The default format string is \"%A - %T (%n/%e)\"\n"\
"  The title string from taginfo.tag will be ignored\n"\
"  if the option -f is used.\n\n"\
;

void print_help()
{
  fprintf(stderr,"%s",help_message);
}

int main( int argc, char *argv[] )
{
  NSF nsf;
  NSF_TAG nsf_tag;
  char *format=NULL;
  char *input=NULL;

  if(argc==1)
  {
    print_help();
    return 0;
  }

  for(int i=1;i<argc;i++)
  {
    if(argv[i][0]=='-')
    {
      switch(argv[i][1])
      {
      case 'f':
      case 'F':
        if(i+1==argc||format!=NULL)
        {
          print_help();
          return 1;
        }
        format = argv[++i];
        break;

      default:
        print_help();
        return 1;
        break;
      }
    }
    else if(input==NULL)
    {
      input = argv[i];
    }
    else
    {
      print_help();
      return 1;
    }
  }

  if( input == NULL )
  {
    print_help();
    return 1;
  }

  if(nsf.LoadFile( input )==false)
  {
    fprintf(stderr,"Can't open the nsf file:%s\n", input);
    return 1;
  }

  nsf_tag.SetNSF(&nsf);
  nsf.playlist_mode = false;

  printf( "[playlist]\n" );
  for(int i=nsf.start-1;i<nsf.songs;i++)
  {  
    nsf.SetSong( i );
    nsf_tag.ReadTagItem( i );
    nsf.title_unknown = format?true:false;
    printf("File%d=%s\n", i+1, nsf.GetPlaylistString( format, true ));
  }
  printf( "NumberOfEntries=%d\n", nsf.songs );
  printf( "Version=2\n" );

  return 0;
}

