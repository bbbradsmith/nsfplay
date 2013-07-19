// nsf_setup.cpp : DLL の初期化ルーチンです。
//

#include "stdafx.h"
#include "nsf_setup.h"

#include "../xgm/version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//	メモ!
//
//		この DLL が MFC DLL に対して動的にリンクされる場合、
//		MFC 内で呼び出されるこの DLL からエクスポートされた
//		どの関数も関数の最初に追加される AFX_MANAGE_STATE 
//		マクロを含んでいなければなりません。
//
//		例:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 通常関数の本体はこの位置にあります
//		}
//
//		このマクロが各関数に含まれていること、MFC 内の
//		どの呼び出しより優先することは非常に重要です。
//		これは関数内の最初のステートメントでなければな
//		らないことを意味します、コンストラクタが MFC 
//		DLL 内への呼び出しを行う可能性があるので、オブ
//		ジェクト変数の宣言よりも前でなければなりません。
//
//		詳細については MFC テクニカル ノート 33 および
//		58 を参照してください。
//

// Cnsf_setupApp

BEGIN_MESSAGE_MAP(Cnsf_setupApp, CWinApp)
END_MESSAGE_MAP()


// Cnsf_setupApp コンストラクション

Cnsf_setupApp::Cnsf_setupApp()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の Cnsf_setupApp オブジェクトです。

Cnsf_setupApp theApp;


// Cnsf_setupApp 初期化
static NSFDialogManager *pDlg = NULL;

static HANDLE hDbgfile;

BOOL Cnsf_setupApp::InitInstance()
{
	CWinApp::InitInstance();
#if defined(_MSC_VER)
#ifdef _DEBUG
   hDbgfile = CreateFile("C:\\in_nsf_ui.log", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) ;
  _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ) ;
  _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE ) ;
  _CrtSetReportFile( _CRT_WARN, (HANDLE)hDbgfile ) ;
#endif
#endif
	return TRUE;
}

int Cnsf_setupApp::ExitInstance()
{
  return CWinApp::ExitInstance();
}

extern "C" __declspec( dllexport ) NSFplug_UI *CreateNSFplug_UI(NSFplug_Model *cf, int mode)
{
  return new NSFDialogManager(cf,mode);
}

extern "C" __declspec( dllexport ) void DeleteNSFplug_UI(NSFplug_UI *p)
{
  delete p;
}

extern "C" __declspec( dllexport ) const char* VersionNSFplug_UI()
{
  return NSFPLAY_VERSION;
}
