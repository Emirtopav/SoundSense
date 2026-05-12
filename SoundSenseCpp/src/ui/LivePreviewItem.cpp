#include "LivePreviewItem.hpp"
#include <QPainter>
#include <QColor>
#include <QPen>
#include <QBrush>
#include <mutex>
#include "Globals.hpp"

LivePreviewItem::LivePreviewItem(QQuickItem* parent) : QQuickPaintedItem(parent) {
    connect(&m_timer, &QTimer::timeout, this, &LivePreviewItem::updatePreview);
    m_timer.start(16); // ~60 FPS
    m_lastFpsTime = std::chrono::steady_clock::now();
}

void LivePreviewItem::paint(QPainter* painter) {
    painter->fillRect(boundingRect(), QColor(20, 20, 20)); // Background

    m_frameCount++;
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now - m_lastFpsTime).count() >= 1) {
        g_RenderFps = m_frameCount;
        m_frameCount = 0;
        m_lastFpsTime = now;
    }

    QImage img(128, 40, QImage::Format_Mono);
    img.fill(0);

    {
        std::lock_guard<std::mutex> lock(g_BitmapMutex);
        for (int y = 0; y < 40; y++) {
            uchar* line = img.scanLine(y);
            for (int x = 0; x < 16; x++) {
                int byteIdx = (y * 16) + x;
                if (byteIdx < (int)g_CurrentBitmap.size()) {
                    line[x] = (uchar)g_CurrentBitmap[byteIdx];
                }
            }
        }
    }

    painter->setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter->drawImage(boundingRect(), img);
}

void LivePreviewItem::updatePreview() {
    update();
}
