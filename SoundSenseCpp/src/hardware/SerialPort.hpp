//       _____      _           _ _____           _     _
//      / ____|    (_)         | |  __ \         | |   | |
//     | (___   ___ _ _ __ __ _| | |__) |__  _ __| |_  | |__  _ __  _ __
//      \___ \ / _ \ | '__/ _` | |  ___/ _ \| '__| __| | '_ \| '_ \| '_ \ 
//      ____) |  __/ | | | (_| | | |  | (_) | |  | |_ _| | | | |_) | |_) |
//     |_____/ \___|_|_|  \__,_|_|_|   \___/|_|   \__(_)_| |_| .__/| .__/
//                                                           | |   | |
//                                                           |_|   |_|
//
#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include <cstdint>

class SerialPort {
private:
  HANDLE hSerial;
  bool connected;
  COMSTAT status;
  DWORD errors;

public:
  static std::vector<std::string> GetAvailablePorts();

  SerialPort();

  bool Open(const std::string &portName, DWORD baudRate = CBR_115200);
  void Close();
  bool Write(const std::string &data);
  bool Write(const uint8_t* data, size_t size);
  bool IsConnected() const;
};
