/*
 * NSFplug for KbMediaPlayer on XGM Framework
 */
#ifdef _DEBUG
#include <crtdbg.h>
#endif
#include "debugout.h"
#include "in_nsf.h"

using namespace xgm;

static char DllPath[MAX_PATH];

HINSTANCE ghPlugin;
std::set<KMP_NSF *>kmp_set;
CRITICAL_SECTION cso;

static NSFplug_UI_DLL *ui;
static NSFplug_Model npm;

static void WINAPI Init()
{ 
  InitializeCriticalSection(&cso);
}

static void WINAPI Deinit()
{
}

static HKMP WINAPI Open(const char *cszFileName, SOUNDINFO *pInfo)
{
  KMP_NSF *kmp = new KMP_NSF(new xgm::NSFPlayer(), npm.cf, new xgm::NSF());

  if(pInfo&&kmp&&kmp->Open(cszFileName, pInfo))
  {
    EnterCriticalSection(&cso);
    if(ui) ui->StopUpdate();
    kmp_set.insert(kmp);
    kmp_update_menu(kmp);
    if(ui) ui->SetInfoData(kmp->sdat);
    npm.pl = kmp->pl;
    npm.sdat = kmp->sdat;
    if(ui) ui->StartUpdate();
    LeaveCriticalSection(&cso);
    return (HKMP)kmp;
  }
  else
  {
    delete kmp->pl;
    delete kmp->sdat;
    delete kmp;
    return NULL;
  }
}

static void WINAPI Close(HKMP kmp)
{
  if(kmp)
  {
    EnterCriticalSection(&cso);
    if(ui) ui->StopUpdate();
    ((KMP_NSF *)kmp)->Close();
    kmp_set.erase((KMP_NSF *)kmp);
    npm.pl = NULL;
    npm.sdat = NULL;
    delete ((KMP_NSF *)kmp)->pl;
    delete ((KMP_NSF *)kmp)->sdat;
    delete (KMP_NSF *)kmp;
    if(kmp_set.size()==1)
    {
      kmp_update_menu(*kmp_set.begin());
      if(ui) ui->SetInfoData((*kmp_set.begin())->sdat);
      npm.pl = (*kmp_set.begin())->pl;
      npm.sdat = (*kmp_set.begin())->sdat;
      if(ui) ui->StartUpdate();
    }
    LeaveCriticalSection(&cso);
  }
}

static DWORD WINAPI Render(HKMP kmp, BYTE* Buffer, DWORD dwSize)
{
  if(kmp) 
    return ((KMP_NSF *)kmp)->Render(Buffer, dwSize);
  else
    return 0;
}

static DWORD WINAPI SetPosition(HKMP kmp, DWORD dwPos)
{
  if(kmp) 
    return ((KMP_NSF *)kmp)->SetPosition(dwPos);
  else
    return 0;
}

static HANDLE hDbgfile;

BOOL APIENTRY DllMain (HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
  char path[MAX_PATH+16];
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
    ghPlugin = (HINSTANCE)hModule;

    // DLLパスの取得
    GetModuleFileName(ghPlugin, DllPath, MAX_PATH);
    strrchr(DllPath,'\\')[1] = '\0';

    // コンフィグレーション作成
    npm.cf = new xgm::NSFPlayerConfig();

    // KbMediaPlayerにおける拡張項目を作成
    npm.cf->CreateValue("WRITE_TAGINFO", 0);
    npm.cf->CreateValue("READ_TAGINFO", 0);
    npm.cf->CreateValue("MASK_INIT", 1);
    npm.cf->CreateValue("UPDATE_PLAYLIST", 1);
    npm.cf->CreateValue("LAST_PRESET", "Default");
    npm.cf->CreateValue("INI_FILE", "");

    // コンフィグレーション読み込み
    strcpy(path,DllPath);
    strcat(path,"in_nsf.ini");
    npm.cf->Load(path,"NSFplug");
    (*(npm.cf))["INI_FILE"] = path;

    // GUIのロード
    strcpy(path,DllPath);
    strcat(path,"nsfplug_ui.dll");
    ui = new NSFplug_UI_DLL(path,&npm,1);
    if(ui&&ui->GetModule()) kmp_hack(ui);
    break;

  case DLL_PROCESS_DETACH:
    // コンフィグレーション破棄
    if(ui&&ui->GetModule()) kmp_unhack();
    strcpy(path,DllPath);
    strcat(path,"in_nsf.ini");
    npm.cf->Save(path,"NSFplug");
    delete npm.cf;
    delete ui;
    break;
  }
  return TRUE;
}

static KMPMODULE mod;
static const char *exts[] = { ".nsf", NULL };

KMPMODULE * KMP_GETMODULE()
{
  // KMP_GETMODULEはKbMediaPlayerから何度も呼ばれる
  memset(&mod, 0, sizeof(KMPMODULE));
  mod.dwVersion = KMPMODULE_VERSION;
  mod.dwPluginVersion = 0x100;
  mod.pszCopyright = "2002 [OK], Mamiya and Kobarin";
  mod.pszDescription = "NSFplug for KbMediaPlayer";
  mod.ppszSupportExts = exts;
  mod.dwReentrant = 1;
  mod.Init = Init;
  mod.Deinit = Deinit;
  mod.Open = Open;
  mod.Close = Close;
  mod.Render = Render;
  mod.SetPosition = SetPosition;
  return &mod;
}