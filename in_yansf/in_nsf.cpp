#include <shlobj.h>
#include <shlwapi.h>
#include "in_nsf.h"

using namespace xgm;

static In_Module mod;

/** プラグイン本体 */
static WA2NSF *pPlugin;
static HINSTANCE hPlugin;

static NSF *sdat;
static NSFplug_UI_DLL *ui;
static NSFplug_Model npm;

static char DllPath[MAX_PATH];
static char IniPath[MAX_PATH+32];
const char* INI_FILENAME = "in_yansf.ini";

static HANDLE hDbgfile;

BOOL APIENTRY DllMain (HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
#if defined(_MSC_VER)
#ifdef _DEBUG
   hDbgfile = CreateFile("C:\\in_nsf.log", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) ;
  _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ) ;
  _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE ) ;
  _CrtSetReportFile( _CRT_WARN, (HANDLE)hDbgfile ) ;
#endif
#endif
    hPlugin = (HINSTANCE)hModule;

    // DLLパスの取得
    GetModuleFileName(hPlugin, DllPath, MAX_PATH);
    strrchr(DllPath,'\\')[1] = '\0';

    // プレイヤー作成
    npm.pl = new NSFPlayer();
    npm.sdat = new NSF();
    npm.cf = new NSFPlayerConfigIni();

    // Winamp版の設定を拡張
    npm.cf->CreateValue("WRITE_TAGINFO", 0);
    npm.cf->CreateValue("READ_TAGINFO", 0);
    npm.cf->CreateValue("UPDATE_PLAYLIST", 0);
    npm.cf->CreateValue("MASK_INIT", 1);
    npm.cf->CreateValue("INFO_DELAY", 50);
    npm.cf->CreateValue("INFO_FREQ", 30);
    npm.cf->CreateValue("GRAPHIC_MODE", 1);
    npm.cf->CreateValue("FREQ_MODE", 1);
    npm.cf->CreateValue("LAST_PRESET", "Default");
    
    // configuration
    strcpy(IniPath,DllPath);
    strcat(IniPath,INI_FILENAME);
    if (FALSE == PathFileExists(IniPath)) // if an INI file does not exists next to the plugin already...
    {
      // first try to see if we have permission to create a new one in that location...
      HANDLE f = CreateFile(IniPath,GENERIC_READ,FILE_SHARE_READ,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
      if (f != INVALID_HANDLE_VALUE)
      {
        CloseHandle(f);
      }
      else  // Otherwise, attempt to use AppData instead.
      {
        if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, IniPath)))
        {
          strcat(IniPath,"\\NSFPlay");
          CreateDirectory(IniPath,NULL);
          strcat(IniPath,"\\");
          strcat(IniPath,INI_FILENAME);
        }
      }
    }
    npm.cf->Load(IniPath,"NSFplug");

    if((*(npm.cf))["MASK_INIT"]) (*(npm.cf))["MASK"] = 0;
	(*(npm.cf))["MULT_SPEED"] = 256; // reset speed to normal on startup

    // GUI initialized in Init()
    ui = NULL;

    pPlugin = new WA2NSF(npm.pl,npm.cf,npm.sdat);
    pPlugin->SetUserInterface(ui);
    break;

  case DLL_PROCESS_DETACH:
    npm.cf->Save(IniPath,"NSFplug");

    delete pPlugin;
    delete npm.pl;
    delete npm.cf;
    delete npm.sdat;
    //delete ui;
    break;
  }
  return TRUE;
}

static void Config(HWND hParent)
{ 
  pPlugin->Config(hParent); 
}
static void About(HWND hParent)
{ 
  pPlugin->About(hParent); 
}
static void Init()
{ 
  if (ui == NULL)
  {
    char path[MAX_PATH+16];
    strcpy(path,DllPath);
    strcat(path,"nsfplug_ui.dll");
    ui = new NSFplug_UI_DLL(path,&npm,0);
    pPlugin->SetUserInterface(ui);
  }

  pPlugin->Init(); 
}
static void Quit()
{ 
  pPlugin->Quit();

  delete ui;
  ui = NULL;
}
static void GetFileInfo(char *file, char *title, int *length_in_ms)
{ 
  pPlugin->GetFileInfo(file, title, length_in_ms);
}
static int InfoBox(char *fn, HWND hParent)
{
  return pPlugin->InfoBox(fn, hParent);
}
static int IsOurFile(char *fn)
{
  return pPlugin->IsOurFile(fn);
}
static int Play(char *fn)
{
  int ret = pPlugin->Play(fn);
  if(!ret && ui) ui->StartUpdate();
  return ret;
}
static void Pause()
{
  pPlugin->Pause();
}
static void UnPause()
{
  pPlugin->UnPause();
}
static int IsPaused()
{
  return pPlugin->IsPaused();
}
static void Stop()
{
  if (ui) ui->StopUpdate();
  pPlugin->Stop();
}
static int GetLength()
{
  return pPlugin->GetLength();
}
static int GetOutputTime()
{
  return pPlugin->GetOutputTime();
}
static void SetOutputTime(int time_in_ms)
{
  pPlugin->SetOutputTime(time_in_ms);
}
static void SetVolume(int volume)
{
  pPlugin->SetVolume(volume);
}
static void SetPan(int pan)
{
  pPlugin->SetPan(pan);
}
static void EQSet(int on, char data[], int preamp)
{
  pPlugin->EQSet(on,data,preamp);
}

extern "C" __declspec( dllexport ) In_Module *winampGetInModule2()
{
  mod.version = IN_VER;
  mod.is_seekable = 1;
  mod.UsesOutputPlug = 1;
  mod.description = "NSFplug " "(" __DATE__ " " __TIME__ ")";
  mod.FileExtensions = "nsf\0NES Sound Format\0nsfe\0NES Sound Format Extended\0";
  mod.Config = Config;
  mod.About = About;
  mod.Init = Init;
  mod.Quit = Quit;
  mod.GetFileInfo = GetFileInfo;
  mod.InfoBox = InfoBox;
  mod.IsOurFile = IsOurFile;
  mod.Play = Play;
  mod.Pause = Pause;
  mod.UnPause = UnPause;
  mod.IsPaused = IsPaused;
  mod.Stop = Stop;
  mod.GetLength = GetLength;
  mod.GetOutputTime = GetOutputTime;
  mod.SetOutputTime = SetOutputTime;
  mod.SetVolume = SetVolume;
  mod.SetPan = SetPan;
  mod.EQSet = EQSet;
  pPlugin->SetModule(&mod);
  return &mod;
}

extern "C" __declspec( dllexport ) const char * nsfLoadError()
{
  return sdat->LoadError();
}
