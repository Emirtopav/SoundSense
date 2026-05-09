#include "GameSenseClient.hpp"
#include "AudioAnalyzer.hpp"
#include "SerialPort.hpp"
#include <chrono>
#include <thread>
#include <atomic>
#include <signal.h>
#include <vector>
#include <commctrl.h>
#include <string>
#include <shellapi.h>
#include <fstream>
#include <mutex>
#include <sstream>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_SHOW 3001
#define ID_TRAY_EXIT 3002

std::atomic<bool> running(true);
VisualizerSettings g_Settings;
HWND g_hWnd = NULL;
HWND g_hLabels[6];
HWND g_hComPortCombo;
HWND g_hConnectBtn;
NOTIFYICONDATA g_nid = {0};
std::vector<int> g_CurrentBitmap(640, 0);
std::mutex g_BitmapMutex;
bool g_IsDarkMode = true;
HBRUSH g_hbrBkg = NULL;
float g_PeakHeights[64] = {0};
SerialPort g_Serial;
std::string g_ComPortName = "COM3";

void SaveSettings() {
    std::ofstream f("settings.bin", std::ios::binary);
    if (f.is_open()) {
        f.write((char*)&g_Settings, sizeof(VisualizerSettings));
        size_t len = g_ComPortName.length(); f.write((char*)&len, sizeof(len));
        f.write(g_ComPortName.c_str(), len);
    }
}

void LoadSettings() {
    std::ifstream f("settings.bin", std::ios::binary);
    if (f.is_open()) {
        f.read((char*)&g_Settings, sizeof(VisualizerSettings));
        size_t len; if (f.read((char*)&len, sizeof(len))) {
            char* buf = new char[len + 1]; f.read(buf, len); buf[len] = '\0';
            g_ComPortName = buf; delete[] buf;
        }
    } else {
        g_Settings.globalSens = 1.0f; g_Settings.bassGain = 1.0f; g_Settings.midGain = 1.0f; 
        g_Settings.trebleGain = 1.0f; g_Settings.falloff = 0.85f; g_Settings.smoothing = 0.5f; 
        g_Settings.mode = 0; g_Settings.showPeaks = true;
    }
}

bool IsWindowsDarkMode() {
    HKEY hKey; DWORD value = 1, size = sizeof(DWORD);
    if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueEx(hKey, "AppsUseLightTheme", NULL, NULL, (LPBYTE)&value, &size); RegCloseKey(hKey);
    }
    return value == 0;
}

void SetPixel(std::vector<int>& buffer, int x, int y, bool on) {
    if (x < 0 || x >= 128 || y < 0 || y >= 40) return;
    int byteIndex = (y * 16) + (x / 8); int bitIndex = 7 - (x % 8);
    if (on) buffer[byteIndex] |= (1 << bitIndex); else buffer[byteIndex] &= ~(1 << bitIndex);
}

std::vector<int> RenderBitmap(const std::vector<float>& data, const VisualizerSettings& settings) {
    std::vector<int> buffer(640, 0);
    for (int b = 0; b < 64; b++) {
        int height = (int)(data[b] * 120); if (height > 40) height = 40;
        if (settings.showPeaks) {
            if (height > g_PeakHeights[b]) g_PeakHeights[b] = (float)height;
            else g_PeakHeights[b] -= 0.4f; if (g_PeakHeights[b] < 0) g_PeakHeights[b] = 0;
        }
        int peakY = 39 - (int)g_PeakHeights[b]; if (peakY < 0) peakY = 0;
        if (settings.mode == 0) {
            int x = b * 2; for (int y = 0; y < height; y++) SetPixel(buffer, x, 39 - y, true);
            if (settings.showPeaks) SetPixel(buffer, x, peakY, true);
        } else if (settings.mode == 1) {
            int center = 32, offset = b / 2, x = (b % 2 == 0) ? (center + offset) : (center - offset - 1);
            int realX = x * 2; for (int y = 0; y < height; y++) SetPixel(buffer, realX, 39 - y, true);
            if (settings.showPeaks) SetPixel(buffer, realX, peakY, true);
        } else if (settings.mode == 2) {
            int x = b * 2; if (height > 0) SetPixel(buffer, x, 39 - (height - 1), true);
        }
    }
    return buffer;
}

void UpdateLabel(int id, float val) {
    char buf[32]; sprintf(buf, "%.2f", val); SetWindowText(g_hLabels[id - 101], buf);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_SIZE: if (wParam == SIZE_MINIMIZED) ShowWindow(hWnd, SW_HIDE); break;
        case WM_TRAYICON:
            if (lParam == WM_LBUTTONDBLCLK) { ShowWindow(hWnd, SW_SHOW); ShowWindow(hWnd, SW_RESTORE); SetForegroundWindow(hWnd); }
            else if (lParam == WM_RBUTTONUP) {
                POINT pt; GetCursorPos(&pt); HMENU hMenu = CreatePopupMenu();
                AppendMenu(hMenu, MF_STRING, ID_TRAY_SHOW, "Show Settings");
                AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
                AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, "Exit");
                SetForegroundWindow(hWnd); TrackPopupMenu(hMenu, TPM_BOTTOMALIGN, pt.x, pt.y, 0, hWnd, NULL); DestroyMenu(hMenu);
            }
            break;
        case WM_HSCROLL: {
            HWND hSlider = (HWND)lParam; int pos = SendMessage(hSlider, TBM_GETPOS, 0, 0), id = GetWindowLong(hSlider, GWL_ID);
            if (id == 101) g_Settings.globalSens = pos / 10.0f;
            if (id == 102) g_Settings.bassGain = pos / 10.0f;
            if (id == 103) g_Settings.midGain = pos / 10.0f;
            if (id == 104) g_Settings.trebleGain = pos / 10.0f;
            if (id == 105) g_Settings.falloff = pos / 100.0f;
            if (id == 106) g_Settings.smoothing = pos / 100.0f;
            if (id >= 101 && id <= 106) { UpdateLabel(id, (id <= 104) ? (pos / 10.0f) : (pos / 100.0f)); SaveSettings(); }
            break;
        }
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_TRAY_SHOW) { ShowWindow(hWnd, SW_SHOW); ShowWindow(hWnd, SW_RESTORE); SetForegroundWindow(hWnd); }
            else if (LOWORD(wParam) == ID_TRAY_EXIT) { running = false; PostQuitMessage(0); }
            else if (LOWORD(wParam) >= 200 && LOWORD(wParam) <= 202) { 
                g_Settings.mode = LOWORD(wParam) - 200; 
                if (g_Serial.IsConnected()) g_Serial.Write("mode," + std::to_string(g_Settings.mode) + "\n");
                SaveSettings(); 
            }
            else if (LOWORD(wParam) == 400) { 
                g_Settings.showPeaks = IsDlgButtonChecked(hWnd, 400); 
                if (g_Serial.IsConnected()) g_Serial.Write("peaks," + std::to_string(g_Settings.showPeaks) + "\n");
                SaveSettings(); 
            }
            else if (LOWORD(wParam) == 501) { // Connect Serial
                int idx = SendMessage(g_hComPortCombo, CB_GETCURSEL, 0, 0);
                if (idx != CB_ERR) {
                    char buf[32]; SendMessage(g_hComPortCombo, CB_GETLBTEXT, idx, (LPARAM)buf);
                    g_ComPortName = buf;
                    if (g_Serial.Open(g_ComPortName)) { 
                        SetWindowText(g_hConnectBtn, "Connected!"); 
                        EnableWindow(g_hConnectBtn, FALSE); 
                        EnableWindow(g_hComPortCombo, FALSE);
                        g_Serial.Write("mode," + std::to_string(g_Settings.mode) + "\n");
                        g_Serial.Write("peaks," + std::to_string(g_Settings.showPeaks) + "\n");
                        SaveSettings(); 
                    } else MessageBox(hWnd, "Could not open COM port!", "Error", MB_ICONERROR);
                } else MessageBox(hWnd, "Please select a COM port!", "Error", MB_ICONERROR);
            }
            if (HIWORD(wParam) == CBN_DROPDOWN && (HWND)lParam == g_hComPortCombo) {
                SendMessage(g_hComPortCombo, CB_RESETCONTENT, 0, 0);
                auto ports = SerialPort::GetAvailablePorts();
                for (const auto& p : ports) SendMessage(g_hComPortCombo, CB_ADDSTRING, 0, (LPARAM)p.c_str());
            }
            break;
        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC)wParam; SetTextColor(hdcStatic, g_IsDarkMode ? RGB(240, 240, 240) : RGB(20, 20, 20));
            SetBkMode(hdcStatic, TRANSPARENT); return (LRESULT)g_hbrBkg;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps; HDC hdc = BeginPaint(hWnd, &ps);
            RECT r = {20, 30, 20 + 256, 30 + 80}; FillRect(hdc, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));
            std::lock_guard<std::mutex> lock(g_BitmapMutex); HBRUSH hWhite = CreateSolidBrush(RGB(255, 255, 255));
            for(int y=0; y<40; y++) {
                for(int x=0; x<128; x++) {
                    int byteIdx = (y*16) + (x/8), bitIdx = 7-(x%8);
                    if (g_CurrentBitmap[byteIdx] & (1 << bitIdx)) {
                        RECT pixel = {20 + x*2, 30 + y*2, 20 + x*2 + 2, 30 + y*2 + 2}; FillRect(hdc, &pixel, hWhite);
                    }
                }
            }
            DeleteObject(hWhite); EndPaint(hWnd, &ps); break;
        }
        case WM_TIMER: InvalidateRect(hWnd, NULL, FALSE); break;
        case WM_DESTROY: running = false; PostQuitMessage(0); break;
        default: return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void AudioThread() {
    try {
        GameSenseClient client; client.RegisterGame(); client.RegisterEvent(); client.BindEvent();
        AudioAnalyzer analyzer; int frameValue = 0; 
        auto lastHeartbeat = std::chrono::steady_clock::now();
        auto lastSerial = std::chrono::steady_clock::now();
        while (running) {
            analyzer.Update(); auto bands = analyzer.Get64Bands(g_Settings);
            auto bitmap = RenderBitmap(bands, g_Settings);
            { std::lock_guard<std::mutex> lock(g_BitmapMutex); g_CurrentBitmap = bitmap; }
            client.SendFrame(bitmap, frameValue++);
            
            auto now = std::chrono::steady_clock::now();
            if (g_Serial.IsConnected() && std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSerial).count() >= 30) {
                std::stringstream ss;
                for (int i = 0; i < 64; i++) {
                    int v = (int)(bands[i] * 192.0f);
                    if (v > 64) v = 64;
                    if (v < 0) v = 0;
                    ss << v << (i == 63 ? "" : ",");
                }
                ss << "\n"; g_Serial.Write(ss.str());
                lastSerial = now;
            }
            
            if (std::chrono::duration_cast<std::chrono::seconds>(now - lastHeartbeat).count() >= 10) {
                client.Heartbeat(); lastHeartbeat = now;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5)); 
        }
    } catch (...) {}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    LoadSettings(); g_IsDarkMode = IsWindowsDarkMode();
    g_hbrBkg = CreateSolidBrush(g_IsDarkMode ? RGB(30, 30, 30) : RGB(245, 245, 245));
    InitCommonControls(); WNDCLASS wc = {0}; wc.lpfnWndProc = WndProc; wc.hInstance = hInstance;
    wc.hbrBackground = g_hbrBkg; wc.lpszClassName = "VisualizerGUI"; RegisterClass(&wc);
    g_hWnd = CreateWindow("VisualizerGUI", "SteelSeries + ESP32 Visualizer", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 450, 750, NULL, NULL, hInstance, NULL);
    SetTimer(g_hWnd, 1, 30, NULL);
    g_nid.cbSize = sizeof(NOTIFYICONDATA); g_nid.hWnd = g_hWnd; g_nid.uID = 1;
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.hIcon = LoadIcon(NULL, IDI_APPLICATION); strcpy(g_nid.szTip, "SteelSeries + ESP32 Visualizer");
    Shell_NotifyIcon(NIM_ADD, &g_nid);
    auto createSlider = [&](int id, int y, const char* label, int def, int range) {
        CreateWindow("STATIC", label, WS_CHILD | WS_VISIBLE, 20, y, 120, 20, g_hWnd, NULL, hInstance, NULL);
        HWND h = CreateWindow(TRACKBAR_CLASS, "", WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS, 150, y, 200, 30, g_hWnd, (HMENU)id, hInstance, NULL);
        SendMessage(h, TBM_SETRANGE, TRUE, MAKELONG(0, range)); SendMessage(h, TBM_SETPOS, TRUE, def);
        g_hLabels[id-101] = CreateWindow("STATIC", "0.00", WS_CHILD | WS_VISIBLE, 360, y, 50, 20, g_hWnd, NULL, hInstance, NULL);
    };
    int startY = 130;
    createSlider(101, startY, "Global Sensitivity", (int)(g_Settings.globalSens*10), 100); UpdateLabel(101, g_Settings.globalSens);
    createSlider(102, startY+50, "Bass Gain", (int)(g_Settings.bassGain*10), 100); UpdateLabel(102, g_Settings.bassGain);
    createSlider(103, startY+100, "Mid Gain", (int)(g_Settings.midGain*10), 100); UpdateLabel(103, g_Settings.midGain);
    createSlider(104, startY+150, "Treble Gain", (int)(g_Settings.trebleGain*10), 100); UpdateLabel(104, g_Settings.trebleGain);
    createSlider(105, startY+200, "Falloff Speed", (int)(g_Settings.falloff*100), 100); UpdateLabel(105, g_Settings.falloff);
    createSlider(106, startY+250, "Smoothness", (int)(g_Settings.smoothing*100), 100); UpdateLabel(106, g_Settings.smoothing);
    CreateWindow("BUTTON", "Classic Bars", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 20, startY+300, 150, 30, g_hWnd, (HMENU)200, hInstance, NULL);
    CreateWindow("BUTTON", "Mirror Mode", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 20, startY+340, 150, 30, g_hWnd, (HMENU)201, hInstance, NULL);
    CreateWindow("BUTTON", "Dot Mode", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 180, startY+300, 150, 30, g_hWnd, (HMENU)202, hInstance, NULL);
    CheckRadioButton(g_hWnd, 200, 202, 200 + g_Settings.mode);
    HWND hPeak = CreateWindow("BUTTON", "Show Peak Dots", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 20, startY+380, 200, 30, g_hWnd, (HMENU)400, hInstance, NULL);
    if (g_Settings.showPeaks) CheckDlgButton(g_hWnd, 400, BST_CHECKED);
    CreateWindow("STATIC", "ESP32 Sync (COM Port):", WS_CHILD | WS_VISIBLE, 20, startY+430, 180, 20, g_hWnd, NULL, hInstance, NULL);
    g_hComPortCombo = CreateWindow("COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL, 200, startY+430, 80, 200, g_hWnd, NULL, hInstance, NULL);
    auto ports = SerialPort::GetAvailablePorts();
    for (const auto& p : ports) SendMessage(g_hComPortCombo, CB_ADDSTRING, 0, (LPARAM)p.c_str());
    int selIdx = SendMessage(g_hComPortCombo, CB_FINDSTRINGEXACT, -1, (LPARAM)g_ComPortName.c_str());
    if (selIdx != CB_ERR) SendMessage(g_hComPortCombo, CB_SETCURSEL, selIdx, 0);
    else if (!ports.empty()) SendMessage(g_hComPortCombo, CB_SETCURSEL, 0, 0);
    g_hConnectBtn = CreateWindow("BUTTON", "Connect", WS_CHILD | WS_VISIBLE, 300, startY+430, 100, 25, g_hWnd, (HMENU)501, hInstance, NULL);
    std::thread audio(AudioThread); MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    Shell_NotifyIcon(NIM_DELETE, &g_nid); running = false; audio.join();
    return 0;
}
