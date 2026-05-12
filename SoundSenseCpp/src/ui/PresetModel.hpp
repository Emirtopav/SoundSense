#pragma once

#include <QAbstractListModel>
#include <string>
#include <vector>

class PresetModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum PresetRoles { NameRole = Qt::UserRole + 1 };
    explicit PresetModel(QObject* parent = nullptr);
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    void refresh();
private:
    std::vector<std::string> m_presets;
};
