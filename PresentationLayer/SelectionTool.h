#pragma once

#include <QObject>
#include <QRectF>
#include <QImage>
#include <QGraphicsItem>
#include <QPainter>

class QGraphicsView;
class QGraphicsScene;
class QGraphicsRectItem;
class QGraphicsEllipseItem;
class QGraphicsPixmapItem;

/**
 * @brief Manages interactive region selection (rect, ellipse, or magic wand) inside a QGraphicsView.
 *
 * Draws a dashed marching-ants style selection or mask overlay over the image.
 * Provides compositeMasked() which applies an already-filtered QImage to only
 * the selected region of a base QImage — entirely in the Presentation Layer,
 * without touching IImageFilter.
 */
class SelectionTool : public QObject {
    Q_OBJECT

public:
    enum Shape { Rectangle, Ellipse, MagicWand };
    enum Mode  { InsideSelection, OutsideSelection };

    explicit SelectionTool(QGraphicsView* view, QGraphicsScene* scene,
                           QObject* parent = nullptr);
    ~SelectionTool() override;

    void setShape(Shape shape);
    Shape shape() const;

    void setTolerance(int tolerance);
    int tolerance() const;

    bool hasSelection() const;
    QRectF selectionRect() const; ///< In scene coordinates

    void clearSelection();
    void setSourceImage(const QImage& img);

    /**
     * @brief Composite filteredImage into baseImage restricted to the current selection.
     *
     * @param baseImage     The unfiltered (or current) image in scene space.
     * @param filteredImage The same image with a filter already applied.
     * @param mode          Whether to take filtered from inside or outside the selection.
     * @return              A new composited image.
     */
    QImage compositeMasked(const QImage& baseImage,
                           const QImage& filteredImage,
                           Mode mode) const;

    // Called by the processed image view's event filter
    bool handleMousePress(QPointF scenePos);
    bool handleMouseMove(QPointF scenePos);
    bool handleMouseRelease(QPointF scenePos);

    bool isActive() const;
    void setActive(bool active);

signals:
    void selectionChanged();

private:
    void updateOverlay();
    void runMagicWand(const QPoint& seed);

    QGraphicsView*          m_view    = nullptr;
    QGraphicsScene*         m_scene   = nullptr;
    Shape                   m_shape   = Rectangle;
    bool                    m_active  = false;
    bool                    m_drawing = false;
    int                     m_tolerance = 42;

    bool                    m_hasMagicSeed = false;
    QPoint                  m_lastMagicSeed;

    QPointF                 m_startPos;
    QRectF                  m_selectionRect;

    QImage                  m_sourceImage;
    QImage                  m_magicMask;

    QGraphicsRectItem*      m_rectOverlay    = nullptr;
    QGraphicsEllipseItem*   m_ellipseOverlay = nullptr;
    QGraphicsPixmapItem*    m_magicOverlay   = nullptr;
};
