#pragma once
#include <map>
#include "afxwin.h"
#include "resource.h"
#include "NSFDialog.h"
#include "PresetManager.h"

// NSFPresetDialog ダイアログ

class NSFPresetDialog : public CDialog, public NSFDialog
{
	DECLARE_DYNAMIC(NSFPresetDialog)
private:
    PresetManager pman;

public:
	NSFPresetDialog(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~NSFPresetDialog();
  void Reset(CString path);
  void Open();

// ダイアログ データ
	enum { IDD = IDD_PRESET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  afx_msg void OnCbnSelchangeCombo();
  CListBox m_listCtrl;
  afx_msg void OnBnClickedDelete();
  afx_msg void OnBnClickedSave();
  afx_msg void OnBnClickedLoad();
  afx_msg void OnBnClickedAdd();
  CString m_editValue;
  afx_msg void OnLbnDblclkList();
};
