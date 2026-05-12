#pragma once

#include <QQuickPaintedItem>
#include <QTimer>
#include <chrono>

class LivePreviewItem : public QQuickPaintedItem {
    Q_OBJECT

public:
    LivePreviewItem(QQuickItem* parent = nullptr);
    void paint(QPainter* painter) override;

public slots:
    void updatePreview();

private:
    QTimer m_timer;
    int m_frameCount = 0;
    std::chrono::steady_clock::time_point m_lastFpsTime;
};
