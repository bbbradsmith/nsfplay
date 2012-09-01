// KeyWindow.cpp : 実装ファイル
//
#include "stdafx.h"
#include "nsfplug_ui.h"
#include "resource.h"
#include "KeyWindow.h"

// KeyWindow ダイアログ
IMPLEMENT_DYNAMIC(KeyWindow, CDialog)
KeyWindow::KeyWindow(CWnd* pParent /*=NULL*/)
	: CDialog(KeyWindow::IDD, pParent)
{
  m_bShowOctave = false;
  m_bInit = false;
  for(int i=0;i<256;i++)
    m_nKeyStatus[i] = 0;
}

KeyWindow::~KeyWindow()
{
}

// キーボードを書く
void KeyWindow::draw_keyboard(CDC *pDC, int x, int y, int nx, int ny)
{
  static int key[14] = {1,2,1,2,1,0,1,2,1,2,1,2,1,0};
  CRect rect;
  int i;

  rect.left = 0;
  rect.top = 0;
  rect.right = 704;
  rect.bottom = 48; 
  pDC->FillSolidRect(&rect, RGB(0,0,0));

  rect.left = 0;
  rect.top = 0;
  rect.right = 14*8/2*nx;
  rect.bottom = ny;
  pDC->FillSolidRect(&rect, RGB(0,0,0));

  for(i=0; i<14*8; i+=2)
  {
    switch(key[i%14])
    {
    case 1:
      rect.left = x + i/2*nx;
      rect.top = y+1;
      rect.right = rect.left + nx-1;
      rect.bottom = rect.top + ny-2;
      pDC->FillSolidRect(&rect, RGB(240,240,240));
      pDC->SetPixel(rect.left-1,rect.bottom,RGB(240,240,240));
      pDC->SetPixel(rect.right,rect.bottom,RGB(240,240,240));
      break;
    default:
      break;
    }
  }

  for(i=0; i<14*8; i++)
  {
    switch(key[i%14])
    {
    case 2:
      rect.left = x + i/2*nx + nx/2 ;
      rect.top = y+1;
      rect.right = rect.left + nx - 1;
      rect.bottom = rect.top + ny*4/7;
      pDC->FillSolidRect(&rect, RGB(0,0,0));
      pDC->Draw3dRect(&rect, RGB(128,128,128), RGB(0,0,0));
      break;
    default:
      break;
    }
  }


}

void KeyWindow::draw_notes(CDC *pDC, int x, int y, int nx, int ny)
{
  CRect rect;
  int i;
  for(i=0; i<14*8; i++)
  {
    if(m_nKeyStatus[i])
    {
      if(i&1)
      {
        rect.left = x + i/2*nx+nx/2 + nx/4;
        rect.top = y-1+ny/4;
      }
      else
      {
        rect.left = x + i/2*nx + nx/4;
        rect.top = y+4+ny*4/7;
      }
      rect.right = rect.left + nx/2;
      rect.bottom = rect.top + nx/2;
      pDC->FillSolidRect(&rect, RGB((m_nKeyColor[i]>>16)&0xFF,(m_nKeyColor[i]>>8)&0xFF,m_nKeyColor[i]&0xFF));
    }
  }

  // オクターブ描画
  if(m_bShowOctave)
  {
    for(i=0; i<8; i++)
      pDC->BitBlt(x+i*7*nx+1,y+1+ny-2-6,4,5,&m_digDC,(i+1)*4,0,SRCCOPY);
  }
}

void KeyWindow::KeyOn(int note, int color)
{
  int tbl[12] = { 0, 1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 12 };
  if(note<=0) return;
  m_nKeyStatus[(tbl[note%12]+(note/12)*14)&0xFF] = 1;
  m_nKeyColor[(tbl[note%12]+(note/12)*14)&0xFF] = color;
}

void KeyWindow::KeyOff(int note)
{
  int tbl[12] = { 0, 1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 12 };
  if(note<=0) return;
  m_nKeyStatus[(tbl[note%12]+(note/12)*14)&0xFF] = 0;
}

void KeyWindow::Reset()
{
  for(int i=0;i<256;i++)
    m_nKeyStatus[i]=0;
}

void KeyWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(KeyWindow, CDialog)
  ON_WM_SIZE()
  ON_WM_PAINT()
  ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// KeyWindow メッセージ ハンドラ
BOOL KeyWindow::OnInitDialog()
{
  CDialog::OnInitDialog();

  CDC *pDC = GetDC();

  m_digBitmap.LoadBitmap(IDB_DIGIT);
  m_digDC.CreateCompatibleDC(pDC);
  m_digDC.SelectObject(&m_digBitmap);
  m_keyBitmap.CreateCompatibleBitmap(pDC,MaxWidth(),MaxWidth()/14);
  m_keyDC.CreateCompatibleDC(pDC);
  m_keyDC.SelectObject(&m_keyBitmap);
  m_memBitmap.CreateCompatibleBitmap(pDC,MaxWidth(),MaxWidth()/14);
  m_memDC.CreateCompatibleDC(pDC);
  m_memDC.SelectObject(&m_memBitmap);

  ReleaseDC(pDC);
  m_bInit = true;
  SetWindowPos(NULL,0,0,MinWidth(),24,SWP_NOMOVE|SWP_NOZORDER);

  return TRUE;  // return TRUE unless you set the focus to a control
  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void KeyWindow::OnSize(UINT nType, int cx, int cy)
{
  CDialog::OnSize(nType, cx, cy);

  if(m_bInit)
  {
    draw_keyboard(&m_keyDC,1,0,cy/4,cy);
  }
}

void KeyWindow::OnPaint()
{
  CPaintDC dc(this); // device context for painting
  CRect rect;
  GetClientRect(rect);

  m_memDC.BitBlt(0,0,rect.Width(),rect.Height(),&m_keyDC,0,0,SRCCOPY);
  draw_notes(&m_memDC,1,0,rect.Height()/4,rect.Height());
  dc.BitBlt(0,0,rect.Width(),rect.Height(),&m_memDC,0,0,SRCCOPY);

  // 描画メッセージで CDialog::OnPaint() を呼び出さないでください。
}

void KeyWindow::OnLButtonDblClk(UINT nFlags, CPoint point)
{
  m_bShowOctave = !m_bShowOctave;
  CDialog::OnLButtonDblClk(nFlags, point);
}

BOOL KeyWindow::DestroyWindow()
{
  return CDialog::DestroyWindow();
}
