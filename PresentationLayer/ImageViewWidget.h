#pragma once

#include <QWidget>
#include <QImage>

class QGraphicsView;
class QGraphicsScene;
class QGraphicsPixmapItem;
class QLabel;
class QPushButton;
class SelectionTool;
class CropTool;

/**
 * @brief Reusable image viewer widget for the workspace panels.
 *
 * Used for both "Original" and "Filtered" image panels.
 * Features:
 *  - Top-left badge ("Original" / "Filtered")
 *  - Aspect-ratio preserving viewport with panning (ScrollHandDrag)
 *  - Mouse-wheel zoom centred on cursor
 *  - Non-destructive view rotation (Ctrl+drag on the viewport)
 *  - Bottom dimension badge
 *  - Bottom-right quick zoom and fit buttons
 *  - Responsive resize handling
 *  - Empty state with iconography and helpful instructions
 *  - SelectionTool hookup (receives mouse events when tool is active)
 *  - CropTool hookup (receives mouse events when crop tool is active)
 *  - Drag & drop support for image files
 *  - Click-to-focus signalling for the focus system
 */
class ImageViewWidget : public QWidget {
    Q_OBJECT

public:
    explicit ImageViewWidget(const QString& title, bool isFilteredPanel = false, QWidget* parent = nullptr);

    void setImage(const QImage& image);
    void clearImage();
    void fitToView();

    double zoomPercent() const;
    void setZoom(double percent);
    void zoomIn();
    void zoomOut();

    void setEmptyStateText(const QString& primary, const QString& secondary = {});
    void setActiveGlow(bool active);

    /// Check whether the widget currently has an image displayed
    bool hasImage() const { return !m_currentImage.isNull() && m_pixmapItem != nullptr; }

    // Expose internals for MainWindow use
    QGraphicsView*  graphicsView() const { return m_view; }
    QGraphicsScene* scene()        const { return m_scene; }

    // Rotate view (non-destructive viewport rotation)
    void setRotateViewEnabled(bool enabled);
    bool rotateViewEnabled() const;

    // Selection tool and Crop tool hookups
    void setSelectionTool(SelectionTool* tool);
    void setCropTool(CropTool* tool);

signals:
    void zoomChanged(double percent);
    /// Emitted when the base image area is clicked (no sticker hit)
    void baseImageClicked();
    /// Emitted when a supported image file is dropped on this view
    void fileDropped(const QString& filePath);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void updateDimensionLabel();
    void updateZoomLabel();
    void showEmptyState(bool show);

    bool m_isFilteredPanel   = false;
    bool m_hasActiveGlow     = false;
    bool m_rotateViewEnabled = false;

    QGraphicsView*         m_view       = nullptr;
    QGraphicsScene*        m_scene      = nullptr;
    QGraphicsPixmapItem*   m_pixmapItem = nullptr;

    QLabel*      m_titleLabel     = nullptr;
    QLabel*      m_dimensionLabel = nullptr;
    QLabel*      m_zoomLabel      = nullptr;
    QPushButton* m_quickZoomBtn   = nullptr;

    // Empty state
    QWidget* m_emptyOverlay   = nullptr;
    QLabel*  m_emptyIcon      = nullptr;
    QLabel*  m_emptyPrimary   = nullptr;
    QLabel*  m_emptySecondary = nullptr;

    QImage m_currentImage;
    double m_zoomLevel = 1.0;

    // Rotate view state
    bool    m_rotDragging    = false;
    QPoint  m_rotLastPos;
    qreal   m_viewRotation   = 0.0;

    // Selection tool (optional, owned by MainWindow)
    SelectionTool* m_selectionTool = nullptr;

    // Crop tool (optional, owned by MainWindow)
    CropTool*      m_cropTool      = nullptr;
};
