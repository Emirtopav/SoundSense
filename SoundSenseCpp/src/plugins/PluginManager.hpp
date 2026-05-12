#pragma once
#include "SoundSenseSDK/SoundSenseSDK.hpp"
#include <string>
#include <vector>
#include <windows.h>

class PluginManager {
public:
  struct PluginInstance {
    HMODULE hModule;
    IPlugin *plugin;
    bool enabled;
  };

  std::vector<PluginInstance> loadedPlugins;

  PluginManager();
  ~PluginManager();

  void LoadPlugins(const std::string &directory);
  void RunPlugins(const std::vector<float> &audioBands,
                  std::vector<int> &bitmap);
  
  bool DoesAnyPluginOverrideBase() const;
};
