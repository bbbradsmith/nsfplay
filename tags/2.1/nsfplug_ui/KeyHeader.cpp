// KeyHeader.cpp : 実装ファイル
//

#include "stdafx.h"
#include "nsfplug_ui.h"
#include "resource.h"
#include "KeyHeader.h"


// KeyHeader ダイアログ

IMPLEMENT_DYNAMIC(KeyHeader, CDialog)
KeyHeader::KeyHeader(CWnd* pParent /*=NULL*/)
	: CDialog(KeyHeader::IDD, pParent)
{
  softgray_pen.CreatePen(PS_SOLID,1,RGB(212,212,212));
}

KeyHeader::~KeyHeader()
{
  if(softgray_pen.DeleteObject()==0)
    xgm::DEBUG_OUT("Error in deleting the softgray_pen object.\n");
}

void KeyHeader::Reset()
{
  m_nNoiseStatus = 0;
  m_nDPCMStatus = 0;
}

void KeyHeader::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KeyHeader, CDialog)
  ON_WM_PAINT()
END_MESSAGE_MAP()


// KeyHeader メッセージ ハンドラ

BOOL KeyHeader::OnInitDialog()
{
  CDialog::OnInitDialog();

  CDC *pDC = GetDC();

  m_hedBitmap.LoadBitmap(IDB_KEYHEADER);
  m_hedDC.CreateCompatibleDC(pDC);
  m_hedDC.SelectObject(&m_hedBitmap);
  m_memBitmap.CreateCompatibleBitmap(pDC,MaxWidth(),MinHeight());
  m_memDC.CreateCompatibleDC(pDC);
  m_memDC.SelectObject(&m_memBitmap);
  m_memDC.SelectObject(&softgray_pen);

  ReleaseDC(pDC);
  return TRUE;  // return TRUE unless you set the focus to a control
}

void KeyHeader::OnPaint()
{
  CPaintDC dc(this); // device context for painting
  CRect rect;
  GetClientRect(rect);

  m_memDC.FillSolidRect(rect,RGB(0,0,0));
  m_memDC.BitBlt(0,0,336,20,&m_hedDC,0,0,SRCCOPY);

  if(m_nNoiseStatus)
  {
    CRect rect(0,0,5,4);
    rect.MoveToXY(92+(16-m_nNoiseStatus)*8,5);
    m_memDC.FillSolidRect(rect, RGB(0,240,0));
  }
  if(m_nDPCMStatus)
  {
    CRect rect(0,0,5,4);
    rect.MoveToXY(92+(16-m_nDPCMStatus)*8,13);
    m_memDC.FillSolidRect(rect, RGB(0,240,0));
  }

  m_memDC.Draw3dRect(0,0,rect.right,rect.bottom,RGB(255,255,255),RGB(128,128,128));

  m_memDC.MoveTo(0,20);
  m_memDC.LineTo(rect.right,20);
  dc.BitBlt(0,0,rect.right,rect.bottom,&m_memDC,0,0,SRCCOPY);
}
