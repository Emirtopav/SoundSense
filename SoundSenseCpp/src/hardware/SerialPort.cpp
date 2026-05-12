#include "SerialPort.hpp"
#include <cstdint>

std::vector<std::string> SerialPort::GetAvailablePorts() {
  std::vector<std::string> ports;
  HKEY hKey;
  if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0,
                   KEY_READ, &hKey) == ERROR_SUCCESS) {
    char valueName[256];
    unsigned char data[256];
    DWORD valueNameSize, dataSize, type;
    for (int i = 0;; i++) {
      valueNameSize = 256;
      dataSize = 256;
      if (RegEnumValueA(hKey, i, valueName, &valueNameSize, NULL, &type, data,
                       &dataSize) != ERROR_SUCCESS)
        break;
      ports.push_back((char *)data);
    }
    RegCloseKey(hKey);
  }
  return ports;
}

SerialPort::SerialPort() : hSerial(INVALID_HANDLE_VALUE), connected(false) {}

bool SerialPort::Open(const std::string &portName, DWORD baudRate) {
  std::string fullPortName = "\\\\.\\" + portName;
  hSerial = CreateFileA(fullPortName.c_str(), GENERIC_READ | GENERIC_WRITE, 0,
                       NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if (hSerial == INVALID_HANDLE_VALUE)
    return false;

  DCB dcbSerialParams = {0};
  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
  if (!GetCommState(hSerial, &dcbSerialParams)) {
    Close();
    return false;
  }

  dcbSerialParams.BaudRate = baudRate;
  dcbSerialParams.ByteSize = 8;
  dcbSerialParams.StopBits = ONESTOPBIT;
  dcbSerialParams.Parity = NOPARITY;
  dcbSerialParams.fDtrControl = DTR_CONTROL_DISABLE;
  dcbSerialParams.fRtsControl = RTS_CONTROL_DISABLE;

  if (!SetCommState(hSerial, &dcbSerialParams)) {
    Close();
    return false;
  }

  connected = true;
  PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
  return true;
}

void SerialPort::Close() {
  if (hSerial != INVALID_HANDLE_VALUE) {
    CloseHandle(hSerial);
    hSerial = INVALID_HANDLE_VALUE;
  }
  connected = false;
}

bool SerialPort::Write(const std::string &data) {
  return Write((const uint8_t*)data.c_str(), data.length());
}

bool SerialPort::Write(const uint8_t* data, size_t size) {
  if (!connected)
    return false;
  DWORD bytesSent;
  if (!WriteFile(hSerial, data, (DWORD)size, &bytesSent, NULL)) {
    connected = false;
    return false;
  }
  return true;
}

bool SerialPort::IsConnected() const { return connected; }
