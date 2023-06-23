#include <shlobj.h>
#include <shlwapi.h>
#include "in_nsf.h"
#include "direct.h"

using namespace xgm;

static bool firstinit = false;
static In_Module mod = {0};

/** プラグイン本体 */
static WA2NSF *pPlugin;
static HINSTANCE hPlugin;

static NSFplug_UI_DLL *ui;
static NSFplug_Model npm = {0};

static InYansfDirect direct = {
	NSFPLUG_TITLE,
	&ui,
	&npm,
};

static char DllPath[1024];
static char IniPath[1024+32];
const char* INI_FILENAME = "in_yansf.ini";
const char* ini_override = NULL;

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
    ini_override = NULL;
    firstinit = false;
    break;

  case DLL_PROCESS_DETACH:
    if (!firstinit) break;
    if (!npm.no_save_config)
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

static void FirstInit()
{
    firstinit = true;

    // DLLパスの取得
    GetModuleFileName(hPlugin, DllPath, 1024);
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
    if (ini_override)
    {
      GetFullPathName(ini_override,sizeof(IniPath),IniPath,NULL);
      if (FALSE == PathFileExists(IniPath)) // try to create if needed
      {
        HANDLE f = CreateFile(IniPath,GENERIC_READ,FILE_SHARE_READ,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
        if (f != INVALID_HANDLE_VALUE) CloseHandle(f);
      }
    }
    else // use default or try to create it
    {
      strcpy(IniPath,DllPath);
      strcat(IniPath,INI_FILENAME);
      if (FALSE == PathFileExists(IniPath)) // if an INI file does not exists next to the plugin already...
      {
        // first try to see if we have permission to create a new one in that location...
        HANDLE f = CreateFile(IniPath,GENERIC_READ,FILE_SHARE_READ,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
        if (f != INVALID_HANDLE_VALUE) {
          CloseHandle(f);
        }
        else  // Otherwise, attempt to use AppData instead.
        {
          if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, IniPath))) {
            strcat(IniPath,"\\NSFPlay");
            CreateDirectory(IniPath,NULL);
            strcat(IniPath,"\\");
            strcat(IniPath,INI_FILENAME);
          }
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
    char path[1024+16];
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
static void GetFileInfo(const char *file, char *title, int *length_in_ms)
{ 
  pPlugin->GetFileInfo(file, title, length_in_ms);
}
static int InfoBox(const char *fn, HWND hParent)
{
  return pPlugin->InfoBox(fn, hParent);
}
static int IsOurFile(const char *fn)
{
  return pPlugin->IsOurFile(fn);
}
static int Play(const char *fn)
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
  if (!firstinit) FirstInit();
  mod.version = IN_VER;
  mod.is_seekable = 1;
  mod.UsesOutputPlug = 1;
  mod.description = NSFPLUG_TITLE;
  mod.FileExtensions = "nsf\0NES Sound Format\0nsfe\0NES Sound Format Extended\0\0";
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

extern "C" __declspec( dllexport ) void * pluginDirect() // direct access, bypassing winamp plugin interface
{
  if (!firstinit) FirstInit();
  return &direct;
}

extern "C" __declspec( dllexport ) void iniOverride(const char* ini) // call before winampGetInModule2/pluginDirect to redirect the INI file
{
  ini_override = ini;
}
