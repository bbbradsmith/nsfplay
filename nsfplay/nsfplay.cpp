/*
 * nsfplay.cpp.h written by Mitsutaka Okazaki 2004.
 *
 * This software is provided 'as-is', without any express or implied warranty. 
 * In no event will the authors be held liable for any damages arising from 
 * the use of this software. 
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it 
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not 
 *    claim that you wrote the original software. If you use this software 
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not 
 *    be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 */
#include "stdafx.h"
#include "nsfplay.h"
#include "nsfplayDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CnsfplayApp

BEGIN_MESSAGE_MAP(CnsfplayApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()



CnsfplayApp::CnsfplayApp() :
    m_pDlg(NULL)
{
}

CnsfplayApp theApp;


BOOL CnsfplayApp::InitInstance()
{
	InitCommonControls();

    CWinApp::InitInstance();

    CMutex mutex(FALSE, m_pszExeName);
    if( mutex.Lock(0) == TRUE ) {

      CnsfplayDlg dlg;
      m_pDlg = &dlg;
      m_pMainWnd = &dlg;
      m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_ACCELERATOR)); 

      if(5==__argc) // command line wave out
      {
        char* nsf_file = __argv[1];
        char* wav_file = __argv[2];
        char* track = __argv[3];
        char* time = __argv[4];
        dlg.WriteSingleWave(nsf_file, wav_file, track, time);
        dlg.m_cancel_open = true;
      }
      else if(2<=__argc) 
      {
        dlg.m_init_file = CString(__argv[1]);
      }

	  INT_PTR nResponse = dlg.DoModal();
	  if (nResponse == IDOK)
	  {
	  }
	  else if (nResponse == IDCANCEL)
	  {
	  }
      mutex.Unlock();

    } else {

        // NOTE this string must match CAPTION in IDD_NSFPLAY_DIALOG in nsfplay.rc
        CWnd* pWnd = CWnd::FindWindow(NULL, "NSFplay 2.2");
        if( pWnd ) {
          pWnd->SetForegroundWindow();

        if(2<=__argc) {
          size_t size = strlen(__argv[1])+1;
          HANDLE hMem = GlobalAlloc(GMEM_ZEROINIT,sizeof(DROPFILES)+size+1);
          DROPFILES *DropFiles = (DROPFILES *)GlobalLock(hMem);
          DropFiles->pFiles=sizeof(DROPFILES);
          DropFiles->pt.x=10;
          DropFiles->pt.y=10;
          DropFiles->fNC=1;
          DropFiles->fWide=0;
          strcpy(((char *)DropFiles)+sizeof(DROPFILES),__argv[1]);
          *(((char *)DropFiles)+sizeof(DROPFILES)+size)='\0';
          GlobalUnlock(hMem);
          pWnd->PostMessage(WM_DROPFILES,(WPARAM)hMem,0);
        }
      }
    }
	return FALSE;
}

BOOL CnsfplayApp::ProcessMessageFilter(int code, LPMSG lpMsg)
{
  // keyboard player commands
  if(m_hAccel!= NULL && m_pDlg->m_hWnd == ::GetForegroundWindow())
  {
    if(::TranslateAccelerator(m_pMainWnd -> m_hWnd, m_hAccel, lpMsg))
    {
      //return TRUE;
      // allowing keyboard messages to pass through (no need to block)
    }
  }
  return CWinApp::ProcessMessageFilter(code, lpMsg);
}
