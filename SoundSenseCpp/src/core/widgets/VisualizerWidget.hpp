#pragma once
#include "Widget.hpp"
#include "Globals.hpp" // For settings


class VisualizerWidget : public Widget {
public:
  VisualizerWidget() : Widget("Visualizer") {}

  std::unique_ptr<Widget> clone() const override {
      return std::make_unique<VisualizerWidget>(*this);
  }

  json toJson() const override {
      json j;
      serializeBase(j);
      j["type"] = "visualizer";
      return j;
  }

  void fromJson(const json &j) override {
      deserializeBase(j);
  }

  void Render(const std::vector<float> &audioBands,
              std::vector<int> &bitmap) override {
    for (int b = 0; b < 64; b++) {
      int height = (int)(audioBands[b] * 120);
      if (height > 40)
        height = 40;
      if (g_Settings.showPeaks) {
        if (height > g_PeakHeights[b])
          g_PeakHeights[b] = (float)height;
        else
          g_PeakHeights[b] -= 0.4f;
        if (g_PeakHeights[b] < 0)
          g_PeakHeights[b] = 0;
      }
      int peakY = 39 - (int)g_PeakHeights[b];
      if (peakY < 0)
        peakY = 0;

      if (g_Settings.mode == 0) {
        int px = x + (b * 2);
        for (int py = 0; py < height; py++)
          SetPixel(bitmap, px, y + 39 - py, true);
        if (g_Settings.showPeaks)
          SetPixel(bitmap, px, y + peakY, true);
      } else if (g_Settings.mode == 1) {
        int center = 32, offset = b / 2;
        int px = (b % 2 == 0) ? (center + offset) : (center - offset - 1);
        int realX = x + (px * 2);
        for (int py = 0; py < height; py++)
          SetPixel(bitmap, realX, y + 39 - py, true);
        if (g_Settings.showPeaks)
          SetPixel(bitmap, realX, y + peakY, true);
      } else if (g_Settings.mode == 2) {
        int px = x + (b * 2);
        if (height > 0)
          SetPixel(bitmap, px, y + 39 - (height - 1), true);
      }
    }
  }


};
