#include "GameSenseClient.hpp"
#include "Globals.hpp"
#include <thread>

void GameSenseClient::DiscoverAddress() {
  char *appData = getenv("PROGRAMDATA");
  if (!appData)
    return;
    
  std::string path = std::string(appData) +
                     "\\SteelSeries\\SteelSeries Engine 3\\coreProps.json";

  std::ifstream file(path);
  if (!file.is_open()) {
      // Try GG path as fallback
      path = std::string(appData) + "\\SteelSeries\\GG\\coreProps.json";
      file.open(path);
      if (!file.is_open()) return;
  }

  try {
      json j;
      file >> j;
      if (j.contains("address")) {
          _address = j["address"];
          size_t colon = _address.find(":");
          if (colon != std::string::npos) {
              _host = _address.substr(0, colon);
              _port = std::stoi(_address.substr(colon + 1));
          }
      }
  } catch (...) {
      // Silent failure
  }
}

bool GameSenseClient::Post(const std::string &endpoint, const std::string &jsonPayload) {
  if (!hConnect) return false;

  std::wstring wEndpoint(endpoint.begin(), endpoint.end());
  std::wstring wPath = L"/" + wEndpoint;
  

  HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", wPath.c_str(), NULL,
                                         WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
  if (!hRequest) {
      std::cerr << "[GameSenseClient] WinHttpOpenRequest failed" << std::endl;
      return false;
  }

  LPCWSTR header = L"Content-Type: application/json\r\n";
  
  BOOL result = WinHttpSendRequest(
      hRequest, header, -1L, (LPVOID)jsonPayload.c_str(), 
      (DWORD)jsonPayload.length(), (DWORD)jsonPayload.length(), 0);

  if (result) {
    result = WinHttpReceiveResponse(hRequest, NULL);
  } else {
    std::cerr << "[GameSenseClient] WinHttpSendRequest failed with error: " << GetLastError() << std::endl;
  }
  
  if (!result) {
    g_SteelSeriesConnected = false;
  } else {
    g_SteelSeriesConnected = true;
  }

  WinHttpCloseHandle(hRequest);
  return result;
}

GameSenseClient::GameSenseClient() : _host("127.0.0.1"), _port(0), hSession(NULL), hConnect(NULL) {
  DiscoverAddress();
  
  if (_port == 0) {
      std::cerr << "[GameSenseClient] ERROR: Invalid port discovered. GameSense integration disabled." << std::endl;
      return;
  }

  hSession = WinHttpOpen(L"SoundSense/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                         WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
  
  if (!hSession) {
      std::cerr << "[GameSenseClient] ERROR: WinHttpOpen failed." << std::endl;
      return;
  }

  // Ultra-low latency timeouts: (Resolve, Connect, Send, Receive) in ms
  WinHttpSetTimeouts(hSession, 100, 100, 100, 100);

  std::wstring wHost(_host.begin(), _host.end());
  hConnect = WinHttpConnect(hSession, wHost.c_str(), _port, 0);
  
  if (!hConnect) {
      std::cerr << "[GameSenseClient] ERROR: WinHttpConnect failed." << std::endl;
  }
}

GameSenseClient::~GameSenseClient() {
  if (hConnect)
    WinHttpCloseHandle(hConnect);
  if (hSession)
    WinHttpCloseHandle(hSession);
}

void GameSenseClient::RegisterGame() {
  // Clear any existing session first
  Post("stop_game", "{\"game\":\"" + GameName + "\"}");
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  std::string payload =
      "{\"game\":\"" + GameName +
      "\",\"game_display_name\":\"SoundSense "
      "Visualizer\",\"developer\":\"Emirtopav\",\"icon_color_id\":5}";
  Post("game_metadata", payload);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void GameSenseClient::RegisterEvent() {
  std::string payload =
      "{\"game\":\"" + GameName +
      "\",\"event\":\"AUDIO_UPDATE\",\"value_optional\":true}";
  Post("register_game_event", payload);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void GameSenseClient::BindEvent() {
  // Matching legacy working version's BindEvent exactly
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

void GameSenseClient::SendFrame(const std::vector<int> &bitmap, int value) {
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

void GameSenseClient::Heartbeat() {
  std::string payload = "{\"game\":\"" + GameName + "\"}";
  Post("game_heartbeat", payload);
}
