#pragma once
#include "Widget.hpp"

class RectWidget : public Widget {
public:
  int width = 20;
  int height = 10;
  bool filled = true;

  RectWidget() : Widget("Rectangle") {}

  std::unique_ptr<Widget> clone() const override {
      return std::make_unique<RectWidget>(*this);
  }

  json toJson() const override {
      json j;
      serializeBase(j);
      j["type"] = "rectangle";
      j["width"] = width;
      j["height"] = height;
      j["filled"] = filled;
      return j;
  }

  void fromJson(const json &j) override {
      deserializeBase(j);
      width = j.value("width", 20);
      height = j.value("height", 10);
      filled = j.value("filled", true);
  }

  void Render(const std::vector<float> &audioBands,
              std::vector<int> &bitmap) override {
    float amp = GetReactiveValue(audioBands);
    
    int drawX = x;
    int drawY = y;
    int drawW = width;
    int drawH = height;
    bool shouldDraw = true;

    // Apply reactivity
    if (reactive) {
        switch (reactiveTarget) {
            case 1: drawY -= (int)(amp * 20); break; // Move Up
            case 2: drawX += (int)(amp * 20); break; // Move Right
            case 3: drawH += (int)(amp * 30); break; // Scale Height
            case 4: drawW += (int)(amp * 50); break; // Scale Width
            case 5: if (amp < 0.1f) shouldDraw = false; break; // Pulse Visibility
        }
    }

    if (!shouldDraw || drawW <= 0 || drawH <= 0) return;

    for (int py = 0; py < drawH; py++) {
      for (int px = 0; px < drawW; px++) {
        bool isBorder = (px == 0 || px == drawW - 1 || py == 0 || py == drawH - 1);
        if (filled || isBorder) {
          SetPixel(bitmap, drawX + px, drawY + py, true);
        }
      }
    }
  }
};
