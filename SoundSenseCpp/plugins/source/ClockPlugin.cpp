#include "../../src/plugins/SoundSenseSDK/SoundSenseSDK.hpp"
#include <chrono>
#include <ctime>
#include <iostream>

class ClockPlugin : public IPlugin {
public:
  const char *GetName() const override { return "ClockPlugin"; }

  void Initialize() override {
    std::cout << "ClockPlugin Initialized: Replacing visualizer with a clock.\n";
  }

  bool OverridesBase() const override {
    return true; // Replace mode
  }

  void SetPixel(std::vector<int> &buffer, int x, int y, bool on) {
    if (x < 0 || x >= 128 || y < 0 || y >= 40)
      return;
    int byteIndex = (y * 16) + (x / 8);
    int bitIndex = 7 - (x % 8);
    if (on)
      buffer[byteIndex] |= (1 << bitIndex);
    else
      buffer[byteIndex] &= ~(1 << bitIndex);
  }

  void DrawChar(std::vector<int> &buffer, int x, int y, char c) {
    // Very simple 3x5 font for demo purposes
    // A simple representation of numbers 0-9 and colon
    const bool font[11][15] = {
        {1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1}, // 0
        {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}, // 1
        {1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1}, // 2
        {1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1}, // 3
        {1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1}, // 4
        {1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1}, // 5
        {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1}, // 6
        {1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1}, // 7
        {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1}, // 8
        {1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1}, // 9
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}  // :
    };

    int idx = 0;
    if (c >= '0' && c <= '9')
      idx = c - '0';
    else if (c == ':')
      idx = 10;
    else
      return;

    for (int dy = 0; dy < 5; dy++) {
      for (int dx = 0; dx < 3; dx++) {
        if (font[idx][dy * 3 + dx]) {
          SetPixel(buffer, x + dx, y + dy, true);
        }
      }
    }
  }

  void Process(const std::vector<float> &audioBands,
               std::vector<int> &bitmap) override {
    // Clear bitmap just in case
    for (size_t i = 0; i < bitmap.size(); i++)
      bitmap[i] = 0;

    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    struct tm *timeinfo = std::localtime(&time);

    char timeStr[16];
    std::strftime(timeStr, sizeof(timeStr), "%H:%M:%S", timeinfo);

    // Draw time string in the center (scale it up a bit by drawing multiple
    // pixels per virtual pixel for better visibility, or just draw as is)
    int startX = 40;
    int startY = 15;
    for (int i = 0; timeStr[i] != '\0'; i++) {
      DrawChar(bitmap, startX + i * 5, startY, timeStr[i]);
    }
  }
};

extern "C" __declspec(dllexport) IPlugin *CreatePlugin() {
  return new ClockPlugin();
}
