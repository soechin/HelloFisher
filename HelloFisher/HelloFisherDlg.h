#pragma once

class CHelloFisherDlg : public CDialogEx {
  DECLARE_MESSAGE_MAP()
 public:
  CHelloFisherDlg();
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();

 protected:
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_HELLOFISHER_DIALOG };
#endif
  HICON m_hIcon;
};
