# SoundSense Plugin SDK

Welcome to the SoundSense SDK! This kit allows you to easily create dynamic plugins (custom visualizers, effects, clocks) for the SoundSense Application.

## Getting Started

1. Create a new `.cpp` file in this directory (e.g., `MyPlugin.cpp`).
2. Include the `SoundSenseSDK.hpp` header.
3. Inherit from the `IPlugin` class and implement the required methods.
4. Export the `CreatePlugin()` function.

## Example

```cpp
#include "SoundSenseSDK.hpp"

class MyPlugin : public IPlugin {
public:
  const char *GetName() const override { return "My Custom Plugin"; }
  void Initialize() override {}
  bool OverridesBase() const override { return false; } // false = layer on top, true = replace
  
  void Process(const std::vector<float> &audioBands, std::vector<int> &bitmap) override {
    // Invert the first pixel if bass hits hard
    if (audioBands[0] > 0.8f) {
        bitmap[0] = 0xFF;
    }
  }
};

extern "C" __declspec(dllexport) IPlugin *CreatePlugin() {
  return new MyPlugin();
}
```

## How to Build
Simply drag and drop your `.cpp` file onto the `build_plugin.bat` script! It will compile it into a `.dll` and place it straight into the application's `plugins/` directory. Next time you start SoundSense, your plugin will be loaded automatically!
