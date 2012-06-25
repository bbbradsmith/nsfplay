// NSFMemoryWriteDialog.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "NSFMemoryWriteDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NSFMemoryWriteDialog ダイアログ


NSFMemoryWriteDialog::NSFMemoryWriteDialog(CWnd* pParent /*=NULL*/)
	: CDialog(NSFMemoryWriteDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(NSFMemoryWriteDialog)
	m_address = _T("");
	m_value = _T("");
	//}}AFX_DATA_INIT
}


void NSFMemoryWriteDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NSFMemoryWriteDialog)
	DDX_Control(pDX, IDC_WLIST, m_wlist);
	DDX_Text(pDX, IDC_ADDRESS, m_address);
	DDX_Text(pDX, IDC_VALUE, m_value);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(NSFMemoryWriteDialog, CDialog)
	//{{AFX_MSG_MAP(NSFMemoryWriteDialog)
	ON_BN_CLICKED(IDC_INSERT, OnInsert)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_SEND, OnSend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NSFMemoryWriteDialog メッセージ ハンドラ

static int hex2int(CString s)
{
  int i, c, ret = 0;

  for(i=0;i<s.GetLength();i++)
  {
    ret <<= 4;
    c = s.GetAt(i);
    if('0'<=c&&c<='9')
      ret += c - '0';
    else if('a'<=c&&c<='f')
      ret += c - 'a' + 0xa;
    else if('A'<=c&&c<='F')
      ret += c - 'A' + 0xa;
  }

  return ret;
}

void NSFMemoryWriteDialog::OnInsert()
{
  UpdateData();
  int ip = m_wlist.GetCaretIndex() + 1;
  if(ip >= m_wlist.GetCount()) ip = -1;
  ip = m_wlist.InsertString(ip, m_address + "," + m_value);
  m_wlist.SetItemData(ip, (DWORD)((hex2int(m_address)<<8)+(hex2int(m_value))));
  m_wlist.SetCaretIndex(ip);
}

void NSFMemoryWriteDialog::OnDelete()
{
  int snum = m_wlist.GetSelCount();
  int i;

  m_wlist.GetSelItems(65536,sidx);
  if(snum>65536) snum = 65536;
  for(i=0;i<snum;i++)
    m_wlist.DeleteString(sidx[i]);
}

void NSFMemoryWriteDialog::OnSend()
{
  int i, adr, val;
  int snum = m_wlist.GetSelCount();

  if(snum==0)
  {
    m_wlist.SelItemRange(TRUE, 0, m_wlist.GetCount()-1);
    snum = m_wlist.GetSelCount();
  }

  m_wlist.GetSelItems(65536,sidx);
  if(snum>65536) snum = 65536;

  for(i=0;i<snum;i++)
  {
    adr = (int)(m_wlist.GetItemData(sidx[i])>>8);
    val = (int)(m_wlist.GetItemData(sidx[i])&0xff);
    pl->bus.Write(adr,val);
  }
}

