#include "PresetModel.hpp"
#include <filesystem>

namespace fs = std::filesystem;

PresetModel::PresetModel(QObject* parent) : QAbstractListModel(parent) {}

int PresetModel::rowCount(const QModelIndex& parent) const {
    return static_cast<int>(m_presets.size());
}

QVariant PresetModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_presets.size()) return QVariant();
    if (role == NameRole) return QString::fromStdString(m_presets[index.row()]);
    return QVariant();
}

QHash<int, QByteArray> PresetModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    return roles;
}

void PresetModel::refresh() {
    beginResetModel();
    m_presets.clear();
    fs::path p("presets");
    if (fs::exists(p) && fs::is_directory(p)) {
        for (const auto& entry : fs::directory_iterator(p)) {
            if (entry.path().extension() == ".preset") {
                m_presets.push_back(entry.path().stem().string());
            }
        }
    }
    endResetModel();
}
