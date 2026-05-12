#pragma once

#include <QObject>
#include <QString>
#include "Globals.hpp"
#include "Settings.hpp"
#include "SceneModel.hpp"
#include "PluginModel.hpp"
#include "PresetModel.hpp"
#include <QTimer>

class BackendContext : public QObject {
    Q_OBJECT
    
    Q_PROPERTY(float bassGain READ bassGain WRITE setBassGain NOTIFY bassGainChanged)
    Q_PROPERTY(float midGain READ midGain WRITE setMidGain NOTIFY midGainChanged)
    Q_PROPERTY(float trebleGain READ trebleGain WRITE setTrebleGain NOTIFY trebleGainChanged)
    Q_PROPERTY(float globalSens READ globalSens WRITE setGlobalSens NOTIFY globalSensChanged)
    Q_PROPERTY(int mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(float falloff READ falloff WRITE setFalloff NOTIFY falloffChanged)
    Q_PROPERTY(float smoothing READ smoothing WRITE setSmoothing NOTIFY smoothingChanged)
    Q_PROPERTY(bool showPeaks READ showPeaks WRITE setShowPeaks NOTIFY showPeaksChanged)
    Q_PROPERTY(int brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
    Q_PROPERTY(int rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(QStringList availablePorts READ availablePorts NOTIFY availablePortsChanged)
    
    Q_PROPERTY(float lfoFreq READ lfoFreq WRITE setLfoFreq NOTIFY lfoFreqChanged)
    Q_PROPERTY(float lfoAmp READ lfoAmp WRITE setLfoAmp NOTIFY lfoAmpChanged)
    
    // Telemetry
    Q_PROPERTY(bool espConnected READ espConnected NOTIFY espConnectedChanged)
    Q_PROPERTY(bool ssConnected READ ssConnected NOTIFY ssConnectedChanged)
    Q_PROPERTY(int fps READ fps NOTIFY fpsChanged)
    Q_PROPERTY(int latency READ latency NOTIFY latencyChanged)
    Q_PROPERTY(bool audioActive READ audioActive NOTIFY audioActiveChanged)
    Q_PROPERTY(int selectedLayerIndex READ selectedLayerIndex WRITE setSelectedLayerIndex NOTIFY selectedLayerIndexChanged)
    Q_PROPERTY(int renderTimeMs READ renderTimeMs NOTIFY renderTimeMsChanged)
    Q_PROPERTY(int droppedFrames READ droppedFrames NOTIFY droppedFramesChanged)

    Q_PROPERTY(SceneModel* sceneModel READ sceneModel CONSTANT)
    Q_PROPERTY(PluginModel* pluginModel READ pluginModel CONSTANT)
    Q_PROPERTY(PresetModel* presetModel READ presetModel CONSTANT)

public:
    explicit BackendContext(QObject* parent = nullptr);

    float bassGain() const;
    void setBassGain(float val);

    float midGain() const;
    void setMidGain(float val);

    float trebleGain() const;
    void setTrebleGain(float val);

    float globalSens() const;
    void setGlobalSens(float val);

    int mode() const;
    void setMode(int val);

    float falloff() const;
    void setFalloff(float val);

    float smoothing() const;
    void setSmoothing(float val);

    bool showPeaks() const;
    void setShowPeaks(bool value);

    int brightness() const;
    void setBrightness(int value);

    int rotation() const;
    void setRotation(int value);

    QStringList availablePorts() const;
    Q_INVOKABLE void refreshPorts();
    Q_INVOKABLE void flashFirmware();
    Q_INVOKABLE void connectSerial(const QString& portName);
    Q_INVOKABLE void disconnectSerial();

    float lfoFreq() const;
    void setLfoFreq(float val);

    float lfoAmp() const;
    void setLfoAmp(float val);

    bool espConnected() const;
    bool ssConnected() const;
    int fps() const;
    int latency() const;
    bool audioActive() const;
    int renderTimeMs() const;
    int droppedFrames() const;
    
    Q_INVOKABLE void resetSettings();
    Q_INVOKABLE void resetDroppedFrames();
    
    int selectedLayerIndex() const;
    void setSelectedLayerIndex(int index);

    SceneModel* sceneModel() { return &m_sceneModel; }
    PluginModel* pluginModel() { return &m_pluginModel; }
    PresetModel* presetModel();

    Q_INVOKABLE void savePreset(const QString& name);
    Q_INVOKABLE void applyPreset(const QString& name);

    Q_INVOKABLE void addVisualizer();
    Q_INVOKABLE void addClock();
    Q_INVOKABLE void addText();
    Q_INVOKABLE void addRect();
    Q_INVOKABLE void addImage();
    Q_INVOKABLE void addWaveform();
    
    Q_INVOKABLE bool saveScene(const QString &path);
    Q_INVOKABLE bool loadScene(const QString &path);

signals:
    void bassGainChanged();
    void midGainChanged();
    void trebleGainChanged();
    void globalSensChanged();
    void modeChanged();
    void falloffChanged();
    void smoothingChanged();
    void showPeaksChanged();
    
    void espConnectedChanged();
    void ssConnectedChanged();
    void fpsChanged();
    void latencyChanged();
    void audioActiveChanged();
    void selectedLayerIndexChanged();
    void renderTimeMsChanged();
    void droppedFramesChanged();
    void settingsChanged();
    void brightnessChanged();
    void rotationChanged();
    void availablePortsChanged();
    void lfoFreqChanged();
    void lfoAmpChanged();

private slots:
    void updateTelemetry();

private:
    QTimer* m_telemetryTimer;
    SceneModel m_sceneModel;
    PluginModel m_pluginModel;
    int m_selectedLayerIndex = -1;
};
