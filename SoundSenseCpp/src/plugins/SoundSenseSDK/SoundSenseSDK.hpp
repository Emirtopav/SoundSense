/**
 * @file SoundSenseSDK.hpp
 * @brief Software Development Kit for SoundSense Plugins
 * 
 * This header defines the interface required to create custom dynamic visualizers
 * (.dll plugins) for the SoundSense Application. 
 * 
 * To create a plugin:
 * 1. Include this header in your C++ project.
 * 2. Create a class that inherits from `IPlugin` and implement the pure virtual methods.
 * 3. Export a `CreatePlugin()` function that returns a new instance of your class.
 * 4. Compile your code as a shared library (.dll) and place it in the `plugins/` directory.
 */

#pragma once
#include <vector>
#include <string>
#include "../../core/TriggerSystem.hpp"

class IPlugin {
public:
  virtual ~IPlugin() = default;

  /**
   * @brief Get the Name of the plugin.
   * @return A null-terminated C-string representing the plugin name.
   */
  virtual const char *GetName() const = 0;
  virtual const char *GetDescription() const { return "No description"; }
  virtual const char *GetVersion() const { return "1.0.0"; }
  virtual const char *GetAuthor() const { return "Unknown"; }

  /**
   * @brief Called once when the plugin is successfully loaded by the manager.
   */
  virtual void Initialize() = 0;

  /**
   * @brief Defines the render mode of this plugin.
   * @return If TRUE, the base visualizer (bars) is skipped, and you draw from scratch.
   *         If FALSE, your drawing is layered ON TOP of the base visualizer.
   */
  virtual bool OverridesBase() const = 0;

  /**
   * @brief Process audio data and modify the 128x40 OLED bitmap.
   * This is called every frame (approx 30-60 FPS).
   * 
   * @param audioBands A 64-element vector of normalized audio magnitudes (0.0 to 1.0).
   * @param bitmap A 640-byte flat buffer representing a monochrome 128x40 display.
   *               Pixel layout: 8 pixels per byte.
   */
  virtual void Process(const std::vector<float> &audioBands,
                       std::vector<int> &bitmap) = 0;

  virtual void OnTrigger(const AudioEvent& event) {}
};

/**
 * @brief The entry point that MUST be exported by the compiled DLL.
 * Example implementation:
 * extern "C" __declspec(dllexport) IPlugin* CreatePlugin() { return new MyPlugin(); }
 */
typedef IPlugin *(*CreatePluginFunc)();
