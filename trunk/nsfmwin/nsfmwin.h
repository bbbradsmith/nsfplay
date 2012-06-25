// nsfmwin.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// メイン シンボル


// CnsfmwinApp:
// このクラスの実装については、nsfmwin.cpp を参照してください。
//

class CnsfmwinApp : public CWinApp
{
public:
	CnsfmwinApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CnsfmwinApp theApp;
