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

  return TRUE;
}

void CHelloFisherDlg::OnDestroy() {
  CDialogEx::OnDestroy();
}
