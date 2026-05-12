#pragma once
#include "AudioAnalyzer.hpp"
#include "SerialPort.hpp"
#include <atomic>
#include <mutex>
#include <string>
#include <vector>

extern std::atomic<bool> g_Running;
extern VisualizerSettings g_Settings;
extern std::vector<int> g_CurrentBitmap;
extern std::mutex g_BitmapMutex;
extern SerialPort g_Serial;
extern std::string g_ComPortName;
extern float g_PeakHeights[64];
extern std::recursive_mutex g_SettingsMutex;
extern std::recursive_mutex g_PluginMutex;

// Telemetry
extern std::atomic<int> g_RenderFps;
extern std::atomic<int> g_AudioLatencyMs;
extern std::atomic<int> g_RenderTimeMs;
extern std::atomic<int> g_DroppedFrames;
extern std::atomic<bool> g_AudioActive;
extern std::atomic<bool> g_SteelSeriesConnected;

#include "Scene.hpp"
extern Scene g_Scene;

#include "TriggerSystem.hpp"
extern TriggerDetector g_TriggerDetector;

#include "LFO.hpp"
extern LFO g_SensitivityLFO;

#include "PluginManager.hpp"
extern PluginManager g_PluginManager;
