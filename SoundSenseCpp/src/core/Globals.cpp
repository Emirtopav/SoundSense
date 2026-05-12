#include "Globals.hpp"

std::atomic<bool> g_Running(true);
VisualizerSettings g_Settings;
std::vector<int> g_CurrentBitmap(640, 0);
std::mutex g_BitmapMutex;
SerialPort g_Serial;
std::string g_ComPortName = "COM3";
float g_PeakHeights[64] = {0};
std::recursive_mutex g_SettingsMutex;
std::recursive_mutex g_PluginMutex;
std::atomic<bool> g_SteelSeriesConnected(false);

TriggerDetector g_TriggerDetector;
LFO g_SensitivityLFO;

Scene g_Scene;
PluginManager g_PluginManager;

// Telemetry
std::atomic<int> g_RenderFps(0);
std::atomic<int> g_AudioLatencyMs(0);
std::atomic<int> g_RenderTimeMs(0);
std::atomic<int> g_DroppedFrames(0);
std::atomic<bool> g_AudioActive(false);
