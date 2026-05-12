#include "PluginManager.hpp"
#include "JsonPlugin.hpp"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

PluginManager::PluginManager() {}

PluginManager::~PluginManager() {
  for (auto &inst : loadedPlugins) {
    if (inst.hModule) {
      FreeLibrary(inst.hModule);
    } else if (inst.plugin) {
      delete inst.plugin; // JsonPlugin instance
    }
  }
  loadedPlugins.clear();
}

void PluginManager::LoadPlugins(const std::string &directory) {
    if (!fs::exists(directory)) return;

    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string path = entry.path().string();
            std::string filename = entry.path().filename().string();
            std::string ext = entry.path().extension().string();


            if (filename == "ClockPlugin.dll" || filename == "InvertPlugin.dll") {
                std::cout << "[PluginManager] Skipping blacklisted plugin: " << filename << std::endl;
                continue;
            }

            if (ext == ".dll") {
                HMODULE hModule = LoadLibraryA(path.c_str());
                if (hModule) {
                    CreatePluginFunc createFunc = (CreatePluginFunc)GetProcAddress(hModule, "CreatePlugin");
                    if (createFunc) {
                        IPlugin *plugin = createFunc();
                        if (plugin) {
                            plugin->Initialize();
                            PluginInstance inst;
                            inst.hModule = hModule;
                            inst.plugin = plugin;
                            inst.enabled = true;
                            loadedPlugins.push_back(inst);
                        } else {
                            std::cerr << "[PluginManager] CreatePlugin returned null for: " << path << std::endl;
                            FreeLibrary(hModule);
                        }
                    } else {
                        std::cerr << "[PluginManager] CreatePlugin export not found in: " << path << std::endl;
                        FreeLibrary(hModule);
                    }
                } else {
                    std::cerr << "[PluginManager] LoadLibrary failed for: " << path << " Error: " << GetLastError() << std::endl;
                }
            } else if (ext == ".scene") {
                JsonPlugin* plugin = new JsonPlugin(path);
                plugin->Initialize();
                
                PluginInstance inst;
                inst.hModule = nullptr;
                inst.plugin = plugin;
                inst.enabled = true;
                loadedPlugins.push_back(inst);
            }
        }
    }
}

void PluginManager::RunPlugins(const std::vector<float> &audioBands,
                               std::vector<int> &bitmap) {
  for (auto &inst : loadedPlugins) {
    if (inst.enabled && inst.plugin) {
      inst.plugin->Process(audioBands, bitmap);
    }
  }
}

bool PluginManager::DoesAnyPluginOverrideBase() const {
  for (const auto &inst : loadedPlugins) {
    if (inst.enabled && inst.plugin && inst.plugin->OverridesBase()) {
      return true;
    }
  }
  return false;
}
