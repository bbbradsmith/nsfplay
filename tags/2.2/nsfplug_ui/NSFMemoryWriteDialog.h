#if !defined(AFX_NSFMEMORYWRITEDIALOG_H__D05FEE33_E3CB_4A3F_97CC_78F81BF87316__INCLUDED_)
#define AFX_NSFMEMORYWRITEDIALOG_H__D05FEE33_E3CB_4A3F_97CC_78F81BF87316__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NSFMemoryWriteDialog.h : ヘッダー ファイル
//
#include "NSFDialog.h"
/////////////////////////////////////////////////////////////////////////////
// NSFMemoryWriteDialog ダイアログ

class NSFMemoryWriteDialog : public CDialog, public NSFDialog
{
// コンストラクション
public:
	NSFMemoryWriteDialog(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(NSFMemoryWriteDialog)
	enum { IDD = IDD_MEMWRITE };
	CListBox	m_wlist;
	CString	m_address;
	CString	m_value;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(NSFMemoryWriteDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
  int sidx[65536];

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(NSFMemoryWriteDialog)
	afx_msg void OnInsert();
	afx_msg void OnDelete();
	afx_msg void OnSend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_NSFMEMORYWRITEDIALOG_H__D05FEE33_E3CB_4A3F_97CC_78F81BF87316__INCLUDED_)
