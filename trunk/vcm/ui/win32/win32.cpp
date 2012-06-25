// win32.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "win32.h"
#include "vcmDlg.h"
#include "vcmTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace vcm;

// Cwin32App

BEGIN_MESSAGE_MAP(Cwin32App, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

// Cwin32App コンストラクション

Cwin32App::Cwin32App() 
: m_cg( "普通の設定", "テスト" ), 
  m_sub1( "良い設定", "ttt"),
  m_sub2( "悪い設定", "t"),
  m_sub3( "噂の設定", "")
{

  // コンフィグを作る
  m_config.CreateValue("test", 8 );
  m_config.CreateValue("test2", true );
  m_config.CreateValue("test3", "文字列" );
  m_config.CreateValue("test4", true );
  m_config.CreateValue("test5", 16 );
  m_config.CreateValue("test6", 32 );
  m_config.CreateValue("test7", (16*3600+17*60+18)*1000 );

  // カテゴリーを作る
  m_cg.Insert( "test5", new VT_SPIN   ( "数値ボックス(&I)", "テスト用数値ボックス", 0, 16 ) );
  m_cg.Insert( "test",  new VT_SPIN   ( "数値ボックス2(&J)", "テスト用数値ボックス", 0, 256 ) );
  m_sub1.Insert( "test2", new VT_CHECK( "チェックボックス(&B)", "テスト用チェックボックス" ) );
  m_sub1.Insert( "test3", new VT_TEXT ( "文字列ボックス(&S)",  "テスト用文字列ボックス", 16 ) );
  m_cg.Insert( "test4", new VT_CHECK  ( "チェックボックスその2(&C)", "テスト用チェックボックス" ) );
  m_cg.Insert( "test5", new VT_SLIDER ( "スライダー(&L)", "テスト用スライダー", 0, 32, "最小", "最大", 4, 4 ) );


  std::vector<std::string> items;
  items.push_back("項目1");
  items.push_back("項目2");
  items.push_back("長い項目3");
  items.push_back("文字列");

  std::map<std::string, int> map;
  map["項目1"]=0;
  map["項目2"]=1;
  map["長い項目3"]=2;
  map["文字列"]=16;

  m_cg.Insert( "test3", new VT_COMBO ( "コンボボックス(&X)", "こんぼっす", items ) );
  m_sub2.Insert( "test",  new VT_COMBO_INT( "内部値が整数のコンボ","コンボなり", items, map ) );

  class TestConv : public ValueConv
  {
    virtual bool GetExportValue( ValueCtrl *vt, Configuration &cfg, const std::string &id, const Value &value, Value &result )
    {
      if( (int)cfg["test5"] < 16 )
        return false;
      result = value;
      return true;
    }
  };

  m_cg.Insert( "test6", new VT_SLIDER ( "スライダーその2(&M)", "テスト用スライダー", 0, 32, "MIN ", "MAX ", 4, 8 ), new TestConv() ); 
  m_sub2.Insert( "test7", new VT_TEXT ( "時間ボックス(&T)", "時刻表示", 8 ), new VC_TIME() );
  m_cg.Insert( "test",  new VT_TEXT ( "文字列ボックス",  "テスト用文字列ボックス", 16 ) );

}

// 唯一の Cwin32App オブジェクトです。

Cwin32App theApp;


// Cwin32App 初期化

BOOL Cwin32App::InitInstance()
{
	// アプリケーション　マニフェストが　visual スタイルを有効にするために、
	// ComCtl32.dll バージョン 6　以降の使用を指定する場合は、
	// Windows XP に　InitCommonControls() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	InitCommonControls();

	CWinApp::InitInstance();

	CvcmTree dlg;
	m_pMainWnd = &dlg;

  m_cg.AddSubGroup(&m_sub1);
  m_sub2.AddSubGroup(&m_sub3);
  dlg.AttachConfig(m_config);
  dlg.AttachGroup(m_cg);
  dlg.AttachGroup(m_sub2);
	
  INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ダイアログが <OK> で消された時のコードを
		//       記述してください。
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ダイアログが <キャンセル> で消された時のコードを
		//       記述してください。
	}

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	// アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}
