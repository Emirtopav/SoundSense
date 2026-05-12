#include "BackendContext.hpp"
#include "PresetManager.hpp"
#include "VisualizerWidget.hpp"
#include "ClockWidget.hpp"
#include "TextWidget.hpp"
#include "RectWidget.hpp"
#include "ImageWidget.hpp"
#include "WaveformWidget.hpp"
#include <QUrl>

BackendContext::BackendContext(QObject* parent) : QObject(parent) {
    m_telemetryTimer = new QTimer(this);
    connect(m_telemetryTimer, &QTimer::timeout, this, &BackendContext::updateTelemetry);
    m_telemetryTimer->start(100); // 10 times per second
}

float BackendContext::bassGain() const { return g_Settings.bassGain; }
void BackendContext::setBassGain(float val) {
    std::lock_guard<std::recursive_mutex> lock(g_SettingsMutex);
    if (g_Settings.bassGain != val) {
        g_Settings.bassGain = val;
        SaveSettings();
        emit bassGainChanged();
    }
}

float BackendContext::midGain() const { return g_Settings.midGain; }
void BackendContext::setMidGain(float val) {
    std::lock_guard<std::recursive_mutex> lock(g_SettingsMutex);
    if (g_Settings.midGain != val) {
        g_Settings.midGain = val;
        SaveSettings();
        emit midGainChanged();
    }
}

float BackendContext::trebleGain() const { return g_Settings.trebleGain; }
void BackendContext::setTrebleGain(float val) {
    std::lock_guard<std::recursive_mutex> lock(g_SettingsMutex);
    if (g_Settings.trebleGain != val) {
        g_Settings.trebleGain = val;
        SaveSettings();
        emit trebleGainChanged();
    }
}

float BackendContext::globalSens() const { return g_Settings.globalSens; }
void BackendContext::setGlobalSens(float val) {
    std::lock_guard<std::recursive_mutex> lock(g_SettingsMutex);
    if (g_Settings.globalSens != val) {
        g_Settings.globalSens = val;
        SaveSettings();
        emit globalSensChanged();
    }
}

int BackendContext::mode() const { return g_Settings.mode; }
void BackendContext::setMode(int val) {
    std::lock_guard<std::recursive_mutex> lock(g_SettingsMutex);
    if (g_Settings.mode != val) {
        g_Settings.mode = val;
        SaveSettings();
        if (g_Serial.IsConnected()) {
            g_Serial.Write("mode," + std::to_string(g_Settings.mode) + "\n");
        }
        emit modeChanged();
    }
}

float BackendContext::falloff() const { return g_Settings.falloff; }
void BackendContext::setFalloff(float val) {
    std::lock_guard<std::recursive_mutex> lock(g_SettingsMutex);
    if (g_Settings.falloff != val) {
        g_Settings.falloff = val;
        SaveSettings();
        emit falloffChanged();
    }
}

float BackendContext::smoothing() const { return g_Settings.smoothing; }
void BackendContext::setSmoothing(float val) {
    std::lock_guard<std::recursive_mutex> lock(g_SettingsMutex);
    if (g_Settings.smoothing != val) {
        g_Settings.smoothing = val;
        SaveSettings();
        emit smoothingChanged();
    }
}

bool BackendContext::showPeaks() const { return g_Settings.showPeaks; }
void BackendContext::setShowPeaks(bool value) {
    std::lock_guard<std::recursive_mutex> lock(g_SettingsMutex);
    if (g_Settings.showPeaks != value) {
        g_Settings.showPeaks = value;
        emit showPeaksChanged();
    }
}

int BackendContext::brightness() const { return g_Settings.brightness; }
void BackendContext::setBrightness(int value) {
    std::lock_guard<std::recursive_mutex> lock(g_SettingsMutex);
    if (g_Settings.brightness != value) {
        g_Settings.brightness = value;
        emit brightnessChanged();
        // Send hardware command
        uint8_t cmd[2] = { 0x02, (uint8_t)value };
        g_Serial.Write(cmd, 2);
    }
}

int BackendContext::rotation() const { return g_Settings.rotation; }
void BackendContext::setRotation(int value) {
    std::lock_guard<std::recursive_mutex> lock(g_SettingsMutex);
    if (g_Settings.rotation != value) {
        g_Settings.rotation = value;
        emit rotationChanged();
        // Send hardware command (mapping 0,90,180,270 to 0,1,2,3)
        uint8_t rotByte = 0;
        if (value == 90) rotByte = 1;
        else if (value == 180) rotByte = 2;
        else if (value == 270) rotByte = 3;
        uint8_t cmd[2] = { 0x03, rotByte };
        g_Serial.Write(cmd, 2);
    }
}

QStringList BackendContext::availablePorts() const {
    QStringList list;
    auto ports = SerialPort::GetAvailablePorts();
    for (const auto& p : ports) list << QString::fromStdString(p);
    return list;
}

void BackendContext::refreshPorts() {
    emit availablePortsChanged();
}

void BackendContext::connectSerial(const QString& portName) {
    g_Serial.Close();
    if (g_Serial.Open(portName.toStdString(), 115200)) {
        Sleep(500); // Kartın kendine gelmesi için bekle
        g_ComPortName = portName.toStdString();
        // Sync hardware state immediately
        setBrightness(g_Settings.brightness);
        setRotation(g_Settings.rotation);
    }
    emit espConnectedChanged();
}

void BackendContext::disconnectSerial() {
    g_Serial.Close();
    emit espConnectedChanged();
}

void BackendContext::flashFirmware() {
    // This will run the bundled flash script
    system("start /min flash_firmware.bat");
}

float BackendContext::lfoFreq() const { return g_SensitivityLFO.frequency; }
void BackendContext::setLfoFreq(float val) {
    if (g_SensitivityLFO.frequency != val) {
        g_SensitivityLFO.frequency = val;
        emit lfoFreqChanged();
    }
}

float BackendContext::lfoAmp() const { return g_SensitivityLFO.amplitude; }
void BackendContext::setLfoAmp(float val) {
    if (g_SensitivityLFO.amplitude != val) {
        g_SensitivityLFO.amplitude = val;
        emit lfoAmpChanged();
    }
}

bool BackendContext::espConnected() const { return g_Serial.IsConnected(); }
bool BackendContext::ssConnected() const { return g_SteelSeriesConnected.load(); }
int BackendContext::fps() const { return g_RenderFps.load(); }
int BackendContext::latency() const { return g_AudioLatencyMs.load(); }
bool BackendContext::audioActive() const { return g_AudioActive.load(); }

int BackendContext::renderTimeMs() const { return g_RenderTimeMs.load(); }
int BackendContext::droppedFrames() const { return g_DroppedFrames.load(); }

int BackendContext::selectedLayerIndex() const { return m_selectedLayerIndex; }
void BackendContext::setSelectedLayerIndex(int index) {
    if (m_selectedLayerIndex != index) {
        m_selectedLayerIndex = index;
        emit selectedLayerIndexChanged();
    }
}

void BackendContext::resetSettings() {
    setBassGain(1.0f);
    setMidGain(1.0f);
    setTrebleGain(1.0f);
    setGlobalSens(1.0f);
    setFalloff(0.4f);
    setSmoothing(0.5f);
    setShowPeaks(true);
}

void BackendContext::resetDroppedFrames() {
    g_DroppedFrames = 0;
    emit droppedFramesChanged();
}

PresetModel* BackendContext::presetModel() {
    return g_PresetManager->model();
}

void BackendContext::savePreset(const QString& name) {
    g_PresetManager->savePreset(name);
}

void BackendContext::applyPreset(const QString& name) {
    g_PresetManager->applyPreset(name);
    
    // Notify all UI properties
    emit bassGainChanged();
    emit midGainChanged();
    emit trebleGainChanged();
    emit globalSensChanged();
    emit falloffChanged();
    emit smoothingChanged();
    emit showPeaksChanged();
    
    m_pluginModel.refresh();
}

void BackendContext::updateTelemetry() {
    emit espConnectedChanged();
    emit ssConnectedChanged();
    emit fpsChanged();
    emit latencyChanged();
    emit audioActiveChanged();
    emit renderTimeMsChanged();
    emit droppedFramesChanged();
}

void BackendContext::addVisualizer() {
    g_Scene.AddWidget(std::make_unique<VisualizerWidget>());
    m_sceneModel.refresh();
}

void BackendContext::addClock() {
    g_Scene.AddWidget(std::make_unique<ClockWidget>());
    m_sceneModel.refresh();
}

void BackendContext::addText() {
    g_Scene.AddWidget(std::make_unique<TextWidget>());
    m_sceneModel.refresh();
}

void BackendContext::addRect() {
    g_Scene.AddWidget(std::make_unique<RectWidget>());
    m_sceneModel.refresh();
}

void BackendContext::addImage() {
    g_Scene.AddWidget(std::make_unique<ImageWidget>());
    m_sceneModel.refresh();
}

void BackendContext::addWaveform() {
    g_Scene.AddWidget(std::make_unique<WaveformWidget>());
    m_sceneModel.refresh();
}

bool BackendContext::saveScene(const QString &path) {
    QString cleanPath = path;
    if (cleanPath.startsWith("file:///")) cleanPath = QUrl(path).toLocalFile();
    return g_Scene.SaveToFile(cleanPath.toStdString());
}

bool BackendContext::loadScene(const QString &path) {
    QString cleanPath = path;
    if (cleanPath.startsWith("file:///")) cleanPath = QUrl(path).toLocalFile();
    bool ok = g_Scene.LoadFromFile(cleanPath.toStdString());
    if (ok) {
        setSelectedLayerIndex(-1);
        m_sceneModel.refresh();
    }
    return ok;
}
