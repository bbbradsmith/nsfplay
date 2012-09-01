// win32.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// メイン シンボル
#include "../../vcm.h"

// Cwin32App:
// このクラスの実装については、win32.cpp を参照してください。
//

class Cwin32App : public CWinApp
{
public:
	Cwin32App();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装
public:
  vcm::Configuration m_config;
  vcm::ConfigGroup m_cg, m_sub1, m_sub2, m_sub3;

	DECLARE_MESSAGE_MAP()
};

extern Cwin32App theApp;
