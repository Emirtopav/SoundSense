#pragma once
#include "Widget.hpp"
#include <cmath>

class WaveformWidget : public Widget {
public:
  int width = 64;
  int height = 20;

  WaveformWidget() : Widget("Waveform") {}

  std::unique_ptr<Widget> clone() const override {
      return std::make_unique<WaveformWidget>(*this);
  }

  json toJson() const override {
      json j;
      serializeBase(j);
      j["type"] = "waveform";
      j["width"] = width;
      j["height"] = height;
      return j;
  }

  void fromJson(const json &j) override {
      deserializeBase(j);
      width = j.value("width", 64);
      height = j.value("height", 20);
  }

  void Render(const std::vector<float> &audioBands,
              std::vector<int> &bitmap) override {
    // Simulate a time-domain waveform using the frequency bands for now.
    // In Phase 3, this would read actual raw PCM time-domain data.
    int midY = height / 2;
    for (int px = 0; px < width; px++) {
      int bandIdx = (px * 64) / width;
      float amplitude = audioBands[bandIdx];
      // Draw a vertical line from center outward
      int h = (int)(amplitude * midY);
      for (int dy = -h; dy <= h; dy++) {
        SetPixel(bitmap, x + px, y + midY + dy, true);
      }
    }
  }
};
