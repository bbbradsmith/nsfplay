// NSFMemoryDialog.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "NSFDialogs.h"
#include "NSFMemoryDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NSFMemoryDialog ダイアログ


NSFMemoryDialog::NSFMemoryDialog(CWnd* pParent /*=NULL*/)
	: CDialog(NSFMemoryDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(NSFMemoryDialog)
	//}}AFX_DATA_INIT
  
  dlgFont.CreateFont(14,0,0,0,0,0,0,0,ANSI_CHARSET,0,0,0,FIXED_PITCH,"Courier New");
}

NSFMemoryDialog::~NSFMemoryDialog()
{
}

void NSFMemoryDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NSFMemoryDialog)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(NSFMemoryDialog, CDialog)
	//{{AFX_MSG_MAP(NSFMemoryDialog)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_COMMAND(ID_JAPANESE, OnJapanese)
	ON_COMMAND(ID_ENGLISH, OnEnglish)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NSFMemoryDialog メッセージ ハンドラ

void NSFMemoryDialog::OnTimer(UINT nIDEvent) 
{
  pm->pl->GetMemory(dumpbuf);
  Invalidate(false);
  CDialog::OnTimer(nIDEvent);
}

void NSFMemoryDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
  
  if(bShow)
  {
    SetTimer(1,250,NULL);
  }
  else
  {
    KillTimer(1);
  }
	
}

BOOL NSFMemoryDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
  HICON hIcon = AfxGetApp()->LoadIcon(IDI_MEMORY);
  SetIcon(hIcon, TRUE);
  SetScrollRange(SB_VERT,0,65536/16);
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void NSFMemoryDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
}


void NSFMemoryDialog::OnPaint() 
{
  static char textbuf[256], itoa[]="0123456789ABCDEF";
  int i,j,t;
	CPaintDC dc(this); // 描画用のデバイス コンテキスト
  dc.SelectObject(&dlgFont);
 
  dc.SetTextColor(RGB(255,255,255));
  dc.SetBkColor(RGB(0,0,0));

  RECT rect;
  GetClientRect(&rect);
  int height = rect.bottom - rect.top;
  int pos = GetScrollPos(SB_VERT);

  for(i=0;i<height/9;i++)
  {
    if((i+pos)>=(65536/16))
    {
      dc.FillSolidRect(0, i*14, 6*83-1, (height/9)*14, RGB(0,0,0));
      break;
    }
    t = 0;
    textbuf[t++] = ' ';
    textbuf[t++] = itoa[(((i+pos)*16)>>12)&0xF];
    textbuf[t++] = itoa[(((i+pos)*16)>>8)&0xF];
    textbuf[t++] = itoa[(((i+pos)*16)>>4)&0xF];
    textbuf[t++] = itoa[(((i+pos)*16))&0xF];
    textbuf[t++] = ':';
    for(j=0;j<16;j++)
    {
      textbuf[t++] = itoa[dumpbuf[(i+pos)*16+j]>>4];
      textbuf[t++] = itoa[dumpbuf[(i+pos)*16+j]&0xF];
      textbuf[t++] = ' ';
    }
    textbuf[t++] = ' ';
    for(j=0;j<16;j++)
    {
      xgm::UINT8 c = dumpbuf[(i+pos)*16+j];
      if((0x20<=c&&c<=0x7E)||(0xA1<=c&&c<=0xDF))
        textbuf[t++] = c;
      else
        textbuf[t++] = '.';
    }
    dc.TextOut(0, i*14, textbuf, t);
  }

	// 描画用メッセージとして CDialog::OnPaint() を呼び出してはいけません
}

void NSFMemoryDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  int pos = GetScrollPos(SB_VERT);

  switch(nSBCode)
  {
  case SB_THUMBPOSITION:
    pos = nPos;
    break;

  case SB_PAGEUP:
    pos -=16;
    break;

  case SB_PAGEDOWN:
    pos +=16;
    break;

  case SB_LINEUP:
    pos--;
    break;

  case SB_LINEDOWN:
    pos++;
    break;

  case SB_ENDSCROLL:
    Invalidate(false);
    break;
  }

  if(pos<0)
    pos = 0;
  else if(65536/16<=pos)
    pos = 65536/16 - 1;

  SetScrollPos(SB_VERT, pos);

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void NSFMemoryDialog::OnJapanese() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
  dlgFont.CreateFont(14,0,0,0,0,0,0,0,SHIFTJIS_CHARSET,0,0,0,FIXED_PITCH,"MS ゴシック");
  GetMenu()->CheckMenuRadioItem(ID_JAPANESE, ID_ENGLISH, ID_JAPANESE, MF_BYCOMMAND);
}

void NSFMemoryDialog::OnEnglish() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
  dlgFont.CreateFont(14,0,0,0,0,0,0,0,ANSI_CHARSET,0,0,0,FIXED_PITCH,"Courier New");
  GetMenu()->CheckMenuRadioItem(ID_JAPANESE, ID_ENGLISH, ID_ENGLISH, MF_BYCOMMAND);
}

