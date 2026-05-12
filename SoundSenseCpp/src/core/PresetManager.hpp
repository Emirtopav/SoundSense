#pragma once

#include <string>
#include <vector>
#include <QObject>
#include "PresetModel.hpp"

class PresetManager : public QObject {
    Q_OBJECT
public:
    explicit PresetManager(QObject* parent = nullptr);
    
    Q_INVOKABLE void savePreset(const QString& name);
    Q_INVOKABLE void applyPreset(const QString& name);
    Q_INVOKABLE void refreshPresets();

    PresetModel* model() { return &m_model; }

private:
    PresetModel m_model;
    std::string m_presetsDir;
};

extern PresetManager* g_PresetManager;
