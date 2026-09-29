#include "StickerItem.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QStyleOptionGraphicsItem>
#include <QCursor>
#include <QGraphicsScene>
#include <QtMath>

// Selection / handle colours — GREEN, not purple
static const QColor kFocusColor("#22c55e");
static const QColor kFocusFill(34, 197, 94, 30);
static constexpr qreal kHandleSize = 10.0;
static constexpr qreal kRotHandleDist = 24.0;
static constexpr qreal kMinScale = 0.05;
static constexpr qreal kMaxScale = 10.0;

// ---------------------------------------------------------------------------
StickerItem::StickerItem(const QImage& image, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , m_originalImage(image)
    , m_committedImage(image)
    , m_currentImage(image)
    , m_pixmap(QPixmap::fromImage(image))
{
    setFlags(QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    setCursor(Qt::SizeAllCursor);
    setTransformOriginPoint(boundingRect().center());
    // Disable item caching to prevent ghost trails
    setCacheMode(QGraphicsItem::NoCache);
}

// ---------------------------------------------------------------------------
QRectF StickerItem::boundingRect() const
{
    // Expand bounding rect to include handles + rotation handle
    qreal pad = kHandleSize + 2.0;
    QRectF base(0, 0, m_pixmap.width(), m_pixmap.height());
    return base.adjusted(-pad, -pad - kRotHandleDist, pad, pad);
}

QPainterPath StickerItem::shape() const
{
    QPainterPath p;
    // The clickable shape is the pixmap rect + handles area
    qreal pad = kHandleSize + 2.0;
    QRectF base(0, 0, m_pixmap.width(), m_pixmap.height());
    p.addRect(base.adjusted(-pad, -pad - kRotHandleDist, pad, pad));
    return p;
}

// ---------------------------------------------------------------------------
void StickerItem::paint(QPainter* painter,
                        const QStyleOptionGraphicsItem* /*option*/,
                        QWidget* /*widget*/)
{
    painter->drawPixmap(0, 0, m_pixmap);

    if (m_selected) {
        // Green selection border
        QPen borderPen(kFocusColor, 2, Qt::SolidLine);
        painter->setPen(borderPen);
        painter->setBrush(Qt::NoBrush);
        QRectF br(0, 0, m_pixmap.width(), m_pixmap.height());
        painter->drawRect(br);

        // Corner resize handles
        const qreal hs = kHandleSize;
        painter->setBrush(kFocusColor);
        painter->setPen(Qt::NoPen);

        auto drawHandle = [&](qreal cx, qreal cy) {
            painter->drawRect(QRectF(cx - hs/2, cy - hs/2, hs, hs));
        };
        drawHandle(br.left(),  br.top());
        drawHandle(br.right(), br.top());
        drawHandle(br.left(),  br.bottom());
        drawHandle(br.right(), br.bottom());

        // Rotation handle — circle above top-center
        qreal mx = br.center().x();
        qreal rotY = br.top() - kRotHandleDist;
        painter->drawEllipse(QPointF(mx, rotY), 6, 6);
        // Stem line
        painter->setPen(QPen(kFocusColor, 1.5));
        painter->drawLine(QPointF(mx, rotY + 6), QPointF(mx, br.top()));
    }
}

// ---------------------------------------------------------------------------
// Filter / image state
// ---------------------------------------------------------------------------
void StickerItem::applyFilter(const QImage& filteredImage)
{
    m_committedImage = filteredImage;
    m_currentImage = filteredImage;
    m_hasTempPreview = false;
    rebuildPixmap();
}

void StickerItem::resetFilter()
{
    m_committedImage = m_originalImage;
    m_currentImage = m_originalImage;
    m_hasTempPreview = false;
    rebuildPixmap();
}

QImage StickerItem::currentImage() const { return m_currentImage; }
QImage StickerItem::originalImage() const { return m_originalImage; }

void StickerItem::setCommittedImage(const QImage& img)
{
    m_committedImage = img;
    m_currentImage = img;
    m_hasTempPreview = false;
    rebuildPixmap();
}

void StickerItem::setPreviewImage(const QImage& img)
{
    m_hasTempPreview = true;
    m_currentImage = img;
    rebuildPixmap();
}

void StickerItem::clearPreview()
{
    if (!m_hasTempPreview) return;
    m_hasTempPreview = false;
    m_currentImage = m_committedImage;
    rebuildPixmap();
}

void StickerItem::rebuildPixmap()
{
    prepareGeometryChange();
    m_pixmap = QPixmap::fromImage(m_currentImage);
    setTransformOriginPoint(QPointF(m_pixmap.width() / 2.0, m_pixmap.height() / 2.0));
    update();
}

// ---------------------------------------------------------------------------
// Selection visual
// ---------------------------------------------------------------------------
void StickerItem::setSelectedVisual(bool selected)
{
    if (m_selected == selected) return;
    m_selected = selected;
    prepareGeometryChange();
    update();
}

bool StickerItem::isSelectedVisual() const { return m_selected; }

// ---------------------------------------------------------------------------
// Per-sticker undo/redo
// ---------------------------------------------------------------------------
StickerItem::Snapshot StickerItem::captureSnapshot() const
{
    return { m_committedImage, pos(), scale(), rotation(), zValue() };
}

void StickerItem::restoreSnapshot(const Snapshot& snap)
{
    m_committedImage = snap.image;
    m_currentImage = snap.image;
    m_hasTempPreview = false;
    rebuildPixmap();
    setPos(snap.position);
    setScale(snap.scaleVal);
    setRotation(snap.rotation);
    setZValue(snap.zVal);
}

void StickerItem::pushSnapshot()
{
    if (static_cast<int>(m_undoStack.size()) >= kMaxHistory)
        m_undoStack.removeFirst();
    m_undoStack.push_back(captureSnapshot());
    m_redoStack.clear();
}

bool StickerItem::canUndo() const { return !m_undoStack.isEmpty(); }
bool StickerItem::canRedo() const { return !m_redoStack.isEmpty(); }

void StickerItem::undo()
{
    if (m_undoStack.isEmpty()) return;
    m_redoStack.push_back(captureSnapshot());
    Snapshot prev = m_undoStack.last();
    m_undoStack.removeLast();
    restoreSnapshot(prev);
}

void StickerItem::redo()
{
    if (m_redoStack.isEmpty()) return;
    m_undoStack.push_back(captureSnapshot());
    Snapshot next = m_redoStack.last();
    m_redoStack.removeLast();
    restoreSnapshot(next);
}

// ---------------------------------------------------------------------------
// Hit-test for drag mode
// ---------------------------------------------------------------------------
StickerItem::DragMode StickerItem::hitTest(const QPointF& localPos) const
{
    if (!m_selected) return Move;

    QRectF br(0, 0, m_pixmap.width(), m_pixmap.height());
    const qreal hs = kHandleSize + 4.0; // enlarged hit area

    // Rotation handle
    qreal mx = br.center().x();
    qreal rotY = br.top() - kRotHandleDist;
    if (QLineF(localPos, QPointF(mx, rotY)).length() < hs)
        return Rotate;

    // Corner handles
    if (QRectF(br.left()  - hs/2, br.top()    - hs/2, hs, hs).contains(localPos)) return ResizeTL;
    if (QRectF(br.right() - hs/2, br.top()    - hs/2, hs, hs).contains(localPos)) return ResizeTR;
    if (QRectF(br.left()  - hs/2, br.bottom() - hs/2, hs, hs).contains(localPos)) return ResizeBL;
    if (QRectF(br.right() - hs/2, br.bottom() - hs/2, hs, hs).contains(localPos)) return ResizeBR;

    return Move;
}

// ---------------------------------------------------------------------------
// Mouse events
// ---------------------------------------------------------------------------
void StickerItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (onClicked) onClicked(this);

    m_dragMode = hitTest(event->pos());
    m_lastScenePos = event->scenePos();
    m_snapshotPushedForDrag = false;

    if (m_dragMode == Rotate) {
        QRectF br(0, 0, m_pixmap.width(), m_pixmap.height());
        QPointF center = mapToScene(br.center());
        m_rotAtDragStart = rotation();
        m_angleAtDragStart = qAtan2(event->scenePos().y() - center.y(),
                                    event->scenePos().x() - center.x());
        event->accept();
        return;
    }

    if (m_dragMode >= ResizeTL && m_dragMode <= ResizeBR) {
        QRectF br(0, 0, m_pixmap.width(), m_pixmap.height());
        QPointF center = mapToScene(br.center());
        m_scaleAtDragStart = scale();
        m_distAtDragStart = QLineF(center, event->scenePos()).length();
        event->accept();
        return;
    }

    // Move mode — let base class handle
    QGraphicsItem::mousePressEvent(event);
}

void StickerItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Push undo snapshot once per drag
    if (!m_snapshotPushedForDrag) {
        pushSnapshot();
        m_snapshotPushedForDrag = true;
    }

    if (m_dragMode == Rotate) {
        QRectF br(0, 0, m_pixmap.width(), m_pixmap.height());
        QPointF center = mapToScene(br.center());
        qreal angle = qAtan2(event->scenePos().y() - center.y(),
                              event->scenePos().x() - center.x());
        qreal delta = qRadiansToDegrees(angle - m_angleAtDragStart);
        setRotation(m_rotAtDragStart + delta);
        event->accept();
        return;
    }

    if (m_dragMode >= ResizeTL && m_dragMode <= ResizeBR) {
        QRectF br(0, 0, m_pixmap.width(), m_pixmap.height());
        QPointF center = mapToScene(br.center());
        qreal dist = QLineF(center, event->scenePos()).length();
        if (m_distAtDragStart > 1.0) {
            qreal newScale = m_scaleAtDragStart * (dist / m_distAtDragStart);
            newScale = qBound(kMinScale, newScale, kMaxScale);
            setScale(newScale);
        }
        event->accept();
        return;
    }

    // Move — use base class
    m_lastScenePos = event->scenePos();
    QGraphicsItem::mouseMoveEvent(event);
}

void StickerItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    m_dragMode = None;
    m_snapshotPushedForDrag = false;
    QGraphicsItem::mouseReleaseEvent(event);
}

void StickerItem::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    pushSnapshot();
    // Ctrl+wheel → rotate; plain wheel → scale
    if (event->modifiers() & Qt::ControlModifier) {
        qreal angle = (event->delta() > 0) ? 5.0 : -5.0;
        setRotation(rotation() + angle);
    } else {
        qreal factor = (event->delta() > 0) ? 1.1 : (1.0 / 1.1);
        qreal sx = scale() * factor;
        sx = qBound(kMinScale, sx, kMaxScale);
        setScale(sx);
    }
    event->accept();
}
