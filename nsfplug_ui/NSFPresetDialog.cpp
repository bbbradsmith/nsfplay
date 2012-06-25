// NSFPresetDialog.cpp : 実装ファイル
//
#include "stdafx.h"
#include "nsfplug_ui.h"
#include "NSFDialogs.h"
#include "NSFPresetDialog.h"

// NSFPresetDialog ダイアログ

IMPLEMENT_DYNAMIC(NSFPresetDialog, CDialog)
NSFPresetDialog::NSFPresetDialog(CWnd* pParent /*=NULL*/)
	: CDialog(NSFPresetDialog::IDD, pParent)
  , m_editValue(_T(""))
{
}

NSFPresetDialog::~NSFPresetDialog()
{
}

void NSFPresetDialog::Reset(CString path)
{
  pman.Reset(path);
}

void NSFPresetDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST, m_listCtrl);
  DDX_Text(pDX, IDC_EDIT, m_editValue);
	DDV_MaxChars(pDX, m_editValue, 128);
}

void NSFPresetDialog::Open()
{
  ShowWindow(SW_SHOW);
}

BEGIN_MESSAGE_MAP(NSFPresetDialog, CDialog)
  ON_BN_CLICKED(IDC_DELETE, OnBnClickedDelete)
  ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
  ON_BN_CLICKED(IDC_LOAD, OnBnClickedLoad)
  ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
  ON_LBN_DBLCLK(IDC_LIST, OnLbnDblclkList)
END_MESSAGE_MAP()

// NSFPresetDialog メッセージ ハンドラ

BOOL NSFPresetDialog::OnInitDialog()
{
  CDialog::OnInitDialog();
  
  // TODO :  ここに初期化を追加してください
  std::set<CString>::iterator it;
  std::set<CString> presets = pman.GetPresetNames();
  for(it=presets.begin();it!=presets.end();it++)
    m_listCtrl.AddString(*it);

  m_listCtrl.SelectString(-1,CONFIG["LAST_PRESET"]);
    
  return TRUE;  // return TRUE unless you set the focus to a control
  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void NSFPresetDialog::OnBnClickedDelete()
{
  CString name;
  m_listCtrl.GetText(m_listCtrl.GetCurSel(),name);
  m_listCtrl.DeleteString(m_listCtrl.GetCurSel());
  if(!pman.DeletePreset(name))
    MessageBox("Error: Can't Delete.");
  parent->UpdateNSFPlayerConfig(true);
}

void NSFPresetDialog::OnBnClickedSave()
{
  CString name;
  m_listCtrl.GetText( m_listCtrl.GetCurSel(), name );
  if(!pman.SavePreset( pm->cf, name ))
    MessageBox("Error: Can't Save.");
  CONFIG["LAST_PRESET"] = name;
}

void NSFPresetDialog::OnBnClickedLoad()
{
  CString name;
  m_listCtrl.GetText( m_listCtrl.GetCurSel(), name );
  if(!pman.LoadPreset( pm->cf, name ))
    MessageBox("Error: Can't Load.");
  CONFIG["LAST_PRESET"] = name ;
  parent->UpdateNSFPlayerConfig(true);
}

void NSFPresetDialog::OnBnClickedAdd()
{
  UpdateData(TRUE);

  if( m_editValue!="" )
  {
    if(m_listCtrl.FindString(-1,m_editValue)!=LB_ERR)
    {
      MessageBox("Error: Can't add the same preset.");
      return;
    }
    m_listCtrl.AddString( m_editValue );
    if(!pman.CreatePreset( pm->cf, m_editValue ))
      MessageBox("Error: Can't Create.");
    else
    {
      CONFIG["LAST_PRESET"] = m_editValue;
      m_editValue = "";
      UpdateData(FALSE);
      parent->UpdateNSFPlayerConfig(true);
    }
  }
}

void NSFPresetDialog::OnLbnDblclkList()
{
  OnBnClickedLoad();
}
