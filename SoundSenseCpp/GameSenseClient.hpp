#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

class GameSenseClient {
private:
  std::string _address;
  std::string _host;
  int _port;
  const std::string GameName = "SOUNDSENSE";

  HINTERNET hSession = NULL;
  HINTERNET hConnect = NULL;

  void DiscoverAddress() {
    char *appData = getenv("PROGRAMDATA");
    if (!appData)
      throw std::runtime_error("PROGRAMDATA not found");
    std::string path = std::string(appData) +
                       "\\SteelSeries\\SteelSeries Engine 3\\coreProps.json";

    std::ifstream file(path);
    if (!file.is_open())
      throw std::runtime_error("coreProps.json not found");

    std::stringstream ss;
    ss << file.rdbuf();
    std::string content = ss.str();
    size_t addrPos = content.find("\"address\"");
    if (addrPos != std::string::npos) {
      size_t start = content.find("\"", addrPos + 9);
      size_t end = content.find("\"", start + 1);
      _address = content.substr(start + 1, end - start - 1);
      size_t colon = _address.find(":");
      _host = _address.substr(0, colon);
      _port = std::stoi(_address.substr(colon + 1));
      std::cout << "Discovered GameSense at " << _address << std::endl;
    }
  }

  bool Post(const std::string &endpoint, const std::string &jsonPayload) {
    if (!hConnect)
      return false;

    std::wstring wEndpoint(endpoint.begin(), endpoint.end());
    std::wstring wPath = L"/" + wEndpoint;
    HINTERNET hRequest =
        WinHttpOpenRequest(hConnect, L"POST", wPath.c_str(), NULL,
                           WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    if (!hRequest)
      return false;

    bool result = WinHttpSendRequest(
        hRequest, L"Content-Type: application/json\r\n", -1L,
        (LPVOID)jsonPayload.c_str(), (DWORD)jsonPayload.length(),
        (DWORD)jsonPayload.length(), 0);
    if (result)
      result = WinHttpReceiveResponse(hRequest, NULL);

    if (!result) {
      std::cout << "WinHttp Error: " << GetLastError() << " on " << endpoint
                << std::endl;
    }

    WinHttpCloseHandle(hRequest);
    return result;
  }

public:
  GameSenseClient() {
    DiscoverAddress();
    hSession = WinHttpOpen(L"SoundSense/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                           WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    std::wstring wHost(_host.begin(), _host.end());
    hConnect = WinHttpConnect(hSession, wHost.c_str(), _port, 0);
  }

  ~GameSenseClient() {
    if (hConnect)
      WinHttpCloseHandle(hConnect);
    if (hSession)
      WinHttpCloseHandle(hSession);
  }

  void RegisterGame() {
    std::string payload = "{\"game\":\"" + GameName +
                          "\",\"game_display_name\":\"SoundSense "
                          "Visualizer\",\"developer\":\"Emirtopav\",\"icon_color_id\":5,"
                          "\"icon_url\":\"file:///C:/ProgramData/SteelSeries/SteelSeries%20Engine%203/db/games/SOUNDSENSE/icon.png\","
                          "\"logo_url\":\"file:///C:/ProgramData/SteelSeries/SteelSeries%20Engine%203/db/games/SOUNDSENSE/banner.png\"}";
    Post("game_metadata", payload);
  }

  void RegisterEvent() {
    std::string payload =
        "{\"game\":\"" + GameName +
        "\",\"event\":\"AUDIO_UPDATE\",\"value_optional\":true}";
    Post("register_game_event", payload);
  }

  void BindEvent() {
    // Including an empty 128x40 bitmap in the bind (640 zeros)
    std::stringstream ss;
    ss << "{\"game\":\"" << GameName
       << "\",\"event\":\"AUDIO_UPDATE\",\"handlers\":[{"
       << "\"device-type\":\"screened-128x40\",\"zone\":\"one\",\"mode\":"
          "\"screen\",\"datas\":[{"
       << "\"has-text\":false,\"image-data\":[";
    for (int i = 0; i < 640; i++)
      ss << "0" << (i == 639 ? "" : ",");
    ss << "]}]}]}";
    Post("bind_game_event", ss.str());
  }

  void SendFrame(const std::vector<int> &bitmap, int value) {
    std::stringstream ss;
    ss << "{\"game\":\"" << GameName
       << "\",\"event\":\"AUDIO_UPDATE\",\"data\":{\"value\":" << value
       << ",\"frame\":{\"image-data-128x40\":[";
    for (size_t i = 0; i < bitmap.size(); ++i) {
      ss << bitmap[i] << (i == bitmap.size() - 1 ? "" : ",");
    }
    ss << "]}}}";
    Post("game_event", ss.str());
  }

  void Heartbeat() {
    std::string payload = "{\"game\":\"" + GameName + "\"}";
    Post("game_heartbeat", payload);
  }
};
