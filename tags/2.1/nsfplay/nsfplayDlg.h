/*
 * nsfplayDlg.h written by Mitsutaka Okazaki 2004.
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
#pragma once
#include "../libemuwa2/emu_winamp.h"
#include "afxwin.h"
#include "afxcmn.h"

class CnsfplayDlg : public CDialog
{
public:
	CnsfplayDlg(CWnd* pParent = NULL);
    ~CnsfplayDlg();

	enum { IDD = IDD_NSFPLAY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);


protected:
	HICON m_hIcon;
    HICON m_hIcon_btn[2];
    EmuWinamp *m_emu;
    UINT_PTR m_timerID;
    bool m_sb_dragging;
    void UpdateInfo();
    int m_update_wait;
    int m_last_time;
    int m_last_len;
    char m_last_title[1024];
    char m_IniPath[MAX_PATH+16];

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  CString m_init_file;
  BOOL m_cancel_open;
  afx_msg void OnDropFiles(HDROP hDropInfo);
  afx_msg void OnBnClickedPrev();
  afx_msg void OnBnClickedNext();
  afx_msg void OnBnClickedInfo();
  afx_msg void OnTimer(UINT nIDEvent);
  CStatic m_timebox;
  afx_msg void OnDestroy();
  afx_msg void OnBnClickedStop();
  afx_msg void OnBnClickedPlay();
  CSliderCtrl m_slider;
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  CStatic m_title;
  CStatic m_time_max;
  afx_msg void OnBnClickedKbwin();
  afx_msg void OnBnClickedPause();
  afx_msg void OnBnClickedOpen();
  CButton m_open_btn;
  CButton m_prop_btn;
  CSliderCtrl m_volume;
  afx_msg void OnBnClickedConfig();
  afx_msg void OnBnClickedWaveout();

  int WriteSingleWave(char* nsf_file, char* wave_file, char* track, char* ms);
};
