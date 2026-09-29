#pragma once

#include <QGraphicsItem>
#include <QPixmap>
#include <QImage>
#include <QVector>
#include <QPointF>
#include <functional>

class QGraphicsSceneMouseEvent;
class QPainter;

/**
 * @brief A movable, scalable, rotatable sticker overlaid on the filtered view.
 *
 * Lives in the QGraphicsScene of the processed-image panel.
 * Has its own independent filter stack and per-sticker undo/redo history.
 *
 * Visual selection uses GREEN (#22c55e) frame instead of purple.
 * Supports interactive corner-resize handles (aspect-ratio preserving)
 * and a rotation handle (circle above the sticker).
 */
class StickerItem : public QGraphicsItem {
public:
    explicit StickerItem(const QImage& image, QGraphicsItem* parent = nullptr);

    // QGraphicsItem
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

    // --- Filter / image state ---
    void applyFilter(const QImage& filteredImage);
    void resetFilter();
    QImage currentImage() const;
    QImage originalImage() const;

    /// Return the committed image (what filters & Apply operate on).
    QImage committedImage() const { return m_committedImage; }
    /// Set committed image directly (e.g. after Apply).
    void setCommittedImage(const QImage& img);
    /// Set a temporary preview image (slider preview). Does NOT commit.
    void setPreviewImage(const QImage& img);
    /// Discard preview, revert display to committed image.
    void clearPreview();
    bool hasPreview() const { return m_hasTempPreview; }

    // --- Selection visual ---
    void setSelectedVisual(bool selected);
    bool isSelectedVisual() const;

    // --- Source path (for asset-in-use checks) ---
    void setSourcePath(const QString& path) { m_sourcePath = path; }
    QString sourcePath() const { return m_sourcePath; }

    // --- Focus callback ---
    std::function<void(StickerItem*)> onClicked;

    // --- Per-sticker undo/redo ---
    struct Snapshot {
        QImage  image;
        QPointF position;
        qreal   scaleVal;
        qreal   rotation;
        qreal   zVal;
    };

    void pushSnapshot();
    bool canUndo() const;
    bool canRedo() const;
    void undo();
    void redo();

    Snapshot captureSnapshot() const;
    void restoreSnapshot(const Snapshot& snap);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

private:
    enum DragMode { None, Move, ResizeTL, ResizeTR, ResizeBL, ResizeBR, Rotate };
    DragMode hitTest(const QPointF& localPos) const;

    void rebuildPixmap();

    QImage   m_originalImage;
    QImage   m_committedImage; // current committed (may be filtered)
    QImage   m_currentImage;   // currently displayed (may be preview)
    QPixmap  m_pixmap;         // cached from m_currentImage
    bool     m_selected = false;
    bool     m_hasTempPreview = false;
    QString  m_sourcePath;

    // Interaction
    DragMode m_dragMode = None;
    QPointF  m_lastScenePos;
    qreal    m_scaleAtDragStart = 1.0;
    qreal    m_distAtDragStart  = 0.0;
    qreal    m_rotAtDragStart   = 0.0;
    qreal    m_angleAtDragStart = 0.0;
    bool     m_snapshotPushedForDrag = false;

    // History
    QVector<Snapshot> m_undoStack;
    QVector<Snapshot> m_redoStack;
    static constexpr int kMaxHistory = 40;
};
