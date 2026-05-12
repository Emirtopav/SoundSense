#include "SceneModel.hpp"
#include <algorithm>

SceneModel::SceneModel(QObject* parent) : QAbstractListModel(parent) {}

int SceneModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(g_Scene.widgets.size());
}

QVariant SceneModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= g_Scene.widgets.size()) return QVariant();

    const auto& w = g_Scene.widgets[index.row()];
    switch (role) {
        case NameRole: return QString::fromStdString(w->name);
        case VisibleRole: return w->visible;
        case LockedRole: return w->locked;
        case XRole: return w->x;
        case YRole: return w->y;
        case TypeRole: return QString::fromStdString(w->name);
        case ReactiveRole: return w->reactive;
        case BandStartRole: return w->bandStart;
        case BandEndRole: return w->bandEnd;
        case SensRole: return w->sensitivity;
        case LerpRole: return w->lerpSpeed;
        case TargetRole: return w->reactiveTarget;
        default: return QVariant();
    }
}

bool SceneModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid() || index.row() >= g_Scene.widgets.size()) return false;

    auto& w = g_Scene.widgets[index.row()];
    bool changed = false;

    if (role == VisibleRole) {
        w->visible = value.toBool();
        changed = true;
    } else if (role == LockedRole) {
        w->locked = value.toBool();
        changed = true;
    } else if (role == XRole) {
        w->x = value.toInt();
        changed = true;
    } else if (role == YRole) {
        w->y = value.toInt();
        changed = true;
    } else if (role == NameRole) {
        w->name = value.toString().toStdString();
        changed = true;
    } else if (role == ReactiveRole) {
        w->reactive = value.toBool();
        changed = true;
    } else if (role == BandStartRole) {
        w->bandStart = value.toInt();
        changed = true;
    } else if (role == BandEndRole) {
        w->bandEnd = value.toInt();
        changed = true;
    } else if (role == SensRole) {
        w->sensitivity = value.toFloat();
        changed = true;
    } else if (role == LerpRole) {
        w->lerpSpeed = value.toFloat();
        changed = true;
    } else if (role == TargetRole) {
        w->reactiveTarget = value.toInt();
        changed = true;
    }

    if (changed) {
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

QHash<int, QByteArray> SceneModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[VisibleRole] = "visible";
    roles[LockedRole] = "locked";
    roles[XRole] = "xPos";
    roles[YRole] = "yPos";
    roles[TypeRole] = "type";
    roles[ReactiveRole] = "reactive";
    roles[BandStartRole] = "bandStart";
    roles[BandEndRole] = "bandEnd";
    roles[SensRole] = "sensitivity";
    roles[LerpRole] = "lerpSpeed";
    roles[TargetRole] = "reactiveTarget";
    return roles;
}

void SceneModel::moveUp(int index) {
    if (index > 0 && index < g_Scene.widgets.size()) {
        beginMoveRows(QModelIndex(), index, index, QModelIndex(), index - 1);
        std::swap(g_Scene.widgets[index], g_Scene.widgets[index - 1]);
        endMoveRows();
    }
}

void SceneModel::moveDown(int index) {
    if (index >= 0 && index < g_Scene.widgets.size() - 1) {
        beginMoveRows(QModelIndex(), index, index, QModelIndex(), index + 2);
        std::swap(g_Scene.widgets[index], g_Scene.widgets[index + 1]);
        endMoveRows();
    }
}

void SceneModel::move(int from, int to) {
    if (from >= 0 && from < g_Scene.widgets.size() && to >= 0 && to < g_Scene.widgets.size() && from != to) {
        int insertPos = to > from ? to + 1 : to;
        beginMoveRows(QModelIndex(), from, from, QModelIndex(), insertPos);
        auto item = std::move(g_Scene.widgets[from]);
        g_Scene.widgets.erase(g_Scene.widgets.begin() + from);
        g_Scene.widgets.insert(g_Scene.widgets.begin() + to, std::move(item));
        endMoveRows();
    }
}

void SceneModel::remove(int index) {
    if (index >= 0 && index < g_Scene.widgets.size()) {
        beginRemoveRows(QModelIndex(), index, index);
        g_Scene.RemoveWidget(index);
        endRemoveRows();
    }
}

void SceneModel::duplicate(int index) {
    if (index >= 0 && index < g_Scene.widgets.size()) {
        beginInsertRows(QModelIndex(), index + 1, index + 1);
        auto clone = g_Scene.widgets[index]->clone();
        clone->name += " (Copy)";
        g_Scene.widgets.insert(g_Scene.widgets.begin() + index + 1, std::move(clone));
        endInsertRows();
    }
}

void SceneModel::rename(int index, const QString& newName) {
    if (index >= 0 && index < g_Scene.widgets.size()) {
        g_Scene.widgets[index]->name = newName.toStdString();
        emit dataChanged(createIndex(index, 0), createIndex(index, 0), {NameRole});
    }
}

void SceneModel::refresh() {
    beginResetModel();
    endResetModel();
}
