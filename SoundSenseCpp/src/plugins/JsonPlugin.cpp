#include "JsonPlugin.hpp"
#include <fstream>
#include <iostream>
#include <cmath>

JsonPlugin::JsonPlugin(const std::string& sceneJsonPath) : m_FilePath(sceneJsonPath), m_OverridesBase(true) {
    LoadFromJson(sceneJsonPath);
}

JsonPlugin::~JsonPlugin() {
}

bool JsonPlugin::LoadFromJson(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    try {
        file >> m_SceneData;
        m_Name = m_SceneData.value("pluginName", "Unknown Scene");
        m_Description = m_SceneData.value("description", "Custom Scene Plugin");
        m_Version = m_SceneData.value("version", "1.0.0");
        m_Author = m_SceneData.value("author", "SoundSense");
        m_OverridesBase = m_SceneData.value("overridesBase", true);
        return true;
    } catch (const json::parse_error& e) {
        std::cerr << "JSON Parse error in " << filePath << ": " << e.what() << std::endl;
        return false;
    }
}

const char* JsonPlugin::GetName() const {
    return m_Name.c_str();
}

void JsonPlugin::Initialize() {
    m_FrameCounter = 0;
    m_TimeCounter = 0.0f;
}

bool JsonPlugin::OverridesBase() const {
    return m_OverridesBase;
}

void JsonPlugin::Process(const std::vector<float>& audioBands, std::vector<int>& bitmap) {
    if (!m_SceneData.contains("layers") || !m_SceneData["layers"].is_array()) return;

    m_FrameCounter++;
    m_TimeCounter += 0.016f; // approx 60fps delta

    for (const auto& layer : m_SceneData["layers"]) {
        std::string type = layer.value("type", "");
        
        // Base Transform
        int x = layer.value("x", 0);
        int y = layer.value("y", 0);
        int w = layer.value("w", 0);
        int h = layer.value("h", 0);

        // Reactivity
        float audioModifier = 0.0f;
        if (layer.contains("reactivity") && !layer["reactivity"].is_null()) {
            auto react = layer["reactivity"];
            std::string prop = react.value("property", "none");
            if (prop != "none") {
                int startBand = react.value("bandStart", 0);
                int endBand = react.value("bandEnd", 11);
                float mult = react.value("multiplier", 1.0f);

                if (startBand >= 0 && endBand < 64 && startBand <= endBand) {
                    float sum = 0;
                    for (int i = startBand; i <= endBand; i++) sum += audioBands[i];
                    float avg = sum / (endBand - startBand + 1);
                    audioModifier = avg * mult;
                }

                if (prop == "scale_y") h += (int)audioModifier;
                else if (prop == "scale_x") w += (int)audioModifier;
                else if (prop == "move_y") y -= (int)audioModifier;
                // Opacity pulsing can be handled by skipping drawing if value is too low,
                // or just alternating frame mod. We'll implement threshold pulse here:
                else if (prop == "opacity") {
                    if (audioModifier < 0.5f) continue; // Skip rendering this layer
                }
            }
        }

        // Appearance
        json app = layer.contains("appearance") ? layer["appearance"] : json::object();

        if (type == "rectangle") {
            bool fill = app.value("fill", true);
            bool outline = app.value("outline", false);

            for(int cy = y; cy < y + h; cy++) {
                for(int cx = x; cx < x + w; cx++) {
                    if(cx >= 0 && cx < 128 && cy >= 0 && cy < 40) {
                        bool drawPixel = fill;
                        if (outline && !fill) {
                            drawPixel = (cx == x || cx == x + w - 1 || cy == y || cy == y + h - 1);
                        } else if (outline && fill) {
                            // If both, maybe invert the outline? 
                            // Standard: draw all.
                            drawPixel = true; 
                        }

                        if (drawPixel) {
                            int byteIdx = (cy * 16) + (cx / 8);
                            int bitIdx = 7 - (cx % 8);
                            bitmap[byteIdx] |= (1 << bitIdx);
                        }
                    }
                }
            }
        } else if (type == "text") {
            std::string text = app.value("text", "TEXT");
            bool blink = app.value("blink", false);
            bool marquee = app.value("marquee", false);

            if (blink && (m_FrameCounter % 60) > 30) continue;

            int drawX = x;
            if (marquee) {
                drawX = x - (m_FrameCounter % (128 + text.length() * 6)) + 128;
            }

            // Simple 5x7 font rendering simulation (just blocks for now, 
            // since we don't have a global font array here, but let's draw a placeholder).
            // Actually, we can just draw small blocks for characters:
            for (size_t i = 0; i < text.length(); i++) {
                int charX = drawX + (i * 6);
                for(int cy = y; cy < y + 7; cy++) {
                    for(int cx = charX; cx < charX + 5; cx++) {
                        // checkerboard to simulate text
                        if(cx >= 0 && cx < 128 && cy >= 0 && cy < 40 && ((cx+cy)%2 == 0)) {
                            int byteIdx = (cy * 16) + (cx / 8);
                            int bitIdx = 7 - (cx % 8);
                            bitmap[byteIdx] |= (1 << bitIdx);
                        }
                    }
                }
            }
        }
    }
}
