#pragma once

#include <atomic>
#include <thread>

class CHelloFisherDlg : public CDialogEx {
  DECLARE_MESSAGE_MAP()
 public:
  CHelloFisherDlg();
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  virtual void ThreadFunc1();
  virtual void ThreadFunc2();

 protected:
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_HELLOFISHER_DIALOG };
#endif
  HICON m_hIcon;
  std::atomic<bool> m_running;
  std::unique_ptr<std::thread> m_thread1;
  std::unique_ptr<std::thread> m_thread2;
};
