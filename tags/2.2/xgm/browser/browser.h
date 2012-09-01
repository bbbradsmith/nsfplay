#ifndef _BROWSER_H_
#define _BROWSER_H_


namespace xgm 
{
  class Browser
  {
  private:
    int bufmax;
    int infoidx;
    BrowserInfo *infobuf;

  public:
    void Browser(int bufmax=256)
    {

    }

    BrowserInfo &GetInfo()
    {
      return infobuf[infoidx];
    }

    
  };

} // namespace


#endif