#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

class ImageEditorService;
class FilterRegistry;
class TopBar;
class ImageViewWidget;
class FiltersPanel;
class StatusBar;
class QTabWidget;
class QSlider;
class QLabel;

/**
 * @brief Root application window that composes all UI components.
 *
 * Implements the dark modern desktop image editor interface
 * resembling the reference design.
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
    void dropEvent(QDropEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onLoadImage();
    void onSaveImage();
    void onClear();
    void onFilterSelected(int index);
    void onBrightnessChanged(int value);
    void onImageLoaded();
    void onImageProcessed();
    void onHistoryChanged();
    void onError(const QString& message);

private:
    QWidget* createControlPanel();
    QWidget* createAdjustmentsTab();

    ImageEditorService* m_service = nullptr;
    FilterRegistry* m_registry = nullptr;

    TopBar* m_topBar = nullptr;
    ImageViewWidget* m_originalView = nullptr;
    ImageViewWidget* m_processedView = nullptr;
    StatusBar* m_statusBar = nullptr;
    QTabWidget* m_tabWidget = nullptr;

    // Filter panels
    FiltersPanel* m_quickFiltersPanel = nullptr;
    FiltersPanel* m_filtersTabPanel = nullptr;
    FiltersPanel* m_effectsTabPanel = nullptr;
    FiltersPanel* m_toolsTabPanel = nullptr;

    // Adjustment sliders
    QSlider* m_brightnessSlider = nullptr;
    QLabel* m_brightnessValLabel = nullptr;
    QSlider* m_contrastSlider = nullptr;
    QLabel* m_contrastValLabel = nullptr;
    QSlider* m_saturationSlider = nullptr;
    QLabel* m_saturationValLabel = nullptr;
    QSlider* m_hueSlider = nullptr;
    QLabel* m_hueValLabel = nullptr;
};

#endif // MAINWINDOW_H
