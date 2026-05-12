//       _____    _____  _____   _
//      / ____|  / ____|/ ____| | |
//     | |  __  | (___ | |      | |__  _ __  _ __
//     | | |_ |  \___ \| |      | '_ \| '_ \| '_ \ 
//     | |__| |_ ____) | |____ _| | | | |_) | |_) |
//      \_____(_)_____(_)_____(_)_| |_| .__/| .__/
//                                    | |   | |
//                                    |_|   |_|
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
  const std::string GameName = "SOUNDSENSE_APP";

  HINTERNET hSession = NULL;
  HINTERNET hConnect = NULL;

  void DiscoverAddress();
  bool Post(const std::string &endpoint, const std::string &jsonPayload);

public:
  GameSenseClient();
  ~GameSenseClient();

  void RegisterGame();
  void RegisterEvent();
  void BindEvent();
  void SendFrame(const std::vector<int> &bitmap, int value);
  void Heartbeat();
};
