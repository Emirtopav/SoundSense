#pragma once

#include <QAbstractListModel>
#include "Globals.hpp"

class PluginModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum PluginRoles {
        NameRole = Qt::UserRole + 1,
        EnabledRole,
        DescriptionRole,
        VersionRole,
        AuthorRole
    };

    explicit PluginModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void moveUp(int index);
    Q_INVOKABLE void moveDown(int index);

public slots:
    void refresh();
};
