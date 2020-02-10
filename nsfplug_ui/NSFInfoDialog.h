#if !defined(AFX_NSFINFODIALOG_H__9C7E1B7E_5545_45F6_AB49_A5C1910D2670__INCLUDED_)
#define AFX_NSFINFODIALOG_H__9C7E1B7E_5545_45F6_AB49_A5C1910D2670__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NSFInfoDialog.h : �w�b�_�[ �t�@�C��
//
#include "NSFDialog.h"
#include "nsf_tag.h"
#include "NSFPresetDialog.h"

using namespace xgm;
/////////////////////////////////////////////////////////////////////////////
// NSFInfoDialog �_�C�A���O


class NSFInfoDialog : public CDialog, public NSFDialog
{
// �R���X�g���N�V����
public:
	NSFInfoDialog(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^
// �_�C�A���O �f�[�^
	//{{AFX_DATA(NSFInfoDialog)
	enum { IDD = IDD_INFOBOX };
	CSliderCtrl	m_songslider;
	UINT	m_song;
	CString	m_artist;
	CString	m_copyright;
	CString	m_title;
	CString	m_info;
	CString	m_fds;
	CString	m_fme7;
	CString	m_mmc5;
	CString	m_n106;
	CString	m_vrc6;
	CString	m_vrc7;
    CString m_pal;
	//}}AFX_DATA

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(NSFInfoDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(NSFInfoDialog)
	afx_msg void OnOk();
	afx_msg void OnMemdlg();
	afx_msg void OnEasy();
	afx_msg void OnConfigBox();
	afx_msg void OnMixerbox();
	afx_msg void OnPanBox();
	virtual BOOL OnInitDialog();
	afx_msg void OnAbout();
	afx_msg void OnSave();
	afx_msg void OnLoad();
	afx_msg void OnGenpls();
	afx_msg void OnMaskbox();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnNext();
	afx_msg void OnPrev();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// ���[�U�[�ǉ�
protected:
  NSF nsf, nsf_copy;
  NSF_TAG ntag;
  bool local_tag;
  void GeneratePlaylist(bool clear);

public:
  void SetInfo(char *fn);
  void SetInfo(NSF *n);

  afx_msg void OnDelall();
  afx_msg void OnDelone();
  afx_msg void OnReadtag();
  afx_msg void OnWritetag();
  afx_msg void OnTrkinfo();
  afx_msg void OnStnClickedArtist();
  afx_msg void OnDropFiles(HDROP hDropInfo);
  afx_msg void OnNewpls();
  afx_msg void OnLoadpreset();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_NSFINFODIALOG_H__9C7E1B7E_5545_45F6_AB49_A5C1910D2670__INCLUDED_)
