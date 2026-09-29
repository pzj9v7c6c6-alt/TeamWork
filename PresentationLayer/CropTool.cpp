#include "CropTool.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QPen>
#include <QBrush>
#include <QCursor>
#include <algorithm>
#include <cmath>

CropTool::CropTool(QGraphicsView* view, QGraphicsScene* scene, QObject* parent)
    : QObject(parent)
    , m_view(view)
    , m_scene(scene)
{
}

CropTool::~CropTool()
{
    reset();
}

bool CropTool::isActive() const
{
    return m_active;
}

void CropTool::setActive(bool active)
{
    if (m_active == active) return;
    m_active = active;
    if (!m_active) {
        reset();
    }
}

void CropTool::initCrop(const QRectF& imageBounds)
{
    m_imageBounds = imageBounds;
    if (m_imageBounds.isEmpty()) {
        reset();
        return;
    }

    // Default crop rectangle: centered with 10% margin
    qreal marginX = m_imageBounds.width() * 0.1;
    qreal marginY = m_imageBounds.height() * 0.1;
    m_cropRect = m_imageBounds.adjusted(marginX, marginY, -marginX, -marginY);

    m_active = true;
    updateVisuals();
    emit cropRectChanged(m_cropRect);
}

QRectF CropTool::cropRect() const
{
    return m_cropRect;
}

void CropTool::reset()
{
    m_dragHandle = None;
    m_cropRect = QRectF();
    m_imageBounds = QRectF();

    if (m_dimOverlay) {
        m_scene->removeItem(m_dimOverlay);
        delete m_dimOverlay;
        m_dimOverlay = nullptr;
    }
    if (m_cropBorder) {
        m_scene->removeItem(m_cropBorder);
        delete m_cropBorder;
        m_cropBorder = nullptr;
    }

    if (m_view && m_view->viewport()) {
        m_view->viewport()->setCursor(Qt::ArrowCursor);
    }
}

CropTool::Handle CropTool::hitTest(QPointF pt) const
{
    if (m_cropRect.isEmpty()) return None;

    const qreal hs = kHandleSize + 4.0; // hit slop
    auto nearPt = [hs](QPointF a, QPointF b) {
        return (std::abs(a.x() - b.x()) <= hs) && (std::abs(a.y() - b.y()) <= hs);
    };

    QPointF tl = m_cropRect.topLeft();
    QPointF tr = m_cropRect.topRight();
    QPointF bl = m_cropRect.bottomLeft();
    QPointF br = m_cropRect.bottomRight();
    QPointF tc = QPointF(m_cropRect.center().x(), m_cropRect.top());
    QPointF bc = QPointF(m_cropRect.center().x(), m_cropRect.bottom());
    QPointF lc = QPointF(m_cropRect.left(), m_cropRect.center().y());
    QPointF rc = QPointF(m_cropRect.right(), m_cropRect.center().y());

    if (nearPt(pt, tl)) return TopLeft;
    if (nearPt(pt, tr)) return TopRight;
    if (nearPt(pt, bl)) return BottomLeft;
    if (nearPt(pt, br)) return BottomRight;

    if (nearPt(pt, tc)) return Top;
    if (nearPt(pt, bc)) return Bottom;
    if (nearPt(pt, lc)) return Left;
    if (nearPt(pt, rc)) return Right;

    if (m_cropRect.contains(pt)) return Move;

    return None;
}

void CropTool::updateCursor(Handle h)
{
    if (!m_view || !m_view->viewport()) return;

    switch (h) {
    case TopLeft:
    case BottomRight:
        m_view->viewport()->setCursor(Qt::SizeFDiagCursor);
        break;
    case TopRight:
    case BottomLeft:
        m_view->viewport()->setCursor(Qt::SizeBDiagCursor);
        break;
    case Top:
    case Bottom:
        m_view->viewport()->setCursor(Qt::SizeVerCursor);
        break;
    case Left:
    case Right:
        m_view->viewport()->setCursor(Qt::SizeHorCursor);
        break;
    case Move:
        m_view->viewport()->setCursor(Qt::SizeAllCursor);
        break;
    default:
        m_view->viewport()->setCursor(Qt::ArrowCursor);
        break;
    }
}

bool CropTool::handleMousePress(QPointF scenePos)
{
    if (!m_active || m_cropRect.isEmpty()) return false;

    Handle h = hitTest(scenePos);
    if (h != None) {
        m_dragHandle = h;
        m_lastMousePos = scenePos;
        updateCursor(h);
        return true;
    }
    return false;
}

bool CropTool::handleMouseMove(QPointF scenePos)
{
    if (!m_active || m_cropRect.isEmpty()) return false;

    if (m_dragHandle == None) {
        Handle h = hitTest(scenePos);
        updateCursor(h);
        return false;
    }

    QPointF delta = scenePos - m_lastMousePos;
    m_lastMousePos = scenePos;

    QRectF newRect = m_cropRect;
    const qreal minSize = 20.0;

    switch (m_dragHandle) {
    case Move: {
        newRect.translate(delta);
        // Constrain to image bounds
        if (newRect.left() < m_imageBounds.left())
            newRect.moveLeft(m_imageBounds.left());
        if (newRect.top() < m_imageBounds.top())
            newRect.moveTop(m_imageBounds.top());
        if (newRect.right() > m_imageBounds.right())
            newRect.moveRight(m_imageBounds.right());
        if (newRect.bottom() > m_imageBounds.bottom())
            newRect.moveBottom(m_imageBounds.bottom());
        break;
    }
    case TopLeft: {
        qreal l = std::min(newRect.right() - minSize, std::max(m_imageBounds.left(), newRect.left() + delta.x()));
        qreal t = std::min(newRect.bottom() - minSize, std::max(m_imageBounds.top(), newRect.top() + delta.y()));
        newRect.setLeft(l);
        newRect.setTop(t);
        break;
    }
    case Top: {
        qreal t = std::min(newRect.bottom() - minSize, std::max(m_imageBounds.top(), newRect.top() + delta.y()));
        newRect.setTop(t);
        break;
    }
    case TopRight: {
        qreal r = std::max(newRect.left() + minSize, std::min(m_imageBounds.right(), newRect.right() + delta.x()));
        qreal t = std::min(newRect.bottom() - minSize, std::max(m_imageBounds.top(), newRect.top() + delta.y()));
        newRect.setRight(r);
        newRect.setTop(t);
        break;
    }
    case Right: {
        qreal r = std::max(newRect.left() + minSize, std::min(m_imageBounds.right(), newRect.right() + delta.x()));
        newRect.setRight(r);
        break;
    }
    case BottomRight: {
        qreal r = std::max(newRect.left() + minSize, std::min(m_imageBounds.right(), newRect.right() + delta.x()));
        qreal b = std::max(newRect.top() + minSize, std::min(m_imageBounds.bottom(), newRect.bottom() + delta.y()));
        newRect.setRight(r);
        newRect.setBottom(b);
        break;
    }
    case Bottom: {
        qreal b = std::max(newRect.top() + minSize, std::min(m_imageBounds.bottom(), newRect.bottom() + delta.y()));
        newRect.setBottom(b);
        break;
    }
    case BottomLeft: {
        qreal l = std::min(newRect.right() - minSize, std::max(m_imageBounds.left(), newRect.left() + delta.x()));
        qreal b = std::max(newRect.top() + minSize, std::min(m_imageBounds.bottom(), newRect.bottom() + delta.y()));
        newRect.setLeft(l);
        newRect.setBottom(b);
        break;
    }
    case Left: {
        qreal l = std::min(newRect.right() - minSize, std::max(m_imageBounds.left(), newRect.left() + delta.x()));
        newRect.setLeft(l);
        break;
    }
    default:
        break;
    }

    m_cropRect = newRect;
    updateVisuals();
    emit cropRectChanged(m_cropRect);
    return true;
}

bool CropTool::handleMouseRelease(QPointF /*scenePos*/)
{
    if (m_dragHandle != None) {
        m_dragHandle = None;
        updateCursor(None);
        return true;
    }
    return false;
}

void CropTool::updateVisuals()
{
    if (!m_scene || m_cropRect.isEmpty()) return;

    // 1. Dim outside region using OddEven fill
    QPainterPath dimPath;
    dimPath.setFillRule(Qt::OddEvenFill);
    dimPath.addRect(m_imageBounds);
    dimPath.addRect(m_cropRect);

    if (!m_dimOverlay) {
        m_dimOverlay = m_scene->addPath(dimPath, QPen(Qt::NoPen), QBrush(QColor(0, 0, 0, 160)));
        m_dimOverlay->setZValue(120);
    } else {
        m_dimOverlay->setPath(dimPath);
    }

    // 2. Crop border + rule of thirds + 8 handles
    QPainterPath borderPath;
    borderPath.addRect(m_cropRect);

    // Rule of thirds grid lines
    qreal wThird = m_cropRect.width() / 3.0;
    qreal hThird = m_cropRect.height() / 3.0;

    borderPath.moveTo(m_cropRect.left() + wThird, m_cropRect.top());
    borderPath.lineTo(m_cropRect.left() + wThird, m_cropRect.bottom());

    borderPath.moveTo(m_cropRect.left() + 2 * wThird, m_cropRect.top());
    borderPath.lineTo(m_cropRect.left() + 2 * wThird, m_cropRect.bottom());

    borderPath.moveTo(m_cropRect.left(), m_cropRect.top() + hThird);
    borderPath.lineTo(m_cropRect.right(), m_cropRect.top() + hThird);

    borderPath.moveTo(m_cropRect.left(), m_cropRect.top() + 2 * hThird);
    borderPath.lineTo(m_cropRect.right(), m_cropRect.top() + 2 * hThird);

    // Add handle boxes
    const qreal hs = kHandleSize;
    auto addHandleRect = [&borderPath, hs](QPointF pt) {
        borderPath.addRect(QRectF(pt.x() - hs / 2.0, pt.y() - hs / 2.0, hs, hs));
    };

    addHandleRect(m_cropRect.topLeft());
    addHandleRect(m_cropRect.topRight());
    addHandleRect(m_cropRect.bottomLeft());
    addHandleRect(m_cropRect.bottomRight());
    addHandleRect(QPointF(m_cropRect.center().x(), m_cropRect.top()));
    addHandleRect(QPointF(m_cropRect.center().x(), m_cropRect.bottom()));
    addHandleRect(QPointF(m_cropRect.left(), m_cropRect.center().y()));
    addHandleRect(QPointF(m_cropRect.right(), m_cropRect.center().y()));

    QPen pen(QColor("#38bdf8"), 1.5, Qt::SolidLine);
    pen.setCosmetic(true);

    if (!m_cropBorder) {
        m_cropBorder = m_scene->addPath(borderPath, pen, QBrush(Qt::transparent));
        m_cropBorder->setZValue(121);
    } else {
        m_cropBorder->setPath(borderPath);
        m_cropBorder->setPen(pen);
    }
}
