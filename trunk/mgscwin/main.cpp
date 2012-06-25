#ifdef _MSC_VER
#pragma warning (disable:4786)
#endif
#include <iostream>
#include <fstream>
#include "mgscwin.h"

using namespace xgm;

static MGSC mgsc;

void help()
{
  std::cerr << "Usage: mgscwin MMLFILENAME [MGSFILENAME] [-T]" << std::endl;
}


int main(int argc, char *argv[])
{
  std::fstream tmp, mml, mgs;
  std::string mml_name, mgs_name, mgsc_name;
  int i, outc = 0, option = 0;

#ifndef EMBED_MGSC

  mgsc_name = std::string(argv[0]);
  if(mgsc_name.rfind('\\')==mgsc_name.npos)
    mgsc_name = "MGSC.COM";
  else
    mgsc_name = mgsc_name.substr(0,mgsc_name.rfind('\\')) + "\\MGSC.COM";
  tmp.open(mgsc_name.c_str(), std::ios::in|std::ios::binary);
  if(tmp.fail())
  {
    std::cerr << "Can't load : " << mgsc_name ;
    return 1;
  }
  mgsc.LoadCompiler(&tmp);
  tmp.close();

#endif

  if(argc==1)
  {
    help();
    return 1;
  }

  mml_name = std::string(argv[1]);

  // オプション解析
  for(i=2;i<argc;i++)
  {
    if(argv[i][0]=='-')
    {
      switch(argv[i][1])
      {
      case 'T':
      case 't':
        option |= MGSC::OPT_TRACK_STATUS;
        break;
      default:
        help();
        break;
      }
    }
    else if(outc==0)
      outc= i;
    else
    {
      help();
      return 1;
    }
  }

  if(outc)
  {
    mgs_name = std::string(argv[outc]);
  }
  else
  {
    mgs_name = mml_name.substr(0,mml_name.rfind('.')) + ".MGS";
  }

  // 必ずbinaryモードで開く
  mml.open(mml_name.c_str(), std::ios::in | std::ios::binary); 
  if(mml.fail())
  {
    std::cerr << "Can't open " << mml_name << std::endl;
    return 1;
  }

  mgs.open(mgs_name.c_str(), std::ios::out | std::ios::in | std::ios::binary | std::ios::trunc);
  if(mgs.fail())
  {
    std::cerr << "Can't create " << mgs_name << std::endl;
    return 1;
  }

  if(!mgsc.Compile(&mml,&mgs,option))
  {
    std::cerr << mgsc.GetErrorMessage() << std::endl;
    return 1;
  }

  mgs.close();
  mml.close();

  return 0;
}

