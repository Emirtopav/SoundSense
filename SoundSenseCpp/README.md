# 🎧 SoundSense Visualizer (C++)

High-performance, ultra-low latency audio visualizer for SteelSeries OLED screens. Built with C++, Qt 6, and WinHTTP.

![Version](https://img.shields.io/badge/version-1.2.0-blue)

![SoundSense Logo](logo.png)

## ✨ Features

- **🚀 Ultra-Low Latency**: Optimized 60 FPS rendering pipeline for real-time response.
- **🎨 Scene Editor**: Create and manage custom visualizer scenes with rectangles, text, clocks, and waveforms.
- **🔌 Plugin System**: Extend functionality with C++ DLLs or JSON-based scene definitions.
- **📉 64-Band FFT**: Detailed audio spectrum analysis using the Windows Core Audio API (WASAPI).
- **🕹️ SteelSeries Integration**: Seamlessly syncs with your Apex series keyboards or other OLED-equipped devices.
- **📥 System Tray Support**: Runs silently in the background with easy access from the system tray.
- **⚙️ Persistent Settings**: Automatically saves and loads your gain, sensitivity, and visual preferences.

## 🛠️ Technical Stack

- **Core**: C++17
- **UI Framework**: Qt 6.11 (QML/Quick)
- **Audio API**: WASAPI (Windows Audio Session API)
- **Network**: WinHTTP (GameSense API)
- **Graphics**: GPU-accelerated rendering

## 🚀 Getting Started

### Prerequisites
- Windows 10/11
- SteelSeries Engine / SteelSeries GG
- Qt 6.x (for development)
- CMake 3.16+

### Installation
1. Download the latest release from the [Official Releases](https://github.com/Emirtopav/SoundSense/releases) page.
2. Run `SoundSenseCpp.exe`.
3. The app will automatically discover your SteelSeries device and start rendering.
4. Access the dashboard by double-clicking the tray icon.

## 📁 Project Structure

- `/src/ui`: QML Backend and UI logic.
- `/src/core`: Audio analysis, rendering engine, and global state.
- `/src/network`: GameSense API client.
- `/src/plugins`: Plugin manager and base plugin implementations.
- `/qml`: UI layouts and components.
- `/plugins/modules`: Compiled DLL plugins.

## 🤝 Contributing

Contributions are welcome! Feel free to open issues or submit pull requests to improve the visualizer or add new plugins.

## 📜 License

Distributed under the MIT License. See `LICENSE` for more information.

---
**Developed with ❤️ by Emirtopav**
