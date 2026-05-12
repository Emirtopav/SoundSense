#pragma once
#include "AudioAnalyzer.hpp"
#include <vector>

void SetPixel(std::vector<int> &buffer, int x, int y, bool on);
std::vector<int> RenderBitmap(const std::vector<float> &data,
                              const VisualizerSettings &settings);
