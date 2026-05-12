#pragma once

#include <QAbstractListModel>
#include "Globals.hpp"

class SceneModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum WidgetRoles {
        NameRole = Qt::UserRole + 1,
        VisibleRole,
        LockedRole,
        XRole,
        YRole,
        TypeRole,
        ReactiveRole,
        BandStartRole,
        BandEndRole,
        SensRole,
        LerpRole,
        TargetRole
    };

    explicit SceneModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void moveUp(int index);
    Q_INVOKABLE void moveDown(int index);
    Q_INVOKABLE void move(int from, int to);
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE void duplicate(int index);
    Q_INVOKABLE void rename(int index, const QString& newName);

public slots:
    void refresh();
};
