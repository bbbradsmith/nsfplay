// nsfmwinDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "nsfmwin.h"
#include "nsfmwinDlg.h"
#include "nsfmeasure.h"
#include "utils/nsf_tag.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedOk();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CnsfmwinDlg ダイアログ



CnsfmwinDlg::CnsfmwinDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CnsfmwinDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  m_pThread = NULL;
  m_bStopflag = FALSE;
  m_bBtnMode = TRUE;
  m_nListCx = 0;
}

void CnsfmwinDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST, m_listbox);
  DDX_Control(pDX, IDC_PROGRESS2, m_progress2);
  DDX_Control(pDX, IDC_START, m_start);
  DDX_Control(pDX, IDC_DELETE, m_delete);
  DDX_Control(pDX, IDC_UP, m_up);
  DDX_Control(pDX, IDC_DOWN, m_down);
  DDX_Control(pDX, IDC_PROGRESS1, m_progress1);
  DDX_Control(pDX, IDC_TRACK, m_track);
  DDX_Control(pDX, IDC_FILE, m_file);
  DDX_Control(pDX, IDC_TOTAL, m_total);
  DDX_Control(pDX, IDC_PROGRESS0, m_progress0);
  DDX_Control(pDX, ID_OPTION, m_option);
}

BEGIN_MESSAGE_MAP(CnsfmwinDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
  ON_WM_DROPFILES()
  ON_BN_CLICKED(IDC_DELETE, OnBnClickedDelete)
  ON_BN_CLICKED(IDC_UP, OnBnClickedUp)
  ON_BN_CLICKED(IDC_DOWN, OnBnClickedDown)
  ON_BN_CLICKED(IDC_START, OnBnClickedStart)
  ON_BN_CLICKED(ID_OPTION, OnBnClickedOption)
  ON_WM_DESTROY()
END_MESSAGE_MAP()


// CnsfmwinDlg メッセージ ハンドラ

BOOL CnsfmwinDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
  DragAcceptFiles(TRUE);
  m_progress1.SetRange(0,100);
  m_progress2.SetRange(0,100);

  char inifile[MAX_PATH], *p;
  GetModuleFileName(AfxGetInstanceHandle(),inifile,MAX_PATH);
  p=strrchr(inifile,'.');
  if(p)
  {
    *p = '\0';
    strcat(inifile,".ini");
    m_nPriority = GetPrivateProfileInt("NSFMeasure","PRIORITY",0,inifile);
    m_nSearchLen = GetPrivateProfileInt("NSFMeasure","SEARCH_LENGH",60*5,inifile);
    m_nMinLoop = GetPrivateProfileInt("NSFMeasure","MIN_LOOP",30,inifile);
    m_nUseAlt = GetPrivateProfileInt("NSFMeasure","USE_ALT",1,inifile);
  }

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CnsfmwinDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CnsfmwinDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CnsfmwinDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void FindNSF( CString dir, CnsfmwinDlg *dlg, int nest)
{
  WIN32_FIND_DATA FindFileData;
  HANDLE hFind;

  if(nest>64) return ;

  dlg->m_total.SetWindowText(dir);
  hFind = FindFirstFile( dir + "\\*", &FindFileData);
  if(hFind!=INVALID_HANDLE_VALUE)
  {
    CSize size;
    CDC *pDC = dlg->m_listbox.GetDC();
    do
    {
      char *p;
      if(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
      {
        if(strcmp(FindFileData.cFileName,".")!=0&&strcmp(FindFileData.cFileName,"..")!=0)
          FindNSF((CString) dir + "\\" + FindFileData.cFileName, dlg, nest+1);
      }
      else if((p=strrchr(FindFileData.cFileName,'.'))&&
              (strcmp(p,".NSF")==0||strcmp(p,".nsf")==0))             
      {
        if(dlg->m_listbox.FindString(0,dir + "\\" + FindFileData.cFileName)==LB_ERR)
        {
          dlg->m_listbox.AddString(dir + "\\" + FindFileData.cFileName);
          size = pDC->GetTextExtent(dir + "\\" + FindFileData.cFileName);
          dlg->m_nListCx = max(size.cx,dlg->m_nListCx);
          dlg->m_listbox.SetHorizontalExtent(dlg->m_nListCx);
        }
      }
    }
    while(FindNextFile(hFind, &FindFileData)&&!dlg->m_bStopflag);
    dlg->m_listbox.ReleaseDC(pDC);
    FindClose(hFind);
  }

  return ;
}

UINT FileFindThread( LPVOID pParam )
{
  CnsfmwinDlg *dlg = (CnsfmwinDlg *)pParam;
  std::set<CString>::iterator it;

  for(it=dlg->m_setTarget.begin();it!=dlg->m_setTarget.end();it++)
  {
    FindNSF(*it,dlg, 1);
  }
  dlg->m_setTarget.clear();
  dlg->m_total.SetWindowText("");
  dlg->EnableDlgItem(TRUE);
  return 0;
}

void CnsfmwinDlg::StartFileFindThread()
{
  m_bStopflag = false;
  EnableDlgItem(FALSE);
  m_pThread=AfxBeginThread(FileFindThread,this,0,0,0,NULL);
  if(m_pThread==NULL)
  {
    MessageBox("Fatal error in creating a thread!");
    PostQuitMessage(1);
  }
}

void CnsfmwinDlg::OnDropFiles(HDROP hDropInfo)
{
  UINT nCount, nSize;
  CString str;
  CArray <char, int> aryFile;
  CDC *pDC = m_listbox.GetDC();
  CSize size;

  nCount = DragQueryFile(hDropInfo, -1, NULL, 0);
  for(UINT i=0;i<nCount;i++)
  {
    nSize = DragQueryFile(hDropInfo, i, NULL, 0);
    aryFile.SetSize(nSize+1);
    DragQueryFile(hDropInfo, i, aryFile.GetData(), nSize+1);

    if(GetFileAttributes(aryFile.GetData())&FILE_ATTRIBUTE_DIRECTORY)
      m_setTarget.insert((CString)aryFile.GetData());
    else if(nSize<4)
      continue;
    else if((strcmp("nsf", aryFile.GetData()+nSize-3)!=0)&&
            (strcmp("NSF", aryFile.GetData()+nSize-3)!=0))
      continue;
    else if(m_listbox.FindString(0,aryFile.GetData())==LB_ERR)
    {
      m_listbox.AddString(aryFile.GetData());
      size = pDC->GetTextExtent(aryFile.GetData());
      m_nListCx = max(size.cx,m_nListCx);
      m_listbox.SetHorizontalExtent(m_nListCx);
    }
  }
  m_listbox.ReleaseDC(pDC);

  if(m_setTarget.size())
    StartFileFindThread();

  CDialog::OnDropFiles(hDropInfo);
}

void CnsfmwinDlg::OnBnClickedDelete()
{
  int nCount = m_listbox.GetSelCount();
  CArray<int,int> aryListBoxSel;
  aryListBoxSel.SetSize(nCount);
  m_listbox.GetSelItems(nCount, aryListBoxSel.GetData()); 
  for(int i=nCount-1; i>=0; i--)
    m_listbox.DeleteString(aryListBoxSel[i]);
}

void CnsfmwinDlg::OnBnClickedUp()
{
  int nCount = m_listbox.GetSelCount();
  CArray<int,int> aryListBoxSel;
  aryListBoxSel.SetSize(nCount);
  m_listbox.GetSelItems(nCount, aryListBoxSel.GetData()); 
  for(int i=0; i<nCount; i++)
  {
    CString str;
    if(aryListBoxSel[i]!=0)
    {
      m_listbox.GetText(aryListBoxSel[i],str);
      m_listbox.DeleteString(aryListBoxSel[i]);
      aryListBoxSel[i] = aryListBoxSel[i] - 1;
      m_listbox.InsertString(aryListBoxSel[i],str);
      m_listbox.SetSel(aryListBoxSel[i]);
    }
    else break;
  }
}

void CnsfmwinDlg::OnBnClickedDown()
{
  int nCount = m_listbox.GetSelCount();
  CArray<int,int> aryListBoxSel;
  aryListBoxSel.SetSize(nCount);
  m_listbox.GetSelItems(nCount, aryListBoxSel.GetData()); 
  for(int i=nCount-1; i>=0; i--)
  {
    CString str;
    if(aryListBoxSel[i]!=m_listbox.GetCount()-1)
    {
      m_listbox.GetText(aryListBoxSel[i],str);
      m_listbox.DeleteString(aryListBoxSel[i]);
      aryListBoxSel[i] = aryListBoxSel[i] + 1;
      m_listbox.InsertString(aryListBoxSel[i],str);
      m_listbox.SetSel(aryListBoxSel[i]);
    }
    else break;
  }
}

static void UpdateTotal(CnsfmwinDlg *dlg, int songs, int files, int detected)
{
  CString str;
  str.Format("%d songs in ", songs);
  str.AppendFormat("%d files. ", files);
  str.AppendFormat("%d songs detected.", detected);
  dlg->m_total.SetWindowText(str);
}

static void UpdateFileTotal(CnsfmwinDlg *dlg, int file, int max_files, int skipped)
{
  CString str;
  str.Format("%d of ", file);
  str.AppendFormat("%d files. ", max_files);
  str.AppendFormat("%d skipped.", skipped);
  dlg->m_file.SetWindowText(str);
}

static void UpdateNSFTotal(CnsfmwinDlg *dlg, int song, int songs, int detected)
{
  CString str;
  str.Format("%d of ", song);
  str.AppendFormat("%d songs. ", songs);
  str.AppendFormat("(%d detected)", detected);
  dlg->m_track.SetWindowText(str);
}


UINT MeasThreadProc( LPVOID pParam )
{
  CnsfmwinDlg *nsfmDlg = (CnsfmwinDlg *)pParam;
  NSFMeas nsfm;
  xgm::NSF nsf;
  NSF_TAG ntag(&nsf);
  CString str;

  int max_time = nsfmDlg->m_nSearchLen * 1000;
  int min_loop = nsfmDlg->m_nMinLoop * 1000;
  int detected;
  int total_detected=0, total_songs=0, total_files=0;
  int max_files = nsfmDlg->m_listbox.GetCount(), skipped=0;

  while(nsfmDlg->m_listbox.GetCount())
  {
    detected = 0;
    total_files++;
    nsfmDlg->m_progress0.SetPos(100*total_files/max_files);
    UpdateFileTotal(nsfmDlg, total_files, max_files, skipped);

    nsfmDlg->m_listbox.GetText(0,str);
    if(!nsf.LoadFile(str))
    {    
      nsfmDlg->m_listbox.DeleteString(0);
      skipped++;
      continue;
    }

    ntag.Sync();
    ntag.CreateTag("%T(%n/%e)");
    
    for(int i=nsf.start-1;i<nsf.songs;i++)
    {
      total_songs++;
      nsfmDlg->m_progress1.SetPos(100*(i+1)/(nsf.songs-nsf.start+1));
      UpdateNSFTotal(nsfmDlg, i+1, (nsf.songs-nsf.start+1), detected);
      UpdateTotal(nsfmDlg, total_songs, total_files-skipped, total_detected);

      nsf.SetSong(i);
      if(nsfm.Open(&nsf,max_time,min_loop,!nsfmDlg->m_nUseAlt))
      {
        while(nsfm.Progress())
        {
          if(nsfmDlg->m_bStopflag) goto Exit;
          nsfmDlg->m_progress2.SetPos(100 * nsfm.GetProgressTime() / max_time);
        }
        nsfmDlg->m_progress2.SetPos(100);
        if(nsfm.IsDetected())
        {
          total_detected++;
          detected++;
          UpdateNSFTotal(nsfmDlg, i+1, (nsf.songs-nsf.start+1), detected);
          ntag.UpdateTagItem(i,"%T(%n/%e)",2);
        }
      }
    }
    UpdateTotal(nsfmDlg, total_songs, total_files-skipped, total_detected);
    nsfmDlg->m_listbox.DeleteString(0);
  }

Exit:
  nsfmDlg->EnableDlgItem(TRUE);
  return 0;
}

void CnsfmwinDlg::EnableDlgItem(BOOL bEnable)
{ 
  if(bEnable==FALSE)
  {
    m_listbox.SetTopIndex(0);
    m_listbox.SetCurSel(0);
  }
  //m_listbox.EnableWindow(bEnable);
  m_up.EnableWindow(bEnable);
  m_down.EnableWindow(bEnable);
  m_delete.EnableWindow(bEnable);
  m_option.EnableWindow(bEnable);
  DragAcceptFiles(bEnable);
  m_start.SetWindowText(bEnable?"&Start":"&Stop");
  m_bBtnMode = bEnable;
}

void CnsfmwinDlg::OnBnClickedStart()
{
  int PriorityList[] = {
    THREAD_PRIORITY_IDLE,
    THREAD_PRIORITY_LOWEST,
    THREAD_PRIORITY_BELOW_NORMAL,
    THREAD_PRIORITY_NORMAL,
    THREAD_PRIORITY_ABOVE_NORMAL,
    THREAD_PRIORITY_HIGHEST
  };

  if(m_bBtnMode)
  {
    m_bStopflag = false;
    EnableDlgItem(FALSE);
    m_pThread=AfxBeginThread(MeasThreadProc,this,PriorityList[m_nPriority],0,0,NULL);
    if(m_pThread==NULL)
    {
      MessageBox("Fatal error in creating a thread!");
      PostQuitMessage(1);
    }
  }
  else
  {
    m_bStopflag = true;
  }
}

void CnsfmwinDlg::OnBnClickedOption()
{
  COptionDlg optDlg;
  optDlg.m_priority = m_nPriority;
  optDlg.m_length = m_nSearchLen;
  optDlg.m_minloop = m_nMinLoop;
  optDlg.m_usealt = m_nUseAlt;
  if(optDlg.DoModal()==IDOK)
  {
    m_nPriority = optDlg.m_priority;
    m_nSearchLen = optDlg.m_length;
    m_nMinLoop = optDlg.m_minloop;
    m_nUseAlt = optDlg.m_usealt;
  }
}

void CnsfmwinDlg::OnDestroy()
{
  CDialog::OnDestroy();

  char inifile[MAX_PATH], *p;
  CString str;
  GetModuleFileName(AfxGetInstanceHandle(),inifile,MAX_PATH);
  p=strrchr(inifile,'.');
  if(p)
  {
    *p = '\0';
    strcat(inifile,".ini");
    str.Format("%d",m_nPriority);
    WritePrivateProfileString("NSFMeasure","PRIORITY",str,inifile);
    str.Format("%d",m_nSearchLen);
    WritePrivateProfileString("NSFMeasure","SEARCH_LENGH",str,inifile);
    str.Format("%d",m_nMinLoop);
    WritePrivateProfileString("NSFMeasure","MIN_LOOP",str,inifile);
    str.Format("%d",m_nUseAlt);
    WritePrivateProfileString("NSFMeasure","USE_ALT",str,inifile);
  }
}

void CAboutDlg::OnBnClickedOk()
{
  // TODO : ここにコントロール通知ハンドラ コードを追加します。
  OnOK();
}
