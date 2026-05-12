#pragma once
#include "Widget.hpp"
#include <memory>
#include <vector>

class Scene {
public:
  std::vector<std::unique_ptr<Widget>> widgets;

  Scene();
  ~Scene();

  void AddWidget(std::unique_ptr<Widget> widget);
  void RemoveWidget(int index);

  void RenderAll(const std::vector<float> &audioBands,
                 std::vector<int> &bitmap);

  bool SaveToFile(const std::string &path);
  bool LoadFromFile(const std::string &path);
};
