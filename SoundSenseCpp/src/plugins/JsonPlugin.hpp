#pragma once

#include "SoundSenseSDK/SoundSenseSDK.hpp"
#include <string>
#include <vector>
#include "vendor/json/json.hpp"

using json = nlohmann::json;

class JsonPlugin : public IPlugin {
public:
    JsonPlugin(const std::string& sceneJsonPath);
    ~JsonPlugin();

    const char* GetName() const override;
    const char* GetDescription() const override { return m_Description.c_str(); }
    const char* GetVersion() const override { return m_Version.c_str(); }
    const char* GetAuthor() const override { return m_Author.c_str(); }
    void Initialize() override;
    bool OverridesBase() const override;
    void Process(const std::vector<float>& audioBands, std::vector<int>& bitmap) override;

    bool LoadFromJson(const std::string& filePath);

private:
    std::string m_Name;
    std::string m_Description = "Custom Scene Plugin";
    std::string m_Version = "1.0.0";
    std::string m_Author = "SoundSense";
    std::string m_FilePath;
    json m_SceneData;
    bool m_OverridesBase;
    
    // Internal state for effects
    float m_TimeCounter = 0.0f;
    int m_FrameCounter = 0;
};
