#include "stdafx.h"

#include "HelloFisher.h"
#include "HelloFisherDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CHelloFisherApp, CWinApp)
END_MESSAGE_MAP()

CHelloFisherApp theApp;

CHelloFisherApp::CHelloFisherApp() {
}

BOOL CHelloFisherApp::InitInstance() {
  INITCOMMONCONTROLSEX InitCtrls;
  InitCtrls.dwSize = sizeof(InitCtrls);
  InitCtrls.dwICC = ICC_WIN95_CLASSES;
  InitCommonControlsEx(&InitCtrls);

  CWinApp::InitInstance();
  CShellManager *pShellManager = new CShellManager;
  CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

  CHelloFisherDlg dlg;
  m_pMainWnd = &dlg;
  dlg.DoModal();

  if (pShellManager != nullptr) {
    delete pShellManager;
  }

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
  ControlBarCleanUp();
#endif

  return FALSE;
}
