#include "../../src/plugins/SoundSenseSDK/SoundSenseSDK.hpp"
#include <iostream>

class InvertPlugin : public IPlugin {
public:
  const char *GetName() const override { return "Invert Colors"; }
  const char *GetDescription() const override { return "Inverts OLED colors based on bass intensity."; }
  const char *GetVersion() const override { return "1.1.0"; }
  const char *GetAuthor() const override { return "SoundSense Official"; }

  void Initialize() override {
    std::cout << "InvertPlugin Initialized: Will invert colors on loud bass!\n";
  }

  bool OverridesBase() const override {
    return false; // Layer mode
  }

  void Process(const std::vector<float> &audioBands,
               std::vector<int> &bitmap) override {
    // Check bass intensity (sum of first 4 bands)
    float bassSum = 0;
    for (int i = 0; i < 4 && i < audioBands.size(); i++) {
      bassSum += audioBands[i];
    }

    // If bass is strong enough, invert the entire bitmap
    if (bassSum > 1.2f) {
      for (size_t i = 0; i < bitmap.size(); i++) {
        bitmap[i] = ~bitmap[i] & 0xFF; // Invert and keep to 8 bits
      }
    }
  }
};

extern "C" __declspec(dllexport) IPlugin *CreatePlugin() {
  return new InvertPlugin();
}
