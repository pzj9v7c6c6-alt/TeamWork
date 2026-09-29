#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QImage>

class ImageEditorService;
class FilterRegistry;
class TopBar;
class ImageViewWidget;
class FiltersPanel;
class StatusBar;
class AssetExplorer;
class StickerItem;
class SelectionTool;
class CropTool;
class EditorTarget;
class QTabWidget;
class QSlider;
class QLabel;
class QComboBox;
class QPushButton;
class QGraphicsView;
class QGraphicsScene;

/**
 * @brief Root application window — GYYM Studio.
 *
 * Architecture:
 *  - Centralized focus system via EditorTarget (BaseImage vs Sticker)
 *  - Temporary adjustment preview vs committed state with per-row Apply buttons
 *  - Context-aware Undo / Redo for Base Image and individual Stickers
 *  - Interactive Sticker resizing, center rotation, and layer ordering (Up / Down)
 *  - High-precision Region filtering (Rectangle / Ellipse / MagicWand, Inside / Outside)
 *  - Non-destructive "Original" card reset without clearing the scene or document
 *  - Raster compositing on save without editor overlays
 *  - Permanent dark appearance
 *  - Asset list deletion with active document in-use safeguards
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(ImageEditorService* service,
                        FilterRegistry* registry,
                        QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onLoadImage();
    void onSaveImage();
    void onClear();
    void onUndo();
    void onRedo();

    void onFilterSelected(int index);

    // Adjustments: preview on slider change, commit on Apply
    void onBrightnessPreview(int value);
    void onContrastPreview(int value);
    void onSaturationPreview(int value);
    void onHuePreview(int value);
    void onFreeRotatePreview(int angle);

    void onApplyBrightness();
    void onApplyContrast();
    void onApplySaturation();
    void onApplyHue();
    void onApplyFreeRotate();

    // Free crop & Resize
    void onToggleFreeCrop(bool checked);
    void onApplyFreeCrop();
    void onCancelFreeCrop();
    void onOpenResizeDialog();

    // External workspace drop
    void handleWorkspaceImageDropped(const QString& path);

    void onImageLoaded();
    void onImageProcessed();
    void onHistoryChanged();
    void onError(const QString& message);

    // Focus system
    void onFocusChanged();

    // Asset explorer
    void onImageActivated(const QString& filePath);
    void onStickerActivated(const QString& filePath);

    // Sticker management
    void onRemoveSelectedSticker();
    void onMoveUpSticker();
    void onMoveDownSticker();
    void onApplyFilterToSticker(int filterIndex);

    // Region filtering UI
    void onToggleSelectionTool(bool checked);
    void onSelectionModeChanged(int index);
    void onApplyFilterToRegion(int filterIndex);

    // Navigation
    void onToggleRotateView(bool checked);

    // Zoom panel
    void onZoomSliderChanged(int value);
    void onViewZoomChanged(double percent);

private:
    QWidget* createControlPanel();
    QWidget* createAdjustmentsTab();
    QWidget* createStickerBar();
    QWidget* createSelectionBar();

    void placeSticker(const QString& filePath);
    void clearAllStickers();
    void updateStickerLayerButtons();
    void resetAdjustmentSliders();
    void updateZoomSliderFromView(double percent);

    ImageEditorService* m_service  = nullptr;
    FilterRegistry*     m_registry = nullptr;
    EditorTarget*       m_target   = nullptr;

    TopBar*          m_topBar        = nullptr;
    ImageViewWidget* m_originalView  = nullptr;
    ImageViewWidget* m_processedView = nullptr;
    StatusBar*       m_statusBar     = nullptr;
    AssetExplorer*   m_assetExplorer = nullptr;
    QTabWidget*      m_tabWidget     = nullptr;

    // Filter panels
    FiltersPanel* m_quickFiltersPanel = nullptr;
    FiltersPanel* m_filtersTabPanel   = nullptr;
    FiltersPanel* m_effectsTabPanel   = nullptr;
    FiltersPanel* m_toolsTabPanel     = nullptr;

    // Adjustment sliders & Apply buttons
    QSlider*     m_brightnessSlider   = nullptr;
    QLabel*      m_brightnessValLabel = nullptr;
    QPushButton* m_applyBrightnessBtn = nullptr;

    QSlider*     m_contrastSlider     = nullptr;
    QLabel*      m_contrastValLabel   = nullptr;
    QPushButton* m_applyContrastBtn   = nullptr;

    QSlider*     m_saturationSlider   = nullptr;
    QLabel*      m_saturationValLabel = nullptr;
    QPushButton* m_applySaturationBtn = nullptr;

    QSlider*     m_hueSlider          = nullptr;
    QLabel*      m_hueValLabel        = nullptr;
    QPushButton* m_applyHueBtn        = nullptr;

    // Stickers
    QVector<StickerItem*> m_stickers;
    QGraphicsScene*       m_processedScene     = nullptr;
    QPushButton*          m_removeStickerBtn   = nullptr;
    QPushButton*          m_moveUpStickerBtn   = nullptr;
    QPushButton*          m_moveDownStickerBtn = nullptr;
    QLabel*               m_stickerStatusLabel = nullptr;

    // Selection tool
    SelectionTool* m_selectionTool       = nullptr;
    QPushButton*   m_selectionToggle     = nullptr;
    QComboBox*     m_selectionShapeCombo = nullptr;
    QComboBox*     m_selectionModeCombo  = nullptr;
    QSlider*       m_toleranceSlider     = nullptr;
    QLabel*        m_toleranceValLabel   = nullptr;

    // Navigation
    bool         m_rotateViewActive = false;
    QPushButton* m_rotateViewBtn    = nullptr;

    // Zoom panel (right panel, connects to active image view)
    QSlider* m_zoomSlider     = nullptr;
    QLabel*  m_zoomPctLabel   = nullptr;
    bool     m_zoomUpdating   = false;  // prevent feedback loop

    // Free rotation slider (Tools section)
    QSlider*     m_freeRotateSlider    = nullptr;
    QLabel*      m_freeRotateValLabel  = nullptr;
    QPushButton* m_applyFreeRotateBtn  = nullptr;

    // Free crop tool (Tools section)
    CropTool*    m_cropTool            = nullptr;
    QPushButton* m_freeCropToggle      = nullptr;
    QPushButton* m_applyFreeCropBtn    = nullptr;
};

#endif // MAINWINDOW_H
