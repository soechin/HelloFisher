#include "stdafx.h"

#include "HelloFisher.h"
#include "HelloFisherDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_LOG_TIME (WM_APP + 1)

BEGIN_MESSAGE_MAP(CHelloFisherDlg, CDialogEx)
ON_WM_DESTROY()
ON_WM_TIMER()
ON_MESSAGE(WM_LOG_TIME, CHelloFisherDlg::OnLogTime)
ON_BN_CLICKED(IDC_DEBUG_CHK, &CHelloFisherDlg::OnBnClickedDebugChk)
ON_BN_CLICKED(IDC_GOLD_CHK, &CHelloFisherDlg::OnBnClickedGoldChk)
ON_BN_CLICKED(IDC_BLUE_CHK, &CHelloFisherDlg::OnBnClickedBlueChk)
ON_BN_CLICKED(IDC_GREEN_CHK, &CHelloFisherDlg::OnBnClickedGreenChk)
END_MESSAGE_MAP()

CHelloFisherDlg::CHelloFisherDlg() : CDialogEx(IDD_HELLOFISHER_DIALOG) {
}

void CHelloFisherDlg::DoDataExchange(CDataExchange *pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_ENABLED_LBL, m_enabledLbl);
  DDX_Control(pDX, IDC_SEMIAUTO_LBL, m_semiautoLbl);
  DDX_Control(pDX, IDC_STEP_LBL, m_stepLbl);
  DDX_Control(pDX, IDC_TIME_LBL, m_timeLbl);
  DDX_Control(pDX, IDC_DEBUG_CHK, m_debugChk);
  DDX_Control(pDX, IDC_GOLD_CHK, m_goldChk);
  DDX_Control(pDX, IDC_BLUE_CHK, m_blueChk);
  DDX_Control(pDX, IDC_GREEN_CHK, m_greenChk);
}

BOOL CHelloFisherDlg::OnInitDialog() {
  CDialogEx::OnInitDialog();
  std::ifstream ifs;
  CString drops;
  WIN32_FIND_DATA data;
  HANDLE handle;

  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  SetIcon(m_hIcon, TRUE);
  SetIcon(m_hIcon, FALSE);

  AfxGetModuleFileName(AfxGetInstanceHandle(), m_ini);
  PathRenameExtension(m_ini.GetBuffer(m_ini.GetLength() + MAX_PATH),
                      TEXT(".json"));
  m_ini.ReleaseBuffer();

  m_dir = m_ini;
  PathRemoveFileSpec(m_dir.GetBuffer(m_dir.GetLength() + MAX_PATH));
  m_dir.ReleaseBuffer();

  drops = m_dir + TEXT("\\drops\\");
  handle = FindFirstFile(drops + TEXT("*.png"), &data);

  if (handle != INVALID_HANDLE_VALUE) {
    do {
      m_drops.push_back(ReadImage((LPWSTR)ATL::CT2W(drops + data.cFileName)));
    } while (FindNextFile(handle, &data));

    FindClose(handle);
  }

  // switches
  m_enabled = false;
  m_semiauto = false;
  m_debug = false;
  m_gold = false;
  m_blue = false;
  m_green = false;

  // status
  m_step = FISHING_TERMINATED;

  // default settings
  m_json["FishingDelay1"] = 30000;
  m_json["FishingDelay2"] = 130000;
  m_json["FishingDelay3"] = 10000;
  m_json["SpaceDelay1"] = 1500;
  m_json["SpaceDelay2"] = 3000;
  m_json["BoxRect"] = {720, 320, 480, 150};
  m_json["SliderLen"] = 268;
  m_json["TimerLen"] = 255;
  m_json["ArrowRect"] = {-42, -42, 34, 14};
  m_json["ArrowSize"] = 10;
  m_json["DropRect"] = {1532, 586, 198, 156};
  m_json["DropGold"] = 21;
  m_json["DropBlue"] = 100;
  m_json["DropGreen"] = 49;
  m_json["DropLen"] = 36;

  // load settings
  ifs.open(m_ini);

  if (ifs.is_open()) {
    ifs >> m_json;
    ifs.close();
  }

  m_fishingDelay1 = m_json["FishingDelay1"];
  m_fishingDelay2 = m_json["FishingDelay2"];
  m_fishingDelay3 = m_json["FishingDelay3"];
  m_spaceDelay1 = m_json["SpaceDelay1"];
  m_spaceDelay2 = m_json["SpaceDelay2"];
  m_boxRect.x = m_json["BoxRect"][0];
  m_boxRect.y = m_json["BoxRect"][1];
  m_boxRect.width = m_json["BoxRect"][2];
  m_boxRect.height = m_json["BoxRect"][3];
  m_sliderLen = m_json["SliderLen"];
  m_timerLen = m_json["TimerLen"];
  m_arrowRect.x = m_json["ArrowRect"][0];
  m_arrowRect.y = m_json["ArrowRect"][1];
  m_arrowRect.width = m_json["ArrowRect"][2];
  m_arrowRect.height = m_json["ArrowRect"][3];
  m_arrowSize = m_json["ArrowSize"];
  m_dropRect.x = m_json["DropRect"][0];
  m_dropRect.y = m_json["DropRect"][1];
  m_dropRect.width = m_json["DropRect"][2];
  m_dropRect.height = m_json["DropRect"][3];
  m_dropGold = m_json["DropGold"];
  m_dropBlue = m_json["DropBlue"];
  m_dropGreen = m_json["DropGreen"];
  m_dropLen = m_json["DropLen"];

  // create thread
  m_running = true;
  m_thread = std::make_unique<std::thread>(&CHelloFisherDlg::ThreadFunc, this);

  // create timer
  m_timer = SetTimer(0, 100, NULL);

  return TRUE;
}

void CHelloFisherDlg::OnDestroy() {
  CDialogEx::OnDestroy();
  std::ofstream ofs;

  // turn off switches
  m_enabled = false;
  m_semiauto = false;
  m_debug = false;

  // destroy thread
  if (m_thread != nullptr) {
    m_running = false;
    if (m_thread->joinable()) {
      m_thread->join();
    }
    m_thread = nullptr;
  }

  // destroy timer
  KillTimer(m_timer);

  // save settings
  ofs.open(m_ini);

  if (ofs.is_open()) {
    ofs << std::setw(4) << m_json;
    ofs.close();
  }
}

void CHelloFisherDlg::OnTimer(UINT_PTR nIDEvent) {
  CString text, old;

  if ((GetAsyncKeyState(VK_SCROLL) & 0x0001) != 0) {
    m_enabled = (GetKeyState(VK_SCROLL) & 0x0001) != 0;
    if (m_enabled) m_semiauto = true;
  }

  if ((GetAsyncKeyState(VK_OEM_3) & 0x0001) != 0) {
    m_semiauto = true;
  }

  m_enabledLbl.EnableWindow(m_enabled);
  m_semiautoLbl.EnableWindow(m_semiauto);

  switch (m_step) {
    case FISHING_SLEEP_30S_BEGIN:
    case FISHING_SLEEP_30S_END:
      text.Format(TEXT("等待開始..."));
      break;
    case FISHING_STOP:
      text.Format(TEXT("收竿"));
      break;
    case FISHING_SLEEP_10S_BEGIN:
    case FISHING_SLEEP_10S_END:
      text.Format(TEXT("等待重試..."));
      break;
    case FISHING_GUESS_WASD:
      text.Format(TEXT("輸入按鍵"));
      break;
    case FISHING_DROP:
      text.Format(TEXT("撿取"));
      break;
    case FISHING_START:
      text.Format(TEXT("拋竿"));
      break;
  }

  m_stepLbl.GetWindowText(old);
  if (text != old) {
    m_stepLbl.SetWindowText(text);
  }

  CDialogEx::OnTimer(nIDEvent);
}

LRESULT CHelloFisherDlg::OnLogTime(WPARAM wParam, LPARAM lParam) {
  CString text;
  text.Format(TEXT("%.1lf 秒"), (double)wParam / 1000);
  m_timeLbl.SetWindowText(text);
  return 0;
}

void CHelloFisherDlg::OnBnClickedDebugChk() {
  m_debug = (m_debugChk.GetCheck() == BST_CHECKED);
}

void CHelloFisherDlg::OnBnClickedGoldChk() {
  m_gold = (m_goldChk.GetCheck() == BST_CHECKED);
}

void CHelloFisherDlg::OnBnClickedBlueChk() {
  m_blue = (m_blueChk.GetCheck() == BST_CHECKED);
}

void CHelloFisherDlg::OnBnClickedGreenChk() {
  m_green = (m_greenChk.GetCheck() == BST_CHECKED);
}

void CHelloFisherDlg::ThreadFunc() {
  std::wstring wasd;
  cv::Mat box, all, arrs[10];
  cv::Rect roi;
  int64_t time0, time1;
  int x, y, color;
  bool drop;

  while (m_running) {
    if (m_enabled || m_semiauto) {
      if (m_step == FISHING_TERMINATED) {
        m_step = FISHING_SLEEP_30S_BEGIN;
      }
    } else {
      m_step = FISHING_TERMINATED;
    }

    if (m_step == FISHING_SLEEP_30S_BEGIN) {
      // fishing time base
      time0 = TimeNow();

      m_step = FISHING_SLEEP_30S_END;
      continue;
    } else if (m_step == FISHING_SLEEP_30S_END) {
      // sleep for 30 seconds in full-auto mode
      if (!m_semiauto) {
        if ((TimeNow() - time0) < m_fishingDelay1) {
          SleepFor(100);
          continue;
        }
      }

      m_step = FISHING_STOP;
      continue;
    } else if (m_step == FISHING_STOP) {
      // time out
      if ((TimeNow() - time0) >= m_fishingDelay2) {
        // turn off switches
        m_enabled = false;
        m_semiauto = false;
        // log time
        PostMessage(WM_LOG_TIME, (WPARAM)(TimeNow() - time0));
        continue;
      }

      // user is typing text
      if (IsCursorShowing()) {
        m_step = FISHING_SLEEP_10S_BEGIN;
        continue;
      }

      // press SPACE key to stop fishing
      KeyPress(VK_SPACE);
      SleepFor(m_spaceDelay1);

      // take screenshot
      box = Screenshot(m_boxRect);

      // try to find slider bar
      if (!SliderBar(box, m_sliderLen)) {
        // debug
        if (m_debug) {
          WriteImage(box, (LPWSTR)ATL::CT2W(m_dir + TEXT("\\1.png")));
        }

        m_step = FISHING_SLEEP_10S_BEGIN;
        continue;
      }

      // press SPACE key again
      KeyPress(VK_SPACE);
      SleepFor(m_spaceDelay2);

      // debug
      if (m_debug) {
        WriteImage(box, (LPWSTR)ATL::CT2W(m_dir + TEXT("\\2.png")));
      }

      m_step = FISHING_GUESS_WASD;
      continue;
    } else if (m_step == FISHING_SLEEP_10S_BEGIN) {
      // sleeping time base
      time1 = TimeNow();

      m_step = FISHING_SLEEP_10S_END;
      continue;
    } else if (m_step == FISHING_SLEEP_10S_END) {
      // sleep for 10 seconds in full-auto mode
      if (!m_semiauto) {
        if ((TimeNow() - time1) < m_fishingDelay3) {
          SleepFor(100);
          continue;
        }
      }

      // turn to full-auto mode
      m_semiauto = false;

      m_step = FISHING_STOP;
      continue;
    } else if (m_step == FISHING_GUESS_WASD) {
      wasd.clear();

      // take screenshot
      box = Screenshot(m_boxRect);

      // find timer bar
      if (TimerBar(box, m_timerLen, x, y)) {
        // crop all arrows
        roi.x = m_arrowRect.x + x;
        roi.y = m_arrowRect.y + y;
        roi.width = m_arrowRect.width * 10;
        roi.height = m_arrowRect.height;
        all = box(roi);

        // for each arrow
        for (int i = 0; i < 10; i++) {
          roi.x = m_arrowRect.width * i;
          roi.y = 0;
          roi.width = m_arrowRect.width;
          roi.height = m_arrowRect.height;
          arrs[i] = all(roi);
        }

        // find color of arrow 0 and arrow 1
        color = ArrowColor(arrs[0]);

        if (color == ArrowColor(arrs[1])) {
          for (int i = 0; i < 10; i++) {
            int type;

            // guess arrow type
            if (!ArrowType(arrs[i], color, m_arrowSize, type)) {
              break;
            }

            wasd += type;
          }
        }
      }

      // press WASD keys
      for (int i = 0; i < (int)wasd.length(); i++) {
        KeyPress(wasd[i]);
      }

      // debug
      if (m_debug) {
        WriteImage(box, (LPWSTR)ATL::CT2W(m_dir + TEXT("\\3.png")));
      }

      m_step = FISHING_DROP;
      continue;
    } else if (m_step == FISHING_DROP) {
      // take screenshot
      box = Screenshot(m_boxRect);

      // wait timer bar
      if (TimerBar(box, m_timerLen, x, y)) {
        SleepFor(100);
        continue;
      }

      // take screenshot(drop area)
      box = Screenshot(m_dropRect);
      drop = !(m_gold || m_blue || m_green);

      if (!drop && m_gold) {
        if (DropFilter(box, m_dropGold, 5, 128, 64, m_dropLen)) {
          drop = true;
        }
      }

      if (!drop && m_blue) {
        if (DropFilter(box, m_dropBlue, 5, 128, 64, m_dropLen)) {
          drop = true;
        }
      }

      if (!drop && m_green) {
        if (DropFilter(box, m_dropGreen, 5, 128, 64, m_dropLen)) {
          drop = true;
        }
      }

      if (!drop) {
        for (int i = 0; i < (int)m_drops.size(); i++) {
          if (DropTemplate(box, m_drops[i], 0.95)) {
            drop = true;
            break;
          }
        }
      }

      if (drop) {
        // press R key
        KeyPress('R');
      }

      // debug
      if (m_debug) {
        WriteImage(box, (LPWSTR)ATL::CT2W(m_dir + TEXT("\\4.png")));
      }

      m_step = FISHING_START;
      continue;
    } else if (m_step == FISHING_START) {
      // press SPACE key to start fishing
      KeyPress(VK_SPACE);

      // log time
      PostMessage(WM_LOG_TIME, (WPARAM)(TimeNow() - time0));

      m_step = FISHING_SLEEP_30S_BEGIN;
      continue;
    }

    SleepFor(100);
  }  // loop
}

int64_t CHelloFisherDlg::TimeNow() {
  static auto zero = std::chrono::high_resolution_clock::now();
  auto now = std::chrono::high_resolution_clock::now();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - zero);
  return ms.count();
}

void CHelloFisherDlg::SleepFor(int ms) {
  if (ms > 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  } else {
    std::this_thread::yield();
  }
}

void CHelloFisherDlg::KeyDown(int vk) {
  INPUT input;

  memset(&input, 0, sizeof(input));
  input.type = INPUT_KEYBOARD;
  input.ki.wVk = vk;
  input.ki.wScan = MapVirtualKeyW(vk, MAPVK_VK_TO_VSC);
  input.ki.dwFlags = 0;

  SendInput(1, &input, sizeof(input));
}

void CHelloFisherDlg::KeyUp(int vk) {
  INPUT input;

  memset(&input, 0, sizeof(input));
  input.type = INPUT_KEYBOARD;
  input.ki.wVk = vk;
  input.ki.wScan = MapVirtualKeyW(vk, MAPVK_VK_TO_VSC);
  input.ki.dwFlags = KEYEVENTF_KEYUP;

  SendInput(1, &input, sizeof(input));
}

void CHelloFisherDlg::KeyPress(int vk) {
  KeyDown(vk);
  SleepFor(50);
  KeyUp(vk);
  SleepFor(100);
}

bool CHelloFisherDlg::IsCursorShowing() {
  CURSORINFO cursor;

  memset(&cursor, 0, sizeof(cursor));
  cursor.cbSize = sizeof(cursor);

  if (GetCursorInfo(&cursor)) {
    if ((cursor.flags & CURSOR_SHOWING) != 0) {
      return true;
    }
  }

  return false;
}

cv::Mat CHelloFisherDlg::Screenshot(cv::Rect roi) {
  cv::Mat mat;
  BITMAPINFOHEADER bmih;
  HWND hwnd;
  RECT rect;
  POINT point;
  HDC hdc, mdc;
  HGDIOBJ dib, obj;
  BYTE *src, *dst;
  void *data;
  int width, height, step;

  hwnd = ::GetForegroundWindow();

  if (::GetClientRect(hwnd, &rect)) {
    point.x = rect.left;
    point.y = rect.top;

    if (::ClientToScreen(hwnd, &point)) {
      roi.x += point.x;
      roi.y += point.y;
    }
  }

  mat.create(roi.size(), CV_8UC3);
  width = mat.cols;
  height = mat.rows;
  step = (width * 3 + 3) & (-4);

  memset(&bmih, 0, sizeof(bmih));
  bmih.biSize = sizeof(bmih);
  bmih.biWidth = width;
  bmih.biHeight = height;
  bmih.biPlanes = 1;
  bmih.biBitCount = 24;
  bmih.biCompression = BI_RGB;
  bmih.biSizeImage = step * height;

  hdc = ::GetDC(NULL);
  mdc = CreateCompatibleDC(hdc);
  dib = CreateDIBSection(hdc, (BITMAPINFO *)&bmih, DIB_RGB_COLORS, &data, NULL,
                         0);
  obj = SelectObject(mdc, dib);

  BitBlt(mdc, 0, 0, width, height, hdc, roi.x, roi.y, SRCCOPY);

  src = (BYTE *)data + step * (height - 1);
  dst = (BYTE *)mat.data;

  for (int j = 0; j < height; j++) {
    memcpy(dst, src, width * 3);
    src -= step;
    dst += mat.step;
  }

  SelectObject(mdc, obj);
  DeleteObject(dib);
  DeleteDC(mdc);
  ::ReleaseDC(NULL, hdc);

  return mat;
}

cv::Mat CHelloFisherDlg::ReadImage(std::wstring path) {
  std::ifstream ifs;
  std::vector<uchar> buf;

  ifs.open(path, std::ios::binary);
  if (ifs.is_open()) {
    ifs.seekg(0, std::ios::end);
    buf.resize((int)ifs.tellg());
    ifs.seekg(0, std::ios::beg);
    ifs.read((char *)buf.data(), (int)buf.size());
    ifs.close();
  }

  return cv::imdecode(buf, cv::IMREAD_COLOR);
}

void CHelloFisherDlg::WriteImage(cv::Mat mat, std::wstring path) {
  std::ofstream ofs;
  std::vector<uchar> buf;

  if (!cv::imencode(".png", mat, buf)) {
    return;
  }

  ofs.open(path, std::ios::binary);
  if (ofs.is_open()) {
    ofs.write((const char *)buf.data(), (int)buf.size());
    ofs.close();
  }
}

bool CHelloFisherDlg::SliderBar(cv::Mat box, int len) {
  int mid;

  mid = box.cols / 2;

  for (int j = 0; j < (box.rows - 1); j++) {
    // R R R R R R ? ? ?
    // . . . . . . ? ? ? W W W W
    uchar *p = box.ptr(j);
    uchar *q = box.ptr(j + 1);
    int left = mid;
    int right = mid - 1;

    // find right edge
    for (int i = mid; i < box.cols; i++) {
      int b = p[i * 3 + 0];
      int g = p[i * 3 + 1];
      int r = p[i * 3 + 2];

      // red color
      if ((r - g) >= 128 && (r - b) >= 128) {
        right = i;
      } else {
        break;
      }
    }

    // find left edge
    for (int i = mid - 1; i >= 0; i--) {
      int b = p[i * 3 + 0];
      int g = p[i * 3 + 1];
      int r = p[i * 3 + 2];

      // red color
      if ((r - g) >= 128 && (r - b) >= 128) {
        left = i;
      } else {
        break;
      }
    }

    // skip 10 pixels and find right edge
    for (int i = right + 10; i < box.cols; i++) {
      int b = q[i * 3 + 0];
      int g = q[i * 3 + 1];
      int r = q[i * 3 + 2];

      // white color
      if (r >= 200 && abs(r - g) <= 25 && abs(r - b) <= 25) {
        right = i;
      } else {
        break;
      }
    }

    if ((right - left + 1) >= len) {
      return true;
    }
  }

  return false;
}

bool CHelloFisherDlg::TimerBar(cv::Mat box, int len, int &x, int &y) {
  cv::Mat bin;
  uchar *p, *q;
  int mid;

  cv::cvtColor(box, bin, cv::COLOR_BGR2GRAY);

  p = bin.data;
  q = p + bin.step;
  mid = bin.cols / 2;

  for (int j = 0; j < (bin.rows - 1); j++) {
    // W W W W W
    // B B B B B
    int left = mid;
    int right = mid - 1;

    for (int i = mid; i < bin.cols; i++) {
      if (p[i] >= 200 && q[i] <= 50) {
        right = i;
      } else {
        break;
      }
    }

    for (int i = mid - 1; i >= 0; i--) {
      if (p[i] >= 200 && q[i] <= 50) {
        left = i;
      } else {
        break;
      }
    }

    if ((right - left + 1) >= len) {
      x = left;
      y = j;
      return true;
    }

    p += bin.step;
    q += bin.step;
  }

  return false;
}

int CHelloFisherDlg::ArrowColor(cv::Mat arr) {
  std::map<int, int> hist;
  int color, num;

  for (int j = 0; j < arr.rows; j++) {
    uchar *p = arr.ptr(j);

    for (int i = 0; i < arr.cols; i++, p += 3) {
      int n = p[0] | p[1] << 8 | p[2] << 16;
      hist[n]++;
    }
  }

  color = 0;
  num = 0;

  for (auto it = hist.begin(); it != hist.end(); it++) {
    if (it->second > num) {
      color = it->first;
      num = it->second;
    }
  }

  return color;
}

bool CHelloFisherDlg::ArrowType(cv::Mat arr, int color, double size,
                                int &type) {
  std::vector<std::vector<cv::Point>> conts;
  std::vector<cv::Point2f> pts;
  cv::Mat bin;
  int idx;
  double area, mindx, mindy;

  bin.create(arr.size(), CV_8UC1);

  for (int j = 0; j < arr.rows; j++) {
    uchar *p = arr.ptr(j);
    uchar *q = bin.ptr(j);

    for (int i = 0; i < arr.cols; i++, p += 3) {
      int n = p[0] | p[1] << 8 | p[2] << 16;
      q[i] = (n == color) ? 255 : 0;
    }
  }

  cv::findContours(bin, conts, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
  idx = 0;
  area = 0;

  for (int i = 0; i < (int)conts.size(); i++) {
    double a = cv::contourArea(conts[i]);

    if (a > area) {
      idx = i;
      area = a;
    }
  }

  if (conts.empty() || area < size) {
    return false;
  }

  cv::minEnclosingTriangle(conts[idx], pts);
  if (pts.size() != 3) return false;

  idx = 0;
  mindx = FLT_MAX;
  mindy = FLT_MAX;

  for (int i = 0; i < 3; i++) {
    cv::Point2f &a = pts[i];
    cv::Point2f &b = pts[(i + 1) % 3];
    float dx = abs(a.x - b.x);
    float dy = abs(a.y - b.y);

    if (dx < mindx && dx < mindy) {
      idx = i;
      mindx = dx;
    }

    if (dy < mindx && dy < mindy) {
      idx = i;
      mindy = dy;
    }
  }

  if (mindx < mindy) {
    cv::Point2f &a = pts[idx];
    cv::Point2f &b = pts[(idx + 2) % 3];
    type = b.x < a.x ? 'A' : 'D';
  } else {
    cv::Point2f &a = pts[idx];
    cv::Point2f &b = pts[(idx + 2) % 3];
    type = b.y < a.y ? 'W' : 'S';
  }

  return true;
}

bool CHelloFisherDlg::DropFilter(cv::Mat mat, int hue, int dif, int sat,
                                 int val, int len) {
  cv::Mat chnls[3], hsv, bin;
  int huel, hueh, num;

  cv::cvtColor(mat, hsv, cv::COLOR_BGR2HSV);
  cv::split(hsv, chnls);

  huel = (hue - dif + 180) % 180;
  hueh = (hue + dif + 180) % 180;

  if (huel < hueh) {
    bin = chnls[0] >= huel & chnls[0] <= hueh;
  } else {
    bin = chnls[0] >= huel | chnls[0] <= hueh;
  }

  bin &= chnls[1] >= sat & chnls[2] >= val;
  num = 0;

  for (int j = 0; j < bin.rows; j++) {
    uchar *p = bin.ptr(j);
    int n = 0;

    for (int i = 0; i < (bin.cols - len); i++) {
      if (p[i] >= 128) {
        if ((++n) >= len) {
          num += 1;
          j += len / 2;
          break;
        }
      } else {
        n = 0;
      }
    }

    if (num >= 2) {
      return true;
    }
  }

  return false;
}

bool CHelloFisherDlg::DropTemplate(cv::Mat mat, cv::Mat tmp, double thr) {
  cv::Mat ret;
  double val;

  cv::matchTemplate(mat, tmp, ret, cv::TM_CCORR_NORMED);
  cv::minMaxLoc(ret, NULL, &val, NULL, NULL);

  if (val >= thr) {
    return true;
  }

  return false;
}
