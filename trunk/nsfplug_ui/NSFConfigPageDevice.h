#if !defined(AFX_NSFCONFIGPAGEDEVICE_H__85114CA7_F58B_4317_A254_A70516EDB80C__INCLUDED_)
#define AFX_NSFCONFIGPAGEDEVICE_H__85114CA7_F58B_4317_A254_A70516EDB80C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NSFConfigPageDevice.h : ヘッダー ファイル
//
#include "NSFDialog.h"
#include "NSFAudioPanel.h"

/////////////////////////////////////////////////////////////////////////////
// NSFConfigPageDevice ダイアログ

class NSFConfigPageDevice : public CPropertyPage, public NSFDialog
{
	DECLARE_DYNCREATE(NSFConfigPageDevice)

protected:

// コンストラクション
public:
	NSFConfigPageDevice();
  ~NSFConfigPageDevice();

// ダイアログ データ
	//{{AFX_DATA(NSFConfigPageDevice)
	enum { IDD = IDD_DEVICE };
		// メモ: ClassWizard はこの位置にデータ メンバを追加します。
	//}}AFX_DATA

// ユーザー追加
public:
  int device_id;
  NSFAudioPanel *audioPanel;
  NSFDialog *mainPanel;
  int mainPanel_id;
  void SetPanel(NSFDialog *p, int id);
  void SetDeviceID(int id);
  void UpdateNSFPlayerConfig(bool b);
  void SetDialogManager(NSFDialogManager *p);

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(NSFConfigPageDevice)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(NSFConfigPageDevice)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_NSFCONFIGPAGEDEVICE_H__85114CA7_F58B_4317_A254_A70516EDB80C__INCLUDED_)
