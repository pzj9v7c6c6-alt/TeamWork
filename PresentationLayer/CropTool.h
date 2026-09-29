#pragma once

#include <QObject>
#include <QRectF>
#include <QPointF>
#include <QColor>

class QGraphicsView;
class QGraphicsScene;
class QGraphicsPathItem;
class QGraphicsRectItem;
class QMouseEvent;

class CropTool : public QObject {
    Q_OBJECT

public:
    enum Handle {
        None,
        Move,
        TopLeft,
        Top,
        TopRight,
        Right,
        BottomRight,
        Bottom,
        BottomLeft,
        Left
    };

    explicit CropTool(QGraphicsView* view, QGraphicsScene* scene, QObject* parent = nullptr);
    ~CropTool() override;

    bool isActive() const;
    void setActive(bool active);

    void initCrop(const QRectF& imageBounds);
    QRectF cropRect() const;
    void reset();

    bool handleMousePress(QPointF scenePos);
    bool handleMouseMove(QPointF scenePos);
    bool handleMouseRelease(QPointF scenePos);

signals:
    void cropRectChanged(const QRectF& rect);
    void cropApplied();
    void cropCancelled();

private:
    void updateVisuals();
    Handle hitTest(QPointF pt) const;
    void updateCursor(Handle h);

    QGraphicsView*     m_view        = nullptr;
    QGraphicsScene*    m_scene       = nullptr;
    bool               m_active      = false;

    QRectF             m_imageBounds;
    QRectF             m_cropRect;

    Handle             m_dragHandle  = None;
    QPointF            m_lastMousePos;

    QGraphicsPathItem* m_dimOverlay  = nullptr;
    QGraphicsPathItem* m_cropBorder  = nullptr;
    static constexpr qreal kHandleSize = 8.0;
};
