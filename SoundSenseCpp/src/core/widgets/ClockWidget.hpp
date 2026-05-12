#pragma once
#include "Widget.hpp"
#include <chrono>
#include <ctime>

class ClockWidget : public Widget {
public:
  bool showSeconds = true;

  ClockWidget() : Widget("Digital Clock") {}

  std::unique_ptr<Widget> clone() const override {
      return std::make_unique<ClockWidget>(*this);
  }

  json toJson() const override {
      json j;
      serializeBase(j);
      j["type"] = "clock";
      j["showSeconds"] = showSeconds;
      return j;
  }

  void fromJson(const json &j) override {
      deserializeBase(j);
      showSeconds = j.value("showSeconds", true);
  }

  void DrawChar(std::vector<int> &buffer, int px, int py, char c) {
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
          SetPixel(buffer, px + dx, py + dy, true);
        }
      }
    }
  }

  void Render(const std::vector<float> &audioBands,
              std::vector<int> &bitmap) override {
    float amp = GetReactiveValue(audioBands);
    
    int drawX = x;
    int drawY = y;
    bool shouldDraw = true;

    // Apply reactivity
    if (reactive) {
        switch (reactiveTarget) {
            case 1: drawY -= (int)(amp * 15); break; // Move Up
            case 2: drawX += (int)(amp * 30); break; // Move Right
            case 5: if (amp < 0.1f) shouldDraw = false; break; // Pulse Visibility
        }
    }

    if (!shouldDraw) return;

    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    struct tm *timeinfo = std::localtime(&time);

    char timeStr[16];
    if (showSeconds)
      std::strftime(timeStr, sizeof(timeStr), "%H:%M:%S", timeinfo);
    else
      std::strftime(timeStr, sizeof(timeStr), "%H:%M", timeinfo);

    for (int i = 0; timeStr[i] != '\0'; i++) {
      DrawChar(bitmap, drawX + i * 5, drawY, timeStr[i]);
    }
  }


};
