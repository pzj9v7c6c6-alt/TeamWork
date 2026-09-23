#pragma once

#include <QWidget>
#include <QImage>

class QGraphicsView;
class QGraphicsScene;
class QGraphicsPixmapItem;
class QLabel;
class QPushButton;

/**
 * @brief Reusable image viewer widget matching the reference UI workspace panels.
 *
 * Used for both "Original" and "Filtered" image panels.
 * Features:
 *  - Top-left badge ("Original" / "Filtered")
 *  - Aspect-ratio preserving viewport with panning
 *  - Bottom dimension badge ("1920 × 1080")
 *  - Bottom-right quick zoom and fit buttons
 *  - Responsive resize handling
 *  - Empty state with iconography and helpful instructions
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

signals:
    void zoomChanged(double percent);
    void fitRequested();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void updateDimensionLabel();
    void updateZoomLabel();
    void showEmptyState(bool show);
    void updatePanelStyle();

    bool m_isFilteredPanel = false;
    bool m_hasActiveGlow = false;

    QGraphicsView* m_view = nullptr;
    QGraphicsScene* m_scene = nullptr;
    QGraphicsPixmapItem* m_pixmapItem = nullptr;

    QLabel* m_titleLabel = nullptr;
    QLabel* m_dimensionLabel = nullptr;
    QLabel* m_zoomLabel = nullptr;
    QPushButton* m_quickFitBtn = nullptr;
    QPushButton* m_quickZoomBtn = nullptr;

    // Empty state
    QWidget* m_emptyOverlay = nullptr;
    QLabel* m_emptyIcon = nullptr;
    QLabel* m_emptyPrimary = nullptr;
    QLabel* m_emptySecondary = nullptr;

    QImage m_currentImage;
    double m_zoomLevel = 1.0;
};
