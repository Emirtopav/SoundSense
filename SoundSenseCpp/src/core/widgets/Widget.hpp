#pragma once
#include <string>
#include <vector>
#include <memory>
#include "vendor/json/json.hpp"

using json = nlohmann::json;

class Widget {
public:
  std::string name;
  int x = 0;
  int y = 0;
  bool visible = true;
  bool locked = false;

  // Reactivity settings
  bool reactive = false;
  int bandStart = 0;
  int bandEnd = 11;
  float sensitivity = 1.0f;
  float lerpSpeed = 0.2f;
  float lastValue = 0.0f;
  int reactiveTarget = 0; // 0:None, 1:Y, 2:X, 3:H, 4:W, 5:Vis

  Widget(const std::string &n) : name(n) {}
  virtual ~Widget() = default;

  virtual std::unique_ptr<Widget> clone() const = 0;

  // Helper function to set a bit in the 128x40 bitmap
  void SetPixel(std::vector<int> &buffer, int px, int py, bool on) {
    if (px < 0 || px >= 128 || py < 0 || py >= 40)
      return;
    int byteIndex = (py * 16) + (px / 8);
    int bitIndex = 7 - (px % 8);
    if (on)
      buffer[byteIndex] |= (1 << bitIndex);
    else
      buffer[byteIndex] &= ~(1 << bitIndex);
  }

  // Helper to calculate reactive amplitude
  float GetReactiveValue(const std::vector<float> &audioBands) {
    if (!reactive || audioBands.empty()) return 0.0f;
    
    float sum = 0;
    int count = 0;
    for (int i = bandStart; i <= bandEnd && i < (int)audioBands.size(); i++) {
        sum += audioBands[i];
        count++;
    }
    
    float avg = (count > 0) ? (sum / count) : 0.0f;
    float target = avg * sensitivity;
    
    // Simple Lerp: last + (target - last) * speed
    lastValue = lastValue + (target - lastValue) * lerpSpeed;
    return lastValue;
  }

  // Common JSON serialization
  virtual void serializeBase(json &j) const {
      j["name"] = name;
      j["x"] = x;
      j["y"] = y;
      j["visible"] = visible;
      j["locked"] = locked;
      j["reactive"] = reactive;
      j["bandStart"] = bandStart;
      j["bandEnd"] = bandEnd;
      j["sensitivity"] = sensitivity;
      j["lerpSpeed"] = lerpSpeed;
      j["reactiveTarget"] = reactiveTarget;
  }

  virtual void deserializeBase(const json &j) {
      name = j.value("name", "Widget");
      x = j.value("x", 0);
      y = j.value("y", 0);
      visible = j.value("visible", true);
      locked = j.value("locked", false);
      reactive = j.value("reactive", false);
      bandStart = j.value("bandStart", 0);
      bandEnd = j.value("bandEnd", 11);
      sensitivity = j.value("sensitivity", 1.0f);
      lerpSpeed = j.value("lerpSpeed", 0.2f);
      reactiveTarget = j.value("reactiveTarget", 0);
  }

  // Pure virtual functions
  virtual void Render(const std::vector<float> &audioBands,
                      std::vector<int> &bitmap) = 0;

  virtual json toJson() const = 0;
  virtual void fromJson(const json &j) = 0;
};
