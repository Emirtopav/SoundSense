#pragma once
#include "Widget.hpp"
#include <string>

class TextWidget : public Widget {
public:
  std::string text = "Hello World";
  int spacing = 1;

  TextWidget() : Widget("Static Text") {}

  std::unique_ptr<Widget> clone() const override {
      return std::make_unique<TextWidget>(*this);
  }

  json toJson() const override {
      json j;
      serializeBase(j);
      j["type"] = "text";
      j["text"] = text;
      j["spacing"] = spacing;
      return j;
  }

  void fromJson(const json &j) override {
      deserializeBase(j);
      text = j.value("text", "Hello");
      spacing = j.value("spacing", 1);
  }

  // A very minimal 3x5 font (only uppercase A-Z and Space for demo)
  // In a real application, a full font bitmap would be included.
  void DrawChar(std::vector<int> &buffer, int px, int py, char c) {
    if (c == ' ') return; // Space is empty
    
    // Very basic font mappings for A, B, C, D, E, H, L, O, R, W
    // Just a placeholder to show the concept
    const bool font_A[15] = {0,1,0, 1,0,1, 1,1,1, 1,0,1, 1,0,1};
    const bool font_E[15] = {1,1,1, 1,0,0, 1,1,0, 1,0,0, 1,1,1};
    const bool font_H[15] = {1,0,1, 1,0,1, 1,1,1, 1,0,1, 1,0,1};
    const bool font_L[15] = {1,0,0, 1,0,0, 1,0,0, 1,0,0, 1,1,1};
    const bool font_O[15] = {0,1,0, 1,0,1, 1,0,1, 1,0,1, 0,1,0};
    const bool font_R[15] = {1,1,0, 1,0,1, 1,1,0, 1,0,1, 1,0,1};
    const bool font_W[15] = {1,0,1, 1,0,1, 1,0,1, 1,1,1, 0,1,0};
    const bool font_D[15] = {1,1,0, 1,0,1, 1,0,1, 1,0,1, 1,1,0};
    const bool font_unk[15]= {1,1,1, 1,1,1, 1,1,1, 1,1,1, 1,1,1};

    const bool* f = font_unk;
    if (c == 'A' || c == 'a') f = font_A;
    if (c == 'E' || c == 'e') f = font_E;
    if (c == 'H' || c == 'h') f = font_H;
    if (c == 'L' || c == 'l') f = font_L;
    if (c == 'O' || c == 'o') f = font_O;
    if (c == 'R' || c == 'r') f = font_R;
    if (c == 'W' || c == 'w') f = font_W;
    if (c == 'D' || c == 'd') f = font_D;

    for (int dy = 0; dy < 5; dy++) {
      for (int dx = 0; dx < 3; dx++) {
        if (f[dy * 3 + dx]) {
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

    for (size_t i = 0; i < text.length(); i++) {
      DrawChar(bitmap, drawX + i * (3 + spacing), drawY, text[i]);
    }
  }


};
