// nsf_setup.h : nsf_setup.DLL のメイン ヘッダー ファイル

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// メイン シンボル
#include "NSFDialogs.h"

// Cnsf_setupApp
// このクラスの実装に関しては nsf_setup.cpp を参照してください。
//

class Cnsf_setupApp : public CWinApp
{
protected:
public:
	Cnsf_setupApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
  virtual int ExitInstance();
};

