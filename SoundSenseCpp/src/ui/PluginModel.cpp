#include "PluginModel.hpp"
#include <algorithm>

PluginModel::PluginModel(QObject* parent) : QAbstractListModel(parent) {}

int PluginModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    std::lock_guard<std::recursive_mutex> lock(g_PluginMutex);
    return static_cast<int>(g_PluginManager.loadedPlugins.size());
}

QVariant PluginModel::data(const QModelIndex& index, int role) const {
    std::lock_guard<std::recursive_mutex> lock(g_PluginMutex);
    if (!index.isValid() || index.row() >= g_PluginManager.loadedPlugins.size()) return QVariant();

    const auto& inst = g_PluginManager.loadedPlugins[index.row()];
    switch (role) {
        case NameRole: return QString(inst.plugin->GetName());
        case EnabledRole: return inst.enabled;
        case DescriptionRole: return QString(inst.plugin->GetDescription());
        case VersionRole: return QString(inst.plugin->GetVersion());
        case AuthorRole: return QString(inst.plugin->GetAuthor());
        default: return QVariant();
    }
}

bool PluginModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    std::lock_guard<std::recursive_mutex> lock(g_PluginMutex);
    if (!index.isValid() || index.row() >= g_PluginManager.loadedPlugins.size()) return false;

    auto& inst = g_PluginManager.loadedPlugins[index.row()];
    if (role == EnabledRole) {
        inst.enabled = value.toBool();
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

QHash<int, QByteArray> PluginModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[EnabledRole] = "enabled";
    roles[DescriptionRole] = "description";
    roles[VersionRole] = "version";
    roles[AuthorRole] = "author";
    return roles;
}

void PluginModel::moveUp(int index) {
    std::lock_guard<std::recursive_mutex> lock(g_PluginMutex);
    if (index > 0 && index < g_PluginManager.loadedPlugins.size()) {
        beginMoveRows(QModelIndex(), index, index, QModelIndex(), index - 1);
        std::swap(g_PluginManager.loadedPlugins[index], g_PluginManager.loadedPlugins[index - 1]);
        endMoveRows();
    }
}

void PluginModel::moveDown(int index) {
    std::lock_guard<std::recursive_mutex> lock(g_PluginMutex);
    if (index >= 0 && index < g_PluginManager.loadedPlugins.size() - 1) {
        beginMoveRows(QModelIndex(), index, index, QModelIndex(), index + 2);
        std::swap(g_PluginManager.loadedPlugins[index], g_PluginManager.loadedPlugins[index + 1]);
        endMoveRows();
    }
}

void PluginModel::refresh() {
    beginResetModel();
    endResetModel();
}
