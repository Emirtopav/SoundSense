#include "Scene.hpp"
#include "VisualizerWidget.hpp"
#include "RectWidget.hpp"
#include "TextWidget.hpp"
#include "ClockWidget.hpp"
#include "ImageWidget.hpp"
#include "WaveformWidget.hpp"
#include <fstream>
#include <iostream>

Scene::Scene() {
  // Add a default visualizer so the screen isn't empty on first boot
  AddWidget(std::make_unique<VisualizerWidget>());
}

Scene::~Scene() {}

void Scene::AddWidget(std::unique_ptr<Widget> widget) {
  widgets.push_back(std::move(widget));
}

void Scene::RemoveWidget(int index) {
  if (index >= 0 && index < widgets.size()) {
    widgets.erase(widgets.begin() + index);
  }
}

void Scene::RenderAll(const std::vector<float> &audioBands,
                      std::vector<int> &bitmap) {
  for (auto &widget : widgets) {
    if (widget->visible) {
      widget->Render(audioBands, bitmap);
    }
  }
}

bool Scene::SaveToFile(const std::string &path) {
    try {
        json j;
        j["version"] = "1.0";
        j["widgets"] = json::array();
        
        for (auto &w : widgets) {
            j["widgets"].push_back(w->toJson());
        }
        
        std::ofstream file(path);
        if (!file.is_open()) return false;
        file << j.dump(4);
        return true;
    } catch (...) {
        return false;
    }
}

bool Scene::LoadFromFile(const std::string &path) {
    try {
        std::ifstream file(path);
        if (!file.is_open()) return false;
        
        json j;
        file >> j;
        
        if (!j.contains("widgets") || !j["widgets"].is_array()) return false;
        
        widgets.clear();
        for (const auto& wj : j["widgets"]) {
            std::string type = wj.value("type", "");
            std::unique_ptr<Widget> w;
            
            if (type == "visualizer") w = std::make_unique<VisualizerWidget>();
            else if (type == "rectangle") w = std::make_unique<RectWidget>();
            else if (type == "text") w = std::make_unique<TextWidget>();
            else if (type == "clock") w = std::make_unique<ClockWidget>();
            else if (type == "image") w = std::make_unique<ImageWidget>();
            else if (type == "waveform") w = std::make_unique<WaveformWidget>();
            
            if (w) {
                w->fromJson(wj);
                widgets.push_back(std::move(w));
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}
