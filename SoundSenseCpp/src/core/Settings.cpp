#include "Settings.hpp"
#include "Globals.hpp"
#include <fstream>

void SaveSettings() {
  std::lock_guard<std::recursive_mutex> lock(g_SettingsMutex);
  std::ofstream f("settings.bin", std::ios::binary);
  if (f.is_open()) {
    f.write((char *)&g_Settings, sizeof(VisualizerSettings));
    size_t len = g_ComPortName.length();
    f.write((char *)&len, sizeof(len));
    f.write(g_ComPortName.c_str(), len);
  }
}

void LoadSettings() {
  std::lock_guard<std::recursive_mutex> lock(g_SettingsMutex);
  std::ifstream f("settings.bin", std::ios::binary);
  if (f.is_open()) {
    f.read((char *)&g_Settings, sizeof(VisualizerSettings));
    size_t len;
    if (f.read((char *)&len, sizeof(len)) && len > 0 && len < 1024) {
      try {
        char *buf = new char[len + 1];
        if (f.read(buf, len)) {
            buf[len] = '\0';
            g_ComPortName = buf;
        }
        delete[] buf;
      } catch (...) {
        g_ComPortName = "";
      }
    }
  } else {
    g_Settings.globalSens = 1.0f;
    g_Settings.bassGain = 1.0f;
    g_Settings.midGain = 1.0f;
    g_Settings.trebleGain = 1.0f;
    g_Settings.falloff = 0.85f;
    g_Settings.smoothing = 0.5f;
    g_Settings.mode = 0;
    g_Settings.showPeaks = true;
    g_Settings.brightness = 128;
    g_Settings.rotation = 0;
  }
}
