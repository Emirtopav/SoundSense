#pragma once
#include <cmath>
#include <chrono>

class LFO {
public:
    enum Type { Sine, Square, Saw };
    
    Type type = Sine;
    float frequency = 1.0f; // Hz
    float amplitude = 0.0f; // Default 0 (no modulation)
    float offset = 0.0f;
    
    float GetValue() {
        if (amplitude == 0.0f) return 0.0f;

        auto now = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        float t = (float)(ms % 1000000) / 1000.0f;
        
        float val = 0.0f;
        float phase = 2.0f * 3.14159f * frequency * t;
        
        if (type == Sine) val = std::sin(phase);
        else if (type == Square) val = (std::sin(phase) > 0) ? 1.0f : -1.0f;
        else if (type == Saw) val = (float)fmod(phase / (2.0f * 3.14159f), 1.0) * 2.0f - 1.0f;
        
        return offset + (val * amplitude);
    }
};
