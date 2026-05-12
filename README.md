<div align="center">
  <img src="SoundSenseCpp/logo.png" alt="SoundSense Logo" width="150" height="150">
  <h1>SoundSense v1.2</h1>
  <p>High-performance audio visualizer for SteelSeries OLED displays and ESP32-based external hardware.</p>

  ![License](https://img.shields.io/github/license/Emirtopav/SoundSense)
  ![Version](https://img.shields.io/badge/version-1.2.0-blue)
  ![Platform](https://img.shields.io/badge/platform-Windows-lightgrey)
  ![C++](https://img.shields.io/badge/C++-17-00599C?logo=c%2B%2B)
</div>

---

## Overview

SoundSense is a professional-grade C++ audio visualization suite that captures system audio with zero latency using WASAPI and renders it across multiple platforms simultaneously.

### Key Features
- **SteelSeries Integration**: Native support for Apex series keyboards (Apex 5, 7, Pro) and SteelSeries OLED mice.
- **ESP32 External Display**: Real-time sync to external SSD1306 OLED screens via Serial (COM).
- **Ultra-Low Latency**: Optimized 60 FPS rendering pipeline.
- **Scene Editor**: Fully customizable UI with drag-and-drop layers, clocks, and waveforms.
- **System Tray Mode**: Runs silently in the background with a professional tray menu.
- **Automated Discovery**: Auto-detects SteelSeries devices and available COM ports.

---

## Visual Modes

| Classic Bars | Mirror Mode | Dot Mode |
|:---:|:---:|:---:|
| <img src="media/Classic.gif" width="250"> | <img src="media/mirror mode.gif" width="250"> | <img src="media/Dot Mode.gif" width="250"> |

---

## Hardware Setup (ESP32)

SoundSense can project the visualization to an external ESP32-powered display.

### Hardware Requirements
- **Microcontroller**: ESP32 (S3, C3, or standard)
- **Display**: SSD1306 OLED (128x64 resolution recommended)
- **Communication**: USB-to-Serial at 115200 Baud

### Communication Protocol
The desktop app sends data packets at 60Hz:
- **Header**: `0xFF` (Sync Byte)
- **Payload**: 64 Bytes (Each byte represents a spectrum band from 0-255)
- **Total Packet Size**: 65 Bytes

### Firmware Installation
1. Navigate to the `SoundSense_IDF` folder.
2. Flash the firmware using ESP-IDF or the provided `flash_firmware.bat`.
3. Default I2C Pins: `SDA: 8`, `SCL: 9` (Adjust in `main.c` if needed).

---

## Technical Stack

- **Framework**: Qt 6.11 (QML/Quick)
- **Audio API**: Windows Audio Session API (WASAPI)
- **Networking**: WinHTTP for GameSense API
- **Language**: C++17
- **Installer**: Inno Setup

---

## Installation & Usage

1. Download the latest `SoundSense_Setup.exe` from the [Official Releases](https://github.com/Emirtopav/SoundSense/releases) section.
2. Install and launch the application.
3. **SteelSeries Users**: Ensure SteelSeries GG is running. The app will connect automatically.
4. **ESP32 Users**: Select your COM Port from the "External Hardware" tab and click "Connect".
5. Double-click the **Tray Icon** to hide/show the Dashboard.

---

## Contributing
Feel free to fork this project and submit pull requests. For major changes, please open an issue first to discuss what you would like to change.

## License
This project is licensed under the MIT License - see the `LICENSE` file for details.

---
<div align="center">
  <b>Developed with ❤️ by <a href="https://github.com/Emirtopav">Emir Topav (Emirtopav)</a></b>
</div>
