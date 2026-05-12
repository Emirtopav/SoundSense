#include "PresetManager.hpp"
#include "vendor/json/json.hpp"
#include "PluginManager.hpp"
#include "SceneModel.hpp"
#include "PluginModel.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <windows.h>

using json = nlohmann::json;
namespace fs = std::filesystem;

PresetManager* g_PresetManager = nullptr;

PresetManager::PresetManager(QObject* parent) : QObject(parent), m_model(this) {
    m_presetsDir = "presets";
    if (!fs::exists(m_presetsDir)) {
        fs::create_directory(m_presetsDir);
    }
    refreshPresets();
}

void PresetManager::savePreset(const QString& name) {
    json j;
    j["name"] = name.toStdString();
    
    // Audio Settings
    j["audio"] = {
        {"bassGain", g_Settings.bassGain},
        {"midGain", g_Settings.midGain},
        {"trebleGain", g_Settings.trebleGain},
        {"globalSens", g_Settings.globalSens},
        {"falloff", g_Settings.falloff},
        {"smoothing", g_Settings.smoothing},
        {"showPeaks", g_Settings.showPeaks}
    };
    
    // Plugin Stack
    json plugins = json::array();
    for (const auto& inst : g_PluginManager.loadedPlugins) {
        plugins.push_back({
            {"name", inst.plugin->GetName()},
            {"enabled", inst.enabled}
        });
    }
    j["plugins"] = plugins;
    
    std::string filename = m_presetsDir + "/" + name.toStdString() + ".preset";
    std::ofstream o(filename);
    o << j.dump(4);
    
    refreshPresets();
}

void PresetManager::applyPreset(const QString& name) {
    std::string filename = m_presetsDir + "/" + name.toStdString() + ".preset";
    if (!fs::exists(filename)) return;
    
    std::ifstream i(filename);
    json j;
    i >> j;
    
    // Audio Settings
    if (j.contains("audio")) {
        auto a = j["audio"];
        g_Settings.bassGain = a.value("bassGain", 1.0f);
        g_Settings.midGain = a.value("midGain", 1.0f);
        g_Settings.trebleGain = a.value("trebleGain", 1.0f);
        g_Settings.globalSens = a.value("globalSens", 1.0f);
        g_Settings.falloff = a.value("falloff", 0.4f);
        g_Settings.smoothing = a.value("smoothing", 0.5f);
        g_Settings.showPeaks = a.value("showPeaks", true);
    }
    
    // Plugin Stack - Reorder and Enable
    if (j.contains("plugins")) {
        auto pluginsJson = j["plugins"];
        std::vector<PluginManager::PluginInstance> newOrder;
        
        for (const auto& pj : pluginsJson) {
            std::string pName = pj["name"];
            bool pEnabled = pj["enabled"];
            
            // Find in current loaded plugins
            auto it = std::find_if(g_PluginManager.loadedPlugins.begin(), g_PluginManager.loadedPlugins.end(),
                [&](const PluginManager::PluginInstance& inst) {
                    return std::string(inst.plugin->GetName()) == pName;
                });
            
            if (it != g_PluginManager.loadedPlugins.end()) {
                PluginManager::PluginInstance inst = *it;
                inst.enabled = pEnabled;
                newOrder.push_back(inst);
                g_PluginManager.loadedPlugins.erase(it);
            }
        }
        
        // Add remaining plugins that weren't in the preset to the end
        for (const auto& inst : g_PluginManager.loadedPlugins) {
            newOrder.push_back(inst);
        }
        
        g_PluginManager.loadedPlugins = newOrder;
    }
    
    // Notify UI
    // Note: We need a way to tell BackendContext that settings changed
}

void PresetManager::refreshPresets() {
    m_model.refresh();
}
