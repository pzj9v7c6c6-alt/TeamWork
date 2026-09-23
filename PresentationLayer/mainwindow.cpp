#include "mainwindow.h"

#include "ImageEditorService.h"
#include "FilterRegistry.h"
#include "TopBar.h"
#include "ImageViewWidget.h"
#include "FiltersPanel.h"
#include "StatusBar.h"
#include "SaveImageDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTabWidget>
#include <QTabBar>
#include <QSlider>
#include <QLabel>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileDialog>
#include <QScrollArea>

// ---------------------------------------------------------------------------
// Stylesheet constants
// ---------------------------------------------------------------------------
namespace {

const char* kTabWidgetStyle = R"(
    QTabWidget::pane {
        background: #0f1524;
        border: none;
    }
    QTabBar {
        background: #0b0f1a;
        qproperty-drawBase: 0;
        margin: 10px 10px 4px 10px;
    }
    QTabBar::tab {
        background: #141c2e;
        color: #8b9bb4;
        border: 1px solid #243049;
        border-radius: 8px;
        padding: 7px 12px;
        margin-right: 6px;
        font-size: 11px;
        font-weight: 600;
        min-width: 60px;
    }
    QTabBar::tab:hover {
        background: #1e293b;
        color: #e2e8f0;
        border-color: #3b82f6;
    }
    QTabBar::tab:selected {
        background: #2563eb;
        color: #ffffff;
        border: 1px solid #3b82f6;
    }
)";

const char* kControlPanelStyle = R"(
    QWidget#controlPanel {
        background: #0f1524;
        border-left: 1px solid #1a2336;
    }
)";

const char* kSplitterStyle = R"(
    QSplitter::handle {
        background: #1a2336;
        width: 1px;
    }
)";

const char* kAdjSliderStyle = R"(
    QSlider::groove:horizontal {
        height: 4px;
        background: #1c263c;
        border-radius: 2px;
    }
    QSlider::handle:horizontal {
        background: #ffffff;
        width: 14px;
        height: 14px;
        margin: -5px 0;
        border-radius: 7px;
    }
    QSlider::handle:horizontal:hover {
        background: #cbd5e1;
    }
    QSlider::sub-page:horizontal {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                     stop:0 #3b82f6, stop:1 #818cf8);
        border-radius: 2px;
    }
)";

} // anon namespace

// ---------------------------------------------------------------------------
// MainWindow
// ---------------------------------------------------------------------------
MainWindow::MainWindow(ImageEditorService* service,
                       FilterRegistry* registry,
                       QWidget* parent)
    : QMainWindow(parent)
    , m_service(service)
    , m_registry(registry)
{
    setWindowTitle("Image Studio");
    setMinimumSize(1100, 720);
    resize(1280, 820);
    setAcceptDrops(true);

    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto* rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // --- Top Bar ---
    m_topBar = new TopBar(this);
    rootLayout->addWidget(m_topBar);

    // --- Main content: splitter with workspace + control panel ---
    auto* mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setStyleSheet(kSplitterStyle);
    mainSplitter->setHandleWidth(2);

    // Workspace container
    auto* workspaceWidget = new QWidget(mainSplitter);
    workspaceWidget->setStyleSheet("background: #090d16;");
    auto* workspaceLayout = new QHBoxLayout(workspaceWidget);
    workspaceLayout->setContentsMargins(12, 12, 8, 12);
    workspaceLayout->setSpacing(12);

    m_originalView = new ImageViewWidget("\xF0\x9F\x96\xBC  Original", false, workspaceWidget);
    m_originalView->setEmptyStateText("No image loaded", "Load an image to get started");

    m_processedView = new ImageViewWidget("\xE2\x9C\xA8  Filtered", true, workspaceWidget);
    m_processedView->setEmptyStateText("No filtered image", "Apply a filter to preview the result");

    workspaceLayout->addWidget(m_originalView, 1);
    workspaceLayout->addWidget(m_processedView, 1);

    // Control panel (right side)
    auto* controlPanel = createControlPanel();

    mainSplitter->addWidget(workspaceWidget);
    mainSplitter->addWidget(controlPanel);
    mainSplitter->setStretchFactor(0, 3);
    mainSplitter->setStretchFactor(1, 1);
    mainSplitter->setSizes({860, 380});

    rootLayout->addWidget(mainSplitter, 1);

    // --- Status Bar ---
    m_statusBar = new StatusBar(this);
    rootLayout->addWidget(m_statusBar);

    // -----------------------------------------------------------------------
    // Connect signals
    // -----------------------------------------------------------------------
    // TopBar
    connect(m_topBar, &TopBar::loadClicked, this, &MainWindow::onLoadImage);
    connect(m_topBar, &TopBar::saveClicked, this, &MainWindow::onSaveImage);
    connect(m_topBar, &TopBar::undoClicked, m_service, &ImageEditorService::undo);
    connect(m_topBar, &TopBar::redoClicked, m_service, &ImageEditorService::redo);
    connect(m_topBar, &TopBar::clearClicked, this, &MainWindow::onClear);

    // Service
    connect(m_service, &ImageEditorService::imageLoaded, this, &MainWindow::onImageLoaded);
    connect(m_service, &ImageEditorService::imageProcessed, this, &MainWindow::onImageProcessed);
    connect(m_service, &ImageEditorService::historyChanged, this, &MainWindow::onHistoryChanged);
    connect(m_service, &ImageEditorService::errorOccurred, this, &MainWindow::onError);
    connect(m_service, &ImageEditorService::imageSaved, this, [this]() {
        m_statusBar->setImageInfo(m_service->imageWidth(), m_service->imageHeight(),
                                  m_service->imageFormat() + " (saved)");
    });

    // StatusBar zoom controls
    connect(m_statusBar, &StatusBar::zoomInClicked, this, [this]() {
        m_originalView->zoomIn();
        m_processedView->zoomIn();
    });
    connect(m_statusBar, &StatusBar::zoomOutClicked, this, [this]() {
        m_originalView->zoomOut();
        m_processedView->zoomOut();
    });
    connect(m_statusBar, &StatusBar::fitToViewClicked, this, [this]() {
        m_originalView->fitToView();
        m_processedView->fitToView();
    });
    connect(m_statusBar, &StatusBar::zoomValueChanged, this, [this](double pct) {
        m_originalView->setZoom(pct);
        m_processedView->setZoom(pct);
    });

    // Sync zoom display from original view
    connect(m_originalView, &ImageViewWidget::zoomChanged, m_statusBar, &StatusBar::setZoomPercent);
}

MainWindow::~MainWindow() = default;

// ---------------------------------------------------------------------------
// Control Panel construction
// ---------------------------------------------------------------------------
QWidget* MainWindow::createControlPanel()
{
    auto* panel = new QWidget(this);
    panel->setObjectName("controlPanel");
    panel->setStyleSheet(kControlPanelStyle);
    panel->setMinimumWidth(320);
    panel->setMaximumWidth(440);

    auto* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_tabWidget = new QTabWidget(panel);
    m_tabWidget->setStyleSheet(kTabWidgetStyle);
    m_tabWidget->setDocumentMode(true);

    // 1. Adjustments tab (Sliders + Quick Filters)
    auto* adjustmentsWidget = createAdjustmentsTab();
    m_tabWidget->addTab(adjustmentsWidget, "\xE2\x9A\x9B  Adjustments"); // ⚛/Sliders

    // 2. Filters tab (dynamic)
    m_filtersTabPanel = new FiltersPanel(m_registry, "Filters", m_tabWidget);
    connect(m_filtersTabPanel, &FiltersPanel::filterSelected, this, &MainWindow::onFilterSelected);
    m_tabWidget->addTab(m_filtersTabPanel, "\xE2\xA7\x96  Filters"); // ⧖

    // 3. Effects tab (dynamic)
    m_effectsTabPanel = new FiltersPanel(m_registry, "Effects", m_tabWidget);
    connect(m_effectsTabPanel, &FiltersPanel::filterSelected, this, &MainWindow::onFilterSelected);
    m_tabWidget->addTab(m_effectsTabPanel, "\xE2\x9C\xA8  Effects"); // ✨

    // 4. Tools tab (dynamic)
    m_toolsTabPanel = new FiltersPanel(m_registry, "Tools", m_tabWidget);
    connect(m_toolsTabPanel, &FiltersPanel::filterSelected, this, &MainWindow::onFilterSelected);
    m_tabWidget->addTab(m_toolsTabPanel, "\xF0\x9F\x94\xA7  Tools"); // 🔧

    // Select Adjustments tab by default (as in reference image!)
    m_tabWidget->setCurrentIndex(0);

    layout->addWidget(m_tabWidget);

    return panel;
}

QWidget* MainWindow::createAdjustmentsTab()
{
    auto* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(R"(
        QScrollArea {
            background: transparent;
            border: none;
        }
        QScrollBar:vertical {
            background: #0b0f19; width: 6px; border-radius: 3px;
        }
        QScrollBar::handle:vertical {
            background: #243049; border-radius: 3px; min-height: 30px;
        }
    )");

    auto* container = new QWidget();
    container->setStyleSheet("background: transparent;");
    auto* layout = new QVBoxLayout(container);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(14);

    // --- Sliders section ---
    auto createSliderRow = [this, container](const QString& icon, const QString& name,
                                             int minVal, int maxVal, int defaultVal,
                                             const QString& suffix,
                                             QSlider*& sliderOut, QLabel*& valLabelOut) {
        auto* row = new QVBoxLayout();
        row->setSpacing(4);

        auto* headerRow = new QHBoxLayout();
        auto* iconLabel = new QLabel(icon, container);
        iconLabel->setStyleSheet("font-size: 14px; color: #a5b4fc; background: transparent;");

        auto* nameLabel = new QLabel(name, container);
        nameLabel->setStyleSheet("font-size: 12px; font-weight: 500; color: #cbd5e1; background: transparent;");

        valLabelOut = new QLabel(QString("%1%2").arg(defaultVal > 0 ? "+" : "").arg(defaultVal).arg(suffix), container);
        valLabelOut->setStyleSheet("font-size: 11px; color: #8b9bb4; background: transparent;");

        headerRow->addWidget(iconLabel);
        headerRow->addSpacing(4);
        headerRow->addWidget(nameLabel);
        headerRow->addStretch(1);
        headerRow->addWidget(valLabelOut);

        sliderOut = new QSlider(Qt::Horizontal, container);
        sliderOut->setRange(minVal, maxVal);
        sliderOut->setValue(defaultVal);
        sliderOut->setStyleSheet(kAdjSliderStyle);

        row->addLayout(headerRow);
        row->addWidget(sliderOut);

        return row;
    };

    // Brightness
    auto* brightLayout = createSliderRow("\xE2\x98\x80", "Brightness", -100, 100, 0, "%",
                                         m_brightnessSlider, m_brightnessValLabel);
    layout->addLayout(brightLayout);
    connect(m_brightnessSlider, &QSlider::valueChanged, this, [this](int val) {
        m_brightnessValLabel->setText(QString("%1%2%").arg(val > 0 ? "+" : "").arg(val));
    });
    connect(m_brightnessSlider, &QSlider::sliderReleased, this, [this]() {
        onBrightnessChanged(m_brightnessSlider->value());
    });

    // Contrast
    auto* contrastLayout = createSliderRow("\xE2\x97\x90", "Contrast", -100, 100, 0, "%",
                                           m_contrastSlider, m_contrastValLabel);
    layout->addLayout(contrastLayout);
    connect(m_contrastSlider, &QSlider::valueChanged, this, [this](int val) {
        m_contrastValLabel->setText(QString("%1%2%").arg(val > 0 ? "+" : "").arg(val));
    });

    // Saturation
    auto* satLayout = createSliderRow("\xF0\x9F\x8E\xA8", "Saturation", -100, 100, 0, "%",
                                      m_saturationSlider, m_saturationValLabel);
    layout->addLayout(satLayout);
    connect(m_saturationSlider, &QSlider::valueChanged, this, [this](int val) {
        m_saturationValLabel->setText(QString("%1%2%").arg(val > 0 ? "+" : "").arg(val));
    });

    // Hue
    auto* hueLayout = createSliderRow("\xE2\x8A\x9B", "Hue", -180, 180, 0, "\xC2\xB0",
                                      m_hueSlider, m_hueValLabel);
    layout->addLayout(hueLayout);
    connect(m_hueSlider, &QSlider::valueChanged, this, [this](int val) {
        m_hueValLabel->setText(QString("%1\xC2\xB0").arg(val));
    });

    layout->addSpacing(8);

    // --- Quick Filters Section (embedded right inside Adjustments tab) ---
    m_quickFiltersPanel = new FiltersPanel(m_registry, "Filters", container);
    connect(m_quickFiltersPanel, &FiltersPanel::filterSelected, this, &MainWindow::onFilterSelected);
    layout->addWidget(m_quickFiltersPanel);

    scroll->setWidget(container);
    return scroll;
}

// ---------------------------------------------------------------------------
// Drag and Drop
// ---------------------------------------------------------------------------
void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        const auto urls = event->mimeData()->urls();
        for (const auto& url : urls) {
            QString path = url.toLocalFile().toLower();
            if (path.endsWith(".png") || path.endsWith(".jpg") ||
                path.endsWith(".jpeg") || path.endsWith(".bmp") ||
                path.endsWith(".tga")) {
                event->acceptProposedAction();
                return;
            }
        }
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    const auto urls = event->mimeData()->urls();
    if (!urls.isEmpty()) {
        QString filePath = urls.first().toLocalFile();
        m_service->loadImage(filePath);
    }
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    if (m_service->hasImage()) {
        m_originalView->fitToView();
        if (m_service->hasProcessedImage())
            m_processedView->fitToView();
    }
}

// ---------------------------------------------------------------------------
// Slots
// ---------------------------------------------------------------------------
void MainWindow::onLoadImage()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "Load Image", {},
        "Images (*.png *.jpg *.jpeg *.bmp *.tga);;All Files (*)");

    if (!filePath.isEmpty()) {
        m_service->loadImage(filePath);
    }
}

void MainWindow::onSaveImage()
{
    if (!m_service->hasImage()) return;

    SaveImageDialog dlg(m_service->loadedFilePath(), m_service->imageFormat(), this);
    if (dlg.exec() == QDialog::Accepted) {
        QString savePath = dlg.selectedFilePath();
        if (!savePath.isEmpty()) {
            m_service->saveImage(savePath);
        }
    }
}

void MainWindow::onClear()
{
    m_service->clear();
    m_processedView->clearImage();
    m_processedView->setActiveGlow(false);

    if (m_quickFiltersPanel) m_quickFiltersPanel->clearSelection();
    if (m_filtersTabPanel) m_filtersTabPanel->clearSelection();
    if (m_effectsTabPanel) m_effectsTabPanel->clearSelection();
    if (m_toolsTabPanel) m_toolsTabPanel->clearSelection();

    if (m_brightnessSlider) {
        m_brightnessSlider->blockSignals(true);
        m_brightnessSlider->setValue(0);
        m_brightnessSlider->blockSignals(false);
        m_brightnessValLabel->setText("0%");
    }
}

void MainWindow::onFilterSelected(int index)
{
    if (index < 0) {
        // "Original" selected
        m_service->clear();
        m_processedView->clearImage();
        m_processedView->setActiveGlow(false);
    } else {
        m_service->applyFilter(index);
    }

    // Sync selection across tabs
    if (m_quickFiltersPanel) m_quickFiltersPanel->selectFilter(index);
    if (m_filtersTabPanel) m_filtersTabPanel->selectFilter(index);
    if (m_effectsTabPanel) m_effectsTabPanel->selectFilter(index);
    if (m_toolsTabPanel) m_toolsTabPanel->selectFilter(index);
}

void MainWindow::onBrightnessChanged(int value)
{
    if (!m_service->hasImage()) return;
    m_service->applyBrightness(value);
}

void MainWindow::onImageLoaded()
{
    QImage orig = m_service->originalQImage();
    m_originalView->setImage(orig);
    m_processedView->clearImage();
    m_processedView->setActiveGlow(false);

    // Update thumbnails on all filter panels!
    if (m_quickFiltersPanel) m_quickFiltersPanel->updateThumbnails(m_service);
    if (m_filtersTabPanel) m_filtersTabPanel->updateThumbnails(m_service);
    if (m_effectsTabPanel) m_effectsTabPanel->updateThumbnails(m_service);
    if (m_toolsTabPanel) m_toolsTabPanel->updateThumbnails(m_service);

    m_statusBar->setImageInfo(m_service->imageWidth(), m_service->imageHeight(),
                              m_service->imageFormat());
    m_topBar->setSaveEnabled(true);
    m_topBar->setClearEnabled(true);

    if (m_brightnessSlider) {
        m_brightnessSlider->setValue(0);
        m_brightnessValLabel->setText("0%");
    }
}

void MainWindow::onImageProcessed()
{
    QImage processed = m_service->processedQImage();
    if (processed.isNull()) {
        m_processedView->clearImage();
        m_processedView->setActiveGlow(false);
    } else {
        m_processedView->setImage(processed);
        m_processedView->setActiveGlow(true);
    }
}

void MainWindow::onHistoryChanged()
{
    m_topBar->setUndoEnabled(m_service->canUndo());
    m_topBar->setRedoEnabled(m_service->canRedo());
    m_topBar->setClearEnabled(m_service->hasImage());
    m_topBar->setSaveEnabled(m_service->hasImage());
}

void MainWindow::onError(const QString& message)
{
    QMessageBox::warning(this, "Image Studio", message);
}
