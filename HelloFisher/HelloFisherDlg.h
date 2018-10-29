#pragma once

// stl
#include <atomic>
#include <map>
#include <thread>
#include <vector>
// opencv
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

class CHelloFisherDlg : public CDialogEx {
  DECLARE_MESSAGE_MAP()
 public:
  CHelloFisherDlg();
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  virtual void ThreadFunc();

  virtual void DoFishing();

  virtual int64_t TimeNow();
  virtual void SleepFor(int ms);
  virtual void KeyDown(int vk);
  virtual void KeyUp(int vk);
  virtual void KeyPress(int vk);
  virtual bool IsCursorShowing();

  virtual cv::Mat Screenshot(cv::Rect roi);
  virtual bool SliderBar(cv::Mat box, int len);
  virtual bool TimerBar(cv::Mat box, int len, int &x, int &y);
  virtual int ArrowColor(cv::Mat arr);
  virtual bool ArrowType(cv::Mat arr, int color, double size, int &type);

 protected:
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_HELLOFISHER_DIALOG };
#endif
  HICON m_hIcon;
  // thread
  std::atomic<bool> m_running;
  std::unique_ptr<std::thread> m_thread;
  // timer
  UINT_PTR m_timer;
  // switches
  std::atomic<bool> m_enabled;
  std::atomic<bool> m_onetime;
  std::atomic<int> m_actions;
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
};
