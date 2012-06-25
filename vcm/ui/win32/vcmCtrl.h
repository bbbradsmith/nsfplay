#pragma once
#include "../../vcm.h"

class CvcmCtrl : public CDialog
{
	DECLARE_DYNAMIC(CvcmCtrl)

public:
  void AttachConfig( vcm::Configuration &config );
  vcm::Configuration *m_pConfig;
  vcm::ValueCtrl *m_vt;
  std::string m_id;
  CvcmCtrl(const std::string &id, vcm::ValueCtrl *vt, UINT nIDTemplate, CWnd *pParent=NULL);
  virtual ~CvcmCtrl();
  void NotifyUpdate();
  virtual void ReadWork(){}
  virtual void WriteWork(){}
  void UpdateStatusMessage();

protected:
  // 位置調節用
  CSize GetTextSize( const CString &str );
  CPoint AlignGridX( const CPoint pos );


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
