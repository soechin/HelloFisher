#include "stdafx.h"

#include "HelloFisher.h"
#include "HelloFisherDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CHelloFisherDlg, CDialogEx)
ON_WM_DESTROY()
END_MESSAGE_MAP()

CHelloFisherDlg::CHelloFisherDlg() : CDialogEx(IDD_HELLOFISHER_DIALOG) {
}

void CHelloFisherDlg::DoDataExchange(CDataExchange *pDX) {
  CDialogEx::DoDataExchange(pDX);
}

BOOL CHelloFisherDlg::OnInitDialog() {
  CDialogEx::OnInitDialog();

  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  SetIcon(m_hIcon, TRUE);
  SetIcon(m_hIcon, FALSE);

  // create thread
  m_running = true;
  m_thread1 = std::make_unique<std::thread>(
      std::bind(&CHelloFisherDlg::ThreadFunc1, this));
  m_thread2 = std::make_unique<std::thread>(
      std::bind(&CHelloFisherDlg::ThreadFunc2, this));

  return TRUE;
}

void CHelloFisherDlg::OnDestroy() {
  CDialogEx::OnDestroy();

  // destroy thread
  m_running = false;
  if (m_thread1 != nullptr && m_thread1->joinable()) {
    m_thread1->join();
  }
  if (m_thread2 != nullptr && m_thread2->joinable()) {
    m_thread2->join();
  }
  m_thread1 = nullptr;
  m_thread2 = nullptr;
}

void CHelloFisherDlg::ThreadFunc1() {
  while (m_running) {
    std::this_thread::yield();
  }
}

void CHelloFisherDlg::ThreadFunc2() {
  while (m_running) {
    std::this_thread::yield();
  }
}
