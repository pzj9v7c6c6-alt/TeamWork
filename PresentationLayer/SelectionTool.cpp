#include "SelectionTool.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QPainterPath>
#include <vector>
#include <algorithm>

SelectionTool::SelectionTool(QGraphicsView* view, QGraphicsScene* scene, QObject* parent)
    : QObject(parent)
    , m_view(view)
    , m_scene(scene)
{
}

SelectionTool::~SelectionTool()
{
    clearSelection();
}

void SelectionTool::setShape(Shape shape)
{
    m_shape = shape;
    clearSelection();
}

SelectionTool::Shape SelectionTool::shape() const
{
    return m_shape;
}

void SelectionTool::setTolerance(int tolerance)
{
    int newTol = qBound(0, tolerance, 255);
    if (m_tolerance == newTol) return;
    m_tolerance = newTol;

    if (m_active && m_shape == MagicWand && m_hasMagicSeed) {
        runMagicWand(m_lastMagicSeed);
    }
}

int SelectionTool::tolerance() const
{
    return m_tolerance;
}

bool SelectionTool::hasSelection() const
{
    if (m_shape == MagicWand) {
        return !m_magicMask.isNull() && !m_selectionRect.isEmpty();
    }
    return !m_selectionRect.isEmpty();
}

QRectF SelectionTool::selectionRect() const
{
    return m_selectionRect;
}

bool SelectionTool::isActive() const
{
    return m_active;
}

void SelectionTool::setActive(bool active)
{
    m_active = active;
    if (!active) clearSelection();
}

void SelectionTool::setSourceImage(const QImage& img)
{
    m_sourceImage = img.convertToFormat(QImage::Format_ARGB32);
}

void SelectionTool::clearSelection()
{
    m_hasMagicSeed = false;
    m_selectionRect = QRectF();
    m_magicMask = QImage();

    if (m_rectOverlay) {
        m_scene->removeItem(m_rectOverlay);
        delete m_rectOverlay;
        m_rectOverlay = nullptr;
    }
    if (m_ellipseOverlay) {
        m_scene->removeItem(m_ellipseOverlay);
        delete m_ellipseOverlay;
        m_ellipseOverlay = nullptr;
    }
    if (m_magicOverlay) {
        m_scene->removeItem(m_magicOverlay);
        delete m_magicOverlay;
        m_magicOverlay = nullptr;
    }
    emit selectionChanged();
}

bool SelectionTool::handleMousePress(QPointF scenePos)
{
    if (!m_active) return false;

    if (m_shape == MagicWand) {
        if (m_sourceImage.isNull()) return false;

        int w = m_sourceImage.width();
        int h = m_sourceImage.height();
        int seedX = static_cast<int>(scenePos.x());
        int seedY = static_cast<int>(scenePos.y());

        if (seedX < 0 || seedX >= w || seedY < 0 || seedY >= h)
            return false;

        m_lastMagicSeed = QPoint(seedX, seedY);
        m_hasMagicSeed = true;

        runMagicWand(m_lastMagicSeed);
        return true;
    }

    m_hasMagicSeed = false;
    m_drawing   = true;
    m_startPos  = scenePos;
    m_selectionRect = QRectF(scenePos, scenePos);
    updateOverlay();
    return true;
}

void SelectionTool::runMagicWand(const QPoint& seed)
{
    if (m_sourceImage.isNull()) return;

    int w = m_sourceImage.width();
    int h = m_sourceImage.height();
    int seedX = seed.x();
    int seedY = seed.y();

    if (seedX < 0 || seedX >= w || seedY < 0 || seedY >= h)
        return;

    const uint32_t* srcBits = reinterpret_cast<const uint32_t*>(m_sourceImage.constBits());
    uint32_t targetColor = srcBits[seedY * w + seedX];
    int targetR = (targetColor >> 16) & 0xFF;
    int targetG = (targetColor >> 8) & 0xFF;
    int targetB = targetColor & 0xFF;

    // Color tolerance threshold: Euclidean RGB distance squared
    const int tolerance = m_tolerance;
    const int tolSq = tolerance * tolerance * 3;

    // Visited status: 0 = unvisited, 1 = visited but out of tolerance, 255 = in selection
    std::vector<uint8_t> visited(static_cast<size_t>(w) * h, 0);

    // Memory-safe index stack
    std::vector<int> stack;
    stack.reserve(4096);

    int seedIdx = seedY * w + seedX;
    visited[seedIdx] = 255;
    stack.push_back(seedIdx);

    int minX = seedX, maxX = seedX;
    int minY = seedY, maxY = seedY;

    while (!stack.empty()) {
        int curIdx = stack.back();
        stack.pop_back();

        int px = curIdx % w;
        int py = curIdx / w;

        // East
        if (px + 1 < w) {
            int nIdx = curIdx + 1;
            if (!visited[nIdx]) {
                uint32_t col = srcBits[nIdx];
                int dr = static_cast<int>((col >> 16) & 0xFF) - targetR;
                int dg = static_cast<int>((col >> 8) & 0xFF) - targetG;
                int db = static_cast<int>(col & 0xFF) - targetB;
                if ((dr * dr + dg * dg + db * db) <= tolSq) {
                    visited[nIdx] = 255;
                    if (px + 1 > maxX) maxX = px + 1;
                    stack.push_back(nIdx);
                } else {
                    visited[nIdx] = 1;
                }
            }
        }

        // West
        if (px - 1 >= 0) {
            int nIdx = curIdx - 1;
            if (!visited[nIdx]) {
                uint32_t col = srcBits[nIdx];
                int dr = static_cast<int>((col >> 16) & 0xFF) - targetR;
                int dg = static_cast<int>((col >> 8) & 0xFF) - targetG;
                int db = static_cast<int>(col & 0xFF) - targetB;
                if ((dr * dr + dg * dg + db * db) <= tolSq) {
                    visited[nIdx] = 255;
                    if (px - 1 < minX) minX = px - 1;
                    stack.push_back(nIdx);
                } else {
                    visited[nIdx] = 1;
                }
            }
        }

        // South
        if (py + 1 < h) {
            int nIdx = curIdx + w;
            if (!visited[nIdx]) {
                uint32_t col = srcBits[nIdx];
                int dr = static_cast<int>((col >> 16) & 0xFF) - targetR;
                int dg = static_cast<int>((col >> 8) & 0xFF) - targetG;
                int db = static_cast<int>(col & 0xFF) - targetB;
                if ((dr * dr + dg * dg + db * db) <= tolSq) {
                    visited[nIdx] = 255;
                    if (py + 1 > maxY) maxY = py + 1;
                    stack.push_back(nIdx);
                } else {
                    visited[nIdx] = 1;
                }
            }
        }

        // North
        if (py - 1 >= 0) {
            int nIdx = curIdx - w;
            if (!visited[nIdx]) {
                uint32_t col = srcBits[nIdx];
                int dr = static_cast<int>((col >> 16) & 0xFF) - targetR;
                int dg = static_cast<int>((col >> 8) & 0xFF) - targetG;
                int db = static_cast<int>(col & 0xFF) - targetB;
                if ((dr * dr + dg * dg + db * db) <= tolSq) {
                    visited[nIdx] = 255;
                    if (py - 1 < minY) minY = py - 1;
                    stack.push_back(nIdx);
                } else {
                    visited[nIdx] = 1;
                }
            }
        }
    }

    // Create the mask image with direct scanline access
    m_magicMask = QImage(w, h, QImage::Format_Alpha8);
    m_magicMask.fill(0);

    for (int y = minY; y <= maxY; ++y) {
        uchar* maskLine = m_magicMask.scanLine(y);
        const uint8_t* visRow = &visited[static_cast<size_t>(y) * w];
        for (int x = minX; x <= maxX; ++x) {
            if (visRow[x] == 255) {
                maskLine[x] = 255;
            }
        }
    }

    m_selectionRect = QRectF(minX, minY, maxX - minX + 1, maxY - minY + 1);
    updateOverlay();
    emit selectionChanged();
}

bool SelectionTool::handleMouseMove(QPointF scenePos)
{
    if (!m_active || !m_drawing || m_shape == MagicWand) return false;
    m_selectionRect = QRectF(m_startPos, scenePos).normalized();
    updateOverlay();
    return true;
}

bool SelectionTool::handleMouseRelease(QPointF /*scenePos*/)
{
    if (!m_active || m_shape == MagicWand) return false;
    m_drawing = false;
    emit selectionChanged();
    return true;
}

void SelectionTool::updateOverlay()
{
    QPen pen(QColor("#22c55e"), 1.5, Qt::DashLine);
    QBrush brush(QColor(34, 197, 94, 35));

    if (m_shape == MagicWand) {
        if (m_rectOverlay) {
            m_scene->removeItem(m_rectOverlay);
            delete m_rectOverlay;
            m_rectOverlay = nullptr;
        }
        if (m_ellipseOverlay) {
            m_scene->removeItem(m_ellipseOverlay);
            delete m_ellipseOverlay;
            m_ellipseOverlay = nullptr;
        }

        if (m_magicMask.isNull() || m_selectionRect.isEmpty()) {
            if (m_magicOverlay) {
                m_scene->removeItem(m_magicOverlay);
                delete m_magicOverlay;
                m_magicOverlay = nullptr;
            }
            return;
        }

        int minX = static_cast<int>(m_selectionRect.left());
        int maxX = static_cast<int>(m_selectionRect.right());
        int minY = static_cast<int>(m_selectionRect.top());
        int maxY = static_cast<int>(m_selectionRect.bottom());

        minX = qMax(0, minX);
        maxX = qMin(m_magicMask.width() - 1, maxX);
        minY = qMax(0, minY);
        maxY = qMin(m_magicMask.height() - 1, maxY);

        int bw = maxX - minX + 1;
        int bh = maxY - minY + 1;

        if (bw <= 0 || bh <= 0) {
            if (m_magicOverlay) {
                m_scene->removeItem(m_magicOverlay);
                delete m_magicOverlay;
                m_magicOverlay = nullptr;
            }
            return;
        }

        // Bounded overlay: only allocate memory for the selection's bounding box
        QImage overlay(bw, bh, QImage::Format_ARGB32_Premultiplied);
        overlay.fill(Qt::transparent);

        const uint32_t greenTint = 0x5A22C55E; // ~35% alpha green
        for (int y = minY; y <= maxY; ++y) {
            const uchar* maskLine = m_magicMask.constScanLine(y);
            uint32_t* ovLine = reinterpret_cast<uint32_t*>(overlay.scanLine(y - minY));
            for (int x = minX; x <= maxX; ++x) {
                if (maskLine[x] > 0) {
                    ovLine[x - minX] = greenTint;
                }
            }
        }

        if (!m_magicOverlay) {
            m_magicOverlay = m_scene->addPixmap(QPixmap::fromImage(overlay));
            m_magicOverlay->setZValue(100);
        } else {
            m_magicOverlay->setPixmap(QPixmap::fromImage(overlay));
        }
        m_magicOverlay->setPos(minX, minY);
        if (m_scene) m_scene->update();
        return;
    }

    if (m_magicOverlay) {
        m_scene->removeItem(m_magicOverlay);
        delete m_magicOverlay;
        m_magicOverlay = nullptr;
    }

    if (m_shape == Rectangle) {
        if (!m_rectOverlay) {
            m_rectOverlay = m_scene->addRect(m_selectionRect, pen, brush);
            m_rectOverlay->setZValue(100);
        } else {
            m_rectOverlay->setRect(m_selectionRect);
        }
        if (m_ellipseOverlay) {
            m_scene->removeItem(m_ellipseOverlay);
            delete m_ellipseOverlay;
            m_ellipseOverlay = nullptr;
        }
    } else {
        if (!m_ellipseOverlay) {
            m_ellipseOverlay = m_scene->addEllipse(m_selectionRect, pen, brush);
            m_ellipseOverlay->setZValue(100);
        } else {
            m_ellipseOverlay->setRect(m_selectionRect);
        }
        if (m_rectOverlay) {
            m_scene->removeItem(m_rectOverlay);
            delete m_rectOverlay;
            m_rectOverlay = nullptr;
        }
    }
}

// ---------------------------------------------------------------------------
// Compositing
// ---------------------------------------------------------------------------
QImage SelectionTool::compositeMasked(const QImage& baseImage,
                                      const QImage& filteredImage,
                                      Mode mode) const
{
    if (baseImage.isNull() || filteredImage.isNull()) return baseImage;
    if (!hasSelection()) return filteredImage; // no selection → full filter

    QImage base = baseImage.convertToFormat(QImage::Format_ARGB32);
    QImage filt = filteredImage.scaled(base.size(), Qt::IgnoreAspectRatio,
                                       Qt::SmoothTransformation)
                               .convertToFormat(QImage::Format_ARGB32);
    QImage result = base;

    if (m_shape == MagicWand && !m_magicMask.isNull()) {
        int w = base.width();
        int h = base.height();
        int maskW = m_magicMask.width();
        int maskH = m_magicMask.height();

        for (int y = 0; y < h; ++y) {
            const uint32_t* filtLine = reinterpret_cast<const uint32_t*>(filt.constScanLine(y));
            uint32_t* resLine = reinterpret_cast<uint32_t*>(result.scanLine(y));
            const uchar* maskLine = (y < maskH) ? m_magicMask.constScanLine(y) : nullptr;

            for (int x = 0; x < w; ++x) {
                bool inside = (maskLine && x < maskW && maskLine[x] > 0);
                if ((mode == InsideSelection && inside) || (mode == OutsideSelection && !inside)) {
                    resLine[x] = filtLine[x];
                }
            }
        }
        return result;
    }

    QRect imgRect(0, 0, base.width(), base.height());
    QRect selRect = m_selectionRect.toAlignedRect().intersected(imgRect);

    if (selRect.isEmpty()) return baseImage;

    QPainter p(&result);

    if (m_shape == Rectangle) {
        if (mode == InsideSelection) {
            p.drawImage(selRect, filt, selRect);
        } else {
            p.drawImage(imgRect, filt, imgRect);
            p.drawImage(selRect, base, selRect);
        }
    } else {
        // Ellipse mask via clipPath
        QPainterPath ellPath;
        ellPath.addEllipse(QRectF(selRect));

        if (mode == InsideSelection) {
            p.setClipPath(ellPath, Qt::IntersectClip);
            p.drawImage(imgRect, filt, imgRect);
        } else {
            p.drawImage(imgRect, filt, imgRect);
            p.setClipPath(ellPath, Qt::IntersectClip);
            p.drawImage(imgRect, base, imgRect);
        }
    }
    p.end();
    return result;
}
