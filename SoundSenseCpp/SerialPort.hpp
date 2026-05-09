#pragma once
#include <windows.h>
#include <string>
#include <vector>

class SerialPort {
private:
    HANDLE hSerial;
    bool connected;
    COMSTAT status;
    DWORD errors;

public:
    static std::vector<std::string> GetAvailablePorts() {
        std::vector<std::string> ports;
        HKEY hKey;
        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            char valueName[256];
            unsigned char data[256];
            DWORD valueNameSize, dataSize, type;
            for (int i = 0; ; i++) {
                valueNameSize = 256;
                dataSize = 256;
                if (RegEnumValue(hKey, i, valueName, &valueNameSize, NULL, &type, data, &dataSize) != ERROR_SUCCESS) break;
                ports.push_back((char*)data);
            }
            RegCloseKey(hKey);
        }
        return ports;
    }

    SerialPort() : hSerial(INVALID_HANDLE_VALUE), connected(false) {}

    bool Open(const std::string& portName, DWORD baudRate = CBR_115200) {
        std::string fullPortName = "\\\\.\\" + portName;
        hSerial = CreateFile(fullPortName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hSerial == INVALID_HANDLE_VALUE) return false;

        DCB dcbSerialParams = {0};
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
        if (!GetCommState(hSerial, &dcbSerialParams)) { Close(); return false; }

        dcbSerialParams.BaudRate = baudRate;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;
        dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

        if (!SetCommState(hSerial, &dcbSerialParams)) { Close(); return false; }

        connected = true;
        PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
        return true;
    }

    void Close() {
        if (hSerial != INVALID_HANDLE_VALUE) {
            CloseHandle(hSerial);
            hSerial = INVALID_HANDLE_VALUE;
        }
        connected = false;
    }

    bool Write(const std::string& data) {
        if (!connected) return false;
        DWORD bytesSent;
        if (!WriteFile(hSerial, data.c_str(), (DWORD)data.length(), &bytesSent, NULL)) {
            connected = false;
            return false;
        }
        return true;
    }

    bool IsConnected() const { return connected; }
};
