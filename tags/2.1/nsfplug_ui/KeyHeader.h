#pragma once


// KeyHeader ダイアログ

class KeyHeader : public CDialog
{
	DECLARE_DYNAMIC(KeyHeader)

public:
	KeyHeader(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~KeyHeader();

  // キーボードヘッダの元絵
  CBitmap m_hedBitmap;
  CDC m_hedDC;

  // 裏画面領域
  CBitmap m_memBitmap;
  CDC m_memDC;

  // ペン
  CPen softgray_pen;

  int m_nNoiseStatus;
  int m_nDPCMStatus;

  virtual void Reset();

  inline int MinWidth(){ return 336+1; }
  inline int MaxWidth(){ return 336*2+1; }
  inline int MinHeight(){ return 24; }
  inline int MaxHeight(){ return 24; }

// ダイアログ データ
	enum { IDD = IDD_KEYHEADER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  afx_msg void OnPaint();
};
