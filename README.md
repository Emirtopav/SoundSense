<div align="center">
  <img src="media/logo.png" alt="SoundSense Logo" width="150" height="150">
  <h1>SoundSense</h1>
  <p>A high-performance audio visualizer for SteelSeries OLED displays and ESP32-based external hardware.</p>

  ![License](https://img.shields.io/github/license/Emirtopav/SoundSense)
  ![Version](https://img.shields.io/badge/version-1.0.0-blue)
  ![Platform](https://img.shields.io/badge/platform-Windows-lightgrey)
  ![C++](https://img.shields.io/badge/C++-17-00599C?logo=c%2B%2B)
</div>

---

## 🚀 Key Features

- **SteelSeries GameSense API**: Full support for Apex 5, 7, Pro, and other SteelSeries devices with OLED screens.
- **ESP32 Sync**: Real-time synchronization of 64-band audio data and visual modes to external hardware via Serial (COM port) with auto port detection.
- **Ultra-Low Latency**: Millisecond response time thanks to WASAPI loopback and optimized FFT analysis.
- **Customizable Modes**: Choose from Classic Bars, Mirror Mode, and Dot Mode, with optional Peak Hold drops.
- **Audio Control**: Fine-tune Global Sensitivity, Bass/Mid/Treble gains, Falloff speed, and Smoothing logic directly from the UI.
- **Native Windows UI**: Auto-detects Windows Dark/Light mode, runs gracefully in the System Tray, and persists settings automatically.

## 📸 Visual Modes

Here are the supported visualization modes that perfectly synchronize across your SteelSeries keyboard and ESP32 display:

| Classic Mode | Mirror Mode | Dot Mode |
|:---:|:---:|:---:|
| <img src="media/Classic.gif" width="250"> | <img src="media/mirror mode.gif" width="250"> | <img src="media/Dot Mode.gif" width="250"> |

## 🛠️ Requirements

- **SteelSeries Engine / SteelSeries GG** software must be running in the background.
- **Windows 10/11** Operating System.
- *(Optional)* An ESP32 connected via USB running the provided `SoundSense_ESP32.ino` sketch.

## 🚀 How to Run
1. Run the `SoundSense.exe`.
2. It will automatically detect SteelSeries GG and bind the app.
3. If you have an ESP32, select your COM port from the dropdown and hit **Connect**.
4. Adjust sliders to fine-tune the visualization to your current audio volume and preferences.

---

<div align="center">
  <h1>Türkçe Açıklama</h1>
</div>

SteelSeries klavyelerdeki OLED ekranlar ve ESP32 tabanlı harici ekranlar için geliştirilmiş, yüksek performanslı bir ses görselleştirici (Audio Visualizer). WASAPI loopback kullanarak sistem sesini yakalar ve gerçek zamanlı olarak 64 bantlı spektrum analizi yapar.

## 🚀 Özellikler

- **SteelSeries GameSense API**: Apex 5, 7, Pro ve diğer OLED ekranlı SteelSeries cihazlarla tam uyum.
- **ESP32 Senkronizasyonu**: Seri port (COM) üzerinden harici donanımlara (ESP32, Arduino vb.) 64 bantlı ses verisini ve görsel efekt komutlarını eşzamanlı olarak gönderir.
- **Düşük Gecikme**: WASAPI ve optimize edilmiş FFT (Fast Fourier Transform) ile milisaniyelik, akıcı tepki süresi.
- **Özelleştirilebilir Görselleştirme**:
  - **Modlar**: Classic Bars (Klasik Çubuklar), Mirror Mode (Ortadan Dışa Yayılan Ayna Modu), Dot Mode (Nokta Modu).
  - **Peak Hold**: En yüksek frekans noktalarını yakalayan, yerçekimi etkili "Peak Dot" desteği.
- **Gelişmiş Ses Ayarları**:
  - Global Hassasiyet, Bass/Mid/Treble kazanç ayarları.
  - Falloff (Düşüş hızı) ve Smoothing (Yumuşatma) kontrolleri.
- **Modern Arayüz**:
  - Windows Temasıyla Uyumlu (Dark/Light mode tespiti).
  - Otomatik COM portu algılama menüsü.
  - Sistem Tepsisi (Tray Icon) desteği ile arka planda gizlice çalışma ve ayarların otomatik kaydedilmesi.

## 💻 Teknik Detaylar

- **Dil**: C++
- **Kütüphaneler/API**:
  - `WASAPI`: Sistem sesini doğrudan yakalamak için.
  - `WinHTTP`: SteelSeries GameSense API'si ile HTTP üzerinden haberleşmek için.
  - `Win32 API`: Yerel Windows GUI (Arayüz) ve Sistem tepsisi (Tray) yönetimi için.
  - `Serial Communication`: ESP32 ile 115200 baud oranında, tampon (buffer) taşmalarını önleyen veri aktarımı için.

---

## 📄 License

This project is licensed under the MIT License - see the `LICENSE` file for details.

---
<div align="center">
  <b>Developed by: <a href="https://github.com/Emirtopav">Emir Topav (Emirtopav)</a></b>
</div>
