#include "TriggerSystem.hpp"
#include <numeric>
#include <algorithm>

TriggerDetector::TriggerDetector() {}

std::vector<AudioEvent> TriggerDetector::Detect(const std::vector<float>& bands) {
    std::vector<AudioEvent> events;
    auto now = std::chrono::steady_clock::now();

    if (bands.empty()) return events;

    // Bass Peak (First 8 bands)
    float bassAvg = std::accumulate(bands.begin(), bands.begin() + 8, 0.0f) / 8.0f;
    if (bassAvg > m_bass.value) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_bass.lastTriggered).count();
        if (elapsed > m_bass.cooldownMs) {
            events.push_back({AudioEventType::BassPeak, bassAvg, "BassDrop"});
            m_bass.lastTriggered = now;
        }
    }

    // Mid Peak (Bands 20-40)
    float midAvg = std::accumulate(bands.begin() + 20, bands.begin() + 40, 0.0f) / 20.0f;
    if (midAvg > m_mid.value) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_mid.lastTriggered).count();
        if (elapsed > m_mid.cooldownMs) {
            events.push_back({AudioEventType::MidPeak, midAvg, "MidSpike"});
            m_mid.lastTriggered = now;
        }
    }

    // Treble Peak (Last 10 bands)
    float trebleAvg = std::accumulate(bands.end() - 10, bands.end(), 0.0f) / 10.0f;
    if (trebleAvg > m_treble.value) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_treble.lastTriggered).count();
        if (elapsed > m_treble.cooldownMs) {
            events.push_back({AudioEventType::TreblePeak, trebleAvg, "TrebleHit"});
            m_treble.lastTriggered = now;
        }
    }

    return events;
}
