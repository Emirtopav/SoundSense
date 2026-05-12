#pragma once
#include "Widget.hpp"

// Note: A true ImageWidget would require stb_image or Qt's QImage to load actual files.
// For Phase 2, we simulate it with a generic placeholder pattern to establish the architecture.
class ImageWidget : public Widget {
public:
  int width = 16;
  int height = 16;
  
  ImageWidget() : Widget("Image") {}

  std::unique_ptr<Widget> clone() const override {
      return std::make_unique<ImageWidget>(*this);
  }

  json toJson() const override {
      json j;
      serializeBase(j);
      j["type"] = "image";
      j["width"] = width;
      j["height"] = height;
      return j;
  }

  void fromJson(const json &j) override {
      deserializeBase(j);
      width = j.value("width", 16);
      height = j.value("height", 16);
  }

  void Render(const std::vector<float> &audioBands,
              std::vector<int> &bitmap) override {
    // Checkerboard placeholder
    for (int py = 0; py < height; py++) {
      for (int px = 0; px < width; px++) {
        if ((px + py) % 2 == 0) {
          SetPixel(bitmap, x + px, y + py, true);
        }
      }
    }
  }
};
