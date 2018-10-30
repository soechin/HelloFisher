#pragma once

// stl
#include <atomic>
#include <fstream>
#include <iomanip>
#include <map>
#include <thread>
#include <vector>
// opencv
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
// json
#include "json.hpp"

enum FISHING_STEP {
  FISHING_TERMINATED,
  FISHING_SLEEP_30S_BEGIN,
  FISHING_SLEEP_30S_END,
  FISHING_STOP,
  FISHING_SLEEP_10S_BEGIN,
  FISHING_SLEEP_10S_END,
  FISHING_GUESS_WASD,
  FISHING_GUESS_OK,
  FISHING_START,
};

class CHelloFisherDlg : public CDialogEx {
  DECLARE_MESSAGE_MAP()
 public:
  CHelloFisherDlg();
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg LRESULT OnLogTime(WPARAM wParam, LPARAM lParam);
  afx_msg void OnBnClickedDebugChk();
  virtual void ThreadFunc();

  virtual int64_t TimeNow();
  virtual void SleepFor(int ms);
  virtual void KeyDown(int vk);
  virtual void KeyUp(int vk);
  virtual void KeyPress(int vk);
  virtual bool IsCursorShowing();

  virtual cv::Mat Screenshot(cv::Rect roi);
  virtual void SaveImage(cv::Mat mat, std::wstring path);
  virtual bool SliderBar(cv::Mat box, int len);
  virtual bool TimerBar(cv::Mat box, int len, int &x, int &y);
  virtual int ArrowColor(cv::Mat arr);
  virtual bool ArrowType(cv::Mat arr, int color, double size, int &type);

 protected:
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_HELLOFISHER_DIALOG };
#endif
  HICON m_hIcon;
  CString m_ini;
  CString m_dir;
  nlohmann::json m_json;
  // thread
  std::atomic<bool> m_running;
  std::unique_ptr<std::thread> m_thread;
  // timer
  UINT_PTR m_timer;
  // switches
  std::atomic<bool> m_enabled;
  std::atomic<bool> m_semiauto;
  std::atomic<bool> m_debug;
  // status
  FISHING_STEP m_step;
  // settings
  int m_fishingDelay1;
  int m_fishingDelay2;
  int m_fishingDelay3;
  int m_spaceDelay1;
  int m_spaceDelay2;
  cv::Rect m_boxRect;
  int m_sliderLen;
  int m_timerLen;
  cv::Rect m_arrowRect;
  int m_arrowSize;
  // user interface
  CStatic m_enabledLbl;
  CStatic m_semiautoLbl;
  CStatic m_stepLbl;
  CStatic m_timeLbl;
  CButton m_debugChk;
};
