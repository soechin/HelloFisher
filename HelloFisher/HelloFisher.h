#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"

class CHelloFisherApp : public CWinApp {
  DECLARE_MESSAGE_MAP()
 public:
  CHelloFisherApp();
  virtual BOOL InitInstance();
};

extern CHelloFisherApp theApp;
