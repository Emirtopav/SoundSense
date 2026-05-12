#pragma once
#include <vector>
#include <string>
#include <chrono>

enum class AudioEventType {
    BassPeak,
    MidPeak,
    TreblePeak,
    VolumeSpike
};

struct AudioEvent {
    AudioEventType type;
    float intensity;
    std::string name;
};

class TriggerDetector {
public:
    struct Threshold {
        float value = 0.8f;
        float cooldownMs = 100.0f;
        std::chrono::steady_clock::time_point lastTriggered;
    };

    TriggerDetector();
    std::vector<AudioEvent> Detect(const std::vector<float>& bands);

private:
    Threshold m_bass;
    Threshold m_mid;
    Threshold m_treble;
};
