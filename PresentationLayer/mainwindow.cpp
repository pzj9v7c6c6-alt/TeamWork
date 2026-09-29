#include "mainwindow.h"

#include "ImageEditorService.h"
#include "FilterRegistry.h"
#include "TopBar.h"
#include "ImageViewWidget.h"
#include "FiltersPanel.h"
#include "StatusBar.h"
#include "SaveImageDialog.h"
#include "AssetExplorer.h"
#include "StickerItem.h"
#include "SelectionTool.h"
#include "CropTool.h"
#include "EditorTarget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QSplitter>
#include <QTabWidget>
#include <QTabBar>
#include <QSlider>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileDialog>
#include <QScrollArea>
#include <QPushButton>
#include <QComboBox>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QFileInfo>
#include <QImage>
#include <QPainter>
#include <QShortcut>
#include <QLineEdit>
#include <algorithm>

// ---------------------------------------------------------------------------
// Stylesheet constants — No purple, clean modern UI
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
        background: #3b82f6;
    }
    QSlider::sub-page:horizontal {
        background: #3b82f6;
        border-radius: 2px;
    }
)";

const char* kSmallActionBtnStyle = R"(
    QPushButton {
        background: #141c2e;
        color: #cbd5e1;
        border: 1px solid #243049;
        border-radius: 6px;
        padding: 5px 10px;
        font-size: 11px;
        font-weight: 500;
    }
    QPushButton:hover {
        background: #1e293b;
        color: #f1f5f9;
        border-color: #3b82f6;
    }
    QPushButton:pressed {
        background: #2563eb;
        color: #ffffff;
    }
    QPushButton:disabled {
        background: #0d1322;
        color: #475569;
        border-color: #1a2336;
    }
)";

const char* kApplyBtnStyle = R"(
    QPushButton {
        background: #1e3a5f;
        color: #93c5fd;
        border: 1px solid #2563eb;
        border-radius: 5px;
        padding: 3px 9px;
        font-size: 11px;
        font-weight: 600;
    }
    QPushButton:hover {
        background: #2563eb;
        color: #ffffff;
    }
    QPushButton:pressed {
        background: #1d4ed8;
    }
    QPushButton:disabled {
        background: #0d1322;
        color: #475569;
        border-color: #1a2336;
    }
)";

const char* kNavToolBarStyle = R"(
    QWidget#navToolBar {
        background: #0c101c;
        border-bottom: 1px solid #1a2336;
    }
    QPushButton {
        background: #141c2e;
        color: #8b9bb4;
        border: 1px solid #243049;
        border-radius: 6px;
        padding: 5px 10px;
        font-size: 11px;
        font-weight: 500;
    }
    QPushButton:hover {
        background: #1e293b;
        color: #e2e8f0;
        border-color: #3b82f6;
    }
    QPushButton:checked {
        background: #2563eb;
        color: #ffffff;
        border: 1px solid #3b82f6;
    }
    QLabel {
        color: #64748b;
        font-size: 10px;
        font-weight: 700;
        letter-spacing: 0.8px;
        background: transparent;
        padding: 0;
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
    , m_target(new EditorTarget(this))
{
    setWindowTitle("GYYM Studio");
    setMinimumSize(1160, 720);
    resize(1400, 860);
    setAcceptDrops(true);

    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto* rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // --- Top Bar ---
    m_topBar = new TopBar(this);
    rootLayout->addWidget(m_topBar);

    // --- Main splitter: AssetExplorer | Workspace | ControlPanel ---
    auto* mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setStyleSheet(kSplitterStyle);
    mainSplitter->setHandleWidth(2);

    // Left: Asset Explorer
    m_assetExplorer = new AssetExplorer(mainSplitter);
    m_assetExplorer->setAssetInUseCallback([this](const QString& path) {
        if (m_service->loadedFilePath() == path)
            return true;
        for (auto* s : m_stickers) {
            if (s->sourcePath() == path)
                return true;
        }
        return false;
    });

    // Center: Workspace
    auto* workspaceWidget = new QWidget(mainSplitter);
    workspaceWidget->setStyleSheet("background: #090d16;");
    auto* workspaceLayout = new QVBoxLayout(workspaceWidget);
    workspaceLayout->setContentsMargins(0, 0, 0, 0);
    workspaceLayout->setSpacing(0);

    // Navigation tool bar
    auto* navBar = new QWidget(workspaceWidget);
    navBar->setObjectName("navToolBar");
    navBar->setFixedHeight(40);
    navBar->setStyleSheet(kNavToolBarStyle);

    auto* navLayout = new QHBoxLayout(navBar);
    navLayout->setContentsMargins(14, 0, 14, 0);
    navLayout->setSpacing(8);

    auto* navLabel = new QLabel("CANVAS TOOLS", navBar);
    navLayout->addWidget(navLabel);

    m_rotateViewBtn = new QPushButton("\xF0\x9F\x94\x84  Rotate View", navBar);
    m_rotateViewBtn->setCheckable(true);
    m_rotateViewBtn->setCursor(Qt::PointingHandCursor);
    m_rotateViewBtn->setToolTip("Toggle non-destructive canvas view rotation (Ctrl+drag in canvas)");
    connect(m_rotateViewBtn, &QPushButton::toggled, this, &MainWindow::onToggleRotateView);
    navLayout->addWidget(m_rotateViewBtn);

    navLayout->addStretch(1);

    // --- Zoom controls in nav bar ---
    auto* fitBothBtn = new QPushButton("Fit", navBar);
    fitBothBtn->setCursor(Qt::PointingHandCursor);
    fitBothBtn->setToolTip("Fit both viewports to screen");
    fitBothBtn->setFixedHeight(28);
    connect(fitBothBtn, &QPushButton::clicked, this, [this]() {
        if (m_originalView)  { m_originalView->fitToView(); }
        if (m_processedView) { m_processedView->fitToView(); }
        // Sync slider after fit
        if (m_processedView) updateZoomSliderFromView(m_processedView->zoomPercent());
    });
    navLayout->addWidget(fitBothBtn);

    auto* zoomMinusBtn = new QPushButton("-", navBar);
    zoomMinusBtn->setCursor(Qt::PointingHandCursor);
    zoomMinusBtn->setToolTip("Zoom Out");
    zoomMinusBtn->setFixedSize(28, 28);
    connect(zoomMinusBtn, &QPushButton::clicked, this, [this]() {
        if (m_originalView)  m_originalView->zoomOut();
        if (m_processedView) { m_processedView->zoomOut(); updateZoomSliderFromView(m_processedView->zoomPercent()); }
    });
    navLayout->addWidget(zoomMinusBtn);

    m_zoomSlider = new QSlider(Qt::Horizontal, navBar);
    m_zoomSlider->setRange(5, 500);
    m_zoomSlider->setValue(100);
    m_zoomSlider->setFixedWidth(120);
    m_zoomSlider->setToolTip("Zoom level (5% - 500%)");
    m_zoomSlider->setStyleSheet(kAdjSliderStyle);
    connect(m_zoomSlider, &QSlider::valueChanged, this, &MainWindow::onZoomSliderChanged);
    navLayout->addWidget(m_zoomSlider);

    auto* zoomPlusBtn = new QPushButton("+", navBar);
    zoomPlusBtn->setCursor(Qt::PointingHandCursor);
    zoomPlusBtn->setToolTip("Zoom In");
    zoomPlusBtn->setFixedSize(28, 28);
    connect(zoomPlusBtn, &QPushButton::clicked, this, [this]() {
        if (m_originalView)  m_originalView->zoomIn();
        if (m_processedView) { m_processedView->zoomIn(); updateZoomSliderFromView(m_processedView->zoomPercent()); }
    });
    navLayout->addWidget(zoomPlusBtn);

    m_zoomPctLabel = new QLabel("100%", navBar);
    m_zoomPctLabel->setFixedWidth(42);
    m_zoomPctLabel->setAlignment(Qt::AlignCenter);
    navLayout->addWidget(m_zoomPctLabel);

    workspaceLayout->addWidget(navBar);

    // Workspace panels: Original | Filtered
    auto* panelsWidget = new QWidget(workspaceWidget);
    auto* panelsLayout = new QHBoxLayout(panelsWidget);
    panelsLayout->setContentsMargins(8, 8, 8, 8);
    panelsLayout->setSpacing(8);

    m_originalView  = new ImageViewWidget("\xF0\x9F\x96\xBC  Original", false, panelsWidget);
    m_originalView->setEmptyStateText("No image loaded", "Load an image to get started");
    // Sync zoom slider when original view zoom changes
    connect(m_originalView, &ImageViewWidget::zoomChanged, this, &MainWindow::onViewZoomChanged);

    m_processedView = new ImageViewWidget("\xE2\x9C\xA8  Filtered", true, panelsWidget);
    m_processedView->setEmptyStateText("No document loaded", "Load an image to get started");
    // Sync zoom slider when processed view zoom changes
    connect(m_processedView, &ImageViewWidget::zoomChanged, this, &MainWindow::onViewZoomChanged);

    panelsLayout->addWidget(m_originalView, 1);
    panelsLayout->addWidget(m_processedView, 1);
    workspaceLayout->addWidget(panelsWidget, 1);

    // Borrow scene reference from processed view for stickers & selection
    m_processedScene = m_processedView->scene();

    // Initialize CropTool on processed view
    m_cropTool = new CropTool(m_processedView->graphicsView(), m_processedView->scene(), this);
    m_processedView->setCropTool(m_cropTool);

    // Connect external drag & drop from image views
    connect(m_originalView, &ImageViewWidget::fileDropped, this, &MainWindow::handleWorkspaceImageDropped);
    connect(m_processedView, &ImageViewWidget::fileDropped, this, &MainWindow::handleWorkspaceImageDropped);

    // Right: Controls
    QWidget* controlPanel = createControlPanel();

    mainSplitter->addWidget(m_assetExplorer);
    mainSplitter->addWidget(workspaceWidget);
    mainSplitter->addWidget(controlPanel);

    mainSplitter->setStretchFactor(0, 0);
    mainSplitter->setStretchFactor(1, 1);
    mainSplitter->setStretchFactor(2, 0);
    mainSplitter->setSizes({200, 880, 320});

    rootLayout->addWidget(mainSplitter, 1);

    // --- Status Bar ---
    m_statusBar = new StatusBar(this);
    rootLayout->addWidget(m_statusBar);

    connect(m_statusBar, &StatusBar::fitToViewClicked, this, [this]() {
        if (m_originalView)  m_originalView->fitToView();
        if (m_processedView) m_processedView->fitToView();
        if (m_processedView) updateZoomSliderFromView(m_processedView->zoomPercent());
    });
    connect(m_statusBar, &StatusBar::zoomInClicked, this, [this]() {
        if (m_originalView)  m_originalView->zoomIn();
        if (m_processedView) { m_processedView->zoomIn(); updateZoomSliderFromView(m_processedView->zoomPercent()); }
    });
    connect(m_statusBar, &StatusBar::zoomOutClicked, this, [this]() {
        if (m_originalView)  m_originalView->zoomOut();
        if (m_processedView) { m_processedView->zoomOut(); updateZoomSliderFromView(m_processedView->zoomPercent()); }
    });
    connect(m_statusBar, &StatusBar::zoomValueChanged, this, [this](double pct) {
        if (m_originalView)  m_originalView->setZoom(pct);
        if (m_processedView) m_processedView->setZoom(pct);
        updateZoomSliderFromView(pct);
    });
    connect(m_originalView, &ImageViewWidget::zoomChanged, m_statusBar, &StatusBar::setZoomPercent);
    connect(m_processedView, &ImageViewWidget::zoomChanged, m_statusBar, &StatusBar::setZoomPercent);

    // --- Connect Focus System ---
    connect(m_target, &EditorTarget::focusChanged, this, &MainWindow::onFocusChanged);
    connect(m_processedView, &ImageViewWidget::baseImageClicked, this, [this]() {
        m_target->focusBaseImage();
    });

    // --- Connect TopBar Actions ---
    connect(m_topBar, &TopBar::loadClicked,  this, &MainWindow::onLoadImage);
    connect(m_topBar, &TopBar::saveClicked,  this, &MainWindow::onSaveImage);
    connect(m_topBar, &TopBar::undoClicked,  this, &MainWindow::onUndo);
    connect(m_topBar, &TopBar::redoClicked,  this, &MainWindow::onRedo);
    connect(m_topBar, &TopBar::clearClicked, this, &MainWindow::onClear);

    // --- Connect Service Signals ---
    connect(m_service, &ImageEditorService::imageLoaded,    this, &MainWindow::onImageLoaded);
    connect(m_service, &ImageEditorService::imageProcessed, this, &MainWindow::onImageProcessed);
    connect(m_service, &ImageEditorService::historyChanged, this, &MainWindow::onHistoryChanged);
    connect(m_service, &ImageEditorService::errorOccurred,  this, &MainWindow::onError);

    // --- Connect Asset Explorer ---
    connect(m_assetExplorer, &AssetExplorer::imageActivated,   this, &MainWindow::onImageActivated);
    connect(m_assetExplorer, &AssetExplorer::stickerActivated, this, &MainWindow::onStickerActivated);

    // --- Keyboard Shortcuts (Non-intrusive) ---
    auto* undoShortcut = new QShortcut(QKeySequence::Undo, this);
    connect(undoShortcut, &QShortcut::activated, this, &MainWindow::onUndo);

    auto* redoShortcut = new QShortcut(QKeySequence::Redo, this);
    connect(redoShortcut, &QShortcut::activated, this, &MainWindow::onRedo);

    auto* redoShortcut2 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Y), this);
    connect(redoShortcut2, &QShortcut::activated, this, &MainWindow::onRedo);

    auto* openShortcut = new QShortcut(QKeySequence::Open, this);
    connect(openShortcut, &QShortcut::activated, this, &MainWindow::onLoadImage);

    auto* saveShortcut = new QShortcut(QKeySequence::Save, this);
    connect(saveShortcut, &QShortcut::activated, this, &MainWindow::onSaveImage);

    auto* delShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(delShortcut, &QShortcut::activated, this, [this]() {
        if (dynamic_cast<QLineEdit*>(focusWidget())) return;
        if (m_assetExplorer && m_assetExplorer->hasFocusedList()) {
            m_assetExplorer->deleteSelectedAsset();
        } else if (m_target->isSticker()) {
            onRemoveSelectedSticker();
        }
    });

    auto* backspaceShortcut = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
    connect(backspaceShortcut, &QShortcut::activated, this, [this]() {
        if (dynamic_cast<QLineEdit*>(focusWidget())) return;
        if (m_target->isSticker()) {
            onRemoveSelectedSticker();
        }
    });

    auto* escShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(escShortcut, &QShortcut::activated, this, [this]() {
        if (m_selectionTool && m_selectionTool->hasSelection()) {
            m_selectionTool->clearSelection();
            if (m_selectionToggle) {
                m_selectionToggle->blockSignals(true);
                m_selectionToggle->setChecked(false);
                m_selectionToggle->blockSignals(false);
                m_selectionToggle->setStyleSheet(kSmallActionBtnStyle);
            }
        } else if (m_target->isSticker()) {
            m_target->focusBaseImage();
        }
    });
}

MainWindow::~MainWindow() = default;

// ---------------------------------------------------------------------------
// Control Panel & Tabs
// ---------------------------------------------------------------------------
QWidget* MainWindow::createControlPanel()
{
    auto* panel = new QWidget(this);
    panel->setObjectName("controlPanel");
    panel->setStyleSheet(kControlPanelStyle);
    panel->setMinimumWidth(300);
    panel->setMaximumWidth(360);

    auto* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_tabWidget = new QTabWidget(panel);
    m_tabWidget->setStyleSheet(kTabWidgetStyle);
    m_tabWidget->setDocumentMode(true);

    // 1. Adjustments tab (Sliders with Apply buttons + Sticker bar + Selection bar + Quick Filters)
    QWidget* adjTab = createAdjustmentsTab();
    m_tabWidget->addTab(adjTab, "\xE2\x98\x80  Adjust");

    // 2. Filters tab
    m_filtersTabPanel = new FiltersPanel(m_registry, "Filters", m_tabWidget);
    connect(m_filtersTabPanel, &FiltersPanel::filterSelected, this, &MainWindow::onFilterSelected);
    m_tabWidget->addTab(m_filtersTabPanel, "\xE2\x97\x86  Filters");

    // 3. Effects tab
    m_effectsTabPanel = new FiltersPanel(m_registry, "Effects", m_tabWidget);
    connect(m_effectsTabPanel, &FiltersPanel::filterSelected, this, &MainWindow::onFilterSelected);
    m_tabWidget->addTab(m_effectsTabPanel, "\xE2\x9C\xA8  Effects");

    // 4. Tools tab (Rotation buttons, flips, crop + Free angle rotation 0°-360°)
    auto* toolsContainer = new QWidget(m_tabWidget);
    auto* toolsLayout = new QVBoxLayout(toolsContainer);
    toolsLayout->setContentsMargins(0, 0, 0, 0);
    toolsLayout->setSpacing(0);

    m_toolsTabPanel = new FiltersPanel(m_registry, "Tools", toolsContainer);
    connect(m_toolsTabPanel, &FiltersPanel::filterSelected, this, &MainWindow::onFilterSelected);
    toolsLayout->addWidget(m_toolsTabPanel, 1);

    auto* rotFrame = new QFrame(toolsContainer);
    rotFrame->setStyleSheet("background: #0d1322; border-top: 1px solid #1e293b;");
    auto* rotFrameLay = new QVBoxLayout(rotFrame);
    rotFrameLay->setContentsMargins(12, 10, 12, 10);
    rotFrameLay->setSpacing(6);

    auto* rotHead = new QHBoxLayout();
    auto* rIcon = new QLabel("\xF0\x9F\x94\x84", rotFrame);
    rIcon->setStyleSheet("font-size: 14px; color: #22d3ee; background: transparent;");
    auto* rTitle = new QLabel(u8"Free Rotation (0°–360°)", rotFrame);
    rTitle->setStyleSheet("font-size: 12px; font-weight: 600; color: #cbd5e1; background: transparent;");
    auto* rVal = new QLabel("0\xC2\xB0", rotFrame);
    rVal->setStyleSheet("font-size: 11px; color: #8b9bb4; background: transparent;");
    rotHead->addWidget(rIcon);
    rotHead->addSpacing(4);
    rotHead->addWidget(rTitle);
    rotHead->addStretch(1);
    rotHead->addWidget(rVal);
    rotFrameLay->addLayout(rotHead);

    auto* rotControlLay = new QHBoxLayout();
    rotControlLay->setSpacing(8);
    auto* toolRotSlider = new QSlider(Qt::Horizontal, rotFrame);
    toolRotSlider->setRange(0, 360);
    toolRotSlider->setValue(0);
    toolRotSlider->setStyleSheet(kAdjSliderStyle);
    toolRotSlider->setToolTip("Free rotate: 0\xC2\xB0 \xe2\x80\x93 360\xC2\xB0");

    auto* toolApplyRotBtn = new QPushButton("Apply", rotFrame);
    toolApplyRotBtn->setStyleSheet(kApplyBtnStyle);
    toolApplyRotBtn->setCursor(Qt::PointingHandCursor);
    toolApplyRotBtn->setEnabled(false);
    toolApplyRotBtn->setToolTip("Apply free rotation");

    connect(toolRotSlider, &QSlider::valueChanged, this, [rVal, toolApplyRotBtn](int val) {
        rVal->setText(QString("%1\xC2\xB0").arg(val));
        toolApplyRotBtn->setEnabled(val != 0);
    });
    connect(toolApplyRotBtn, &QPushButton::clicked, this, [this, toolRotSlider, rVal, toolApplyRotBtn]() {
        int angle = toolRotSlider->value();
        if (angle == 0 || !m_service->hasImage()) return;
        QImage current = m_service->currentCommittedQImage();
        QTransform tfm;
        tfm.rotate(angle);
        QImage rotated = current.transformed(tfm, Qt::SmoothTransformation);
        if (!rotated.isNull()) {
            m_service->setProcessedFromQImage(rotated);
        }
        toolRotSlider->blockSignals(true);
        toolRotSlider->setValue(0);
        toolRotSlider->blockSignals(false);
        rVal->setText("0\xC2\xB0");
        toolApplyRotBtn->setEnabled(false);
    });

    rotControlLay->addWidget(toolRotSlider, 1);
    rotControlLay->addWidget(toolApplyRotBtn);
    rotFrameLay->addLayout(rotControlLay);

    toolsLayout->addWidget(rotFrame);
    m_tabWidget->addTab(toolsContainer, "\xF0\x9F\x94\xA7  Tools");

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

    // Helper for a slider row with an explicit Apply button beside it
    auto createSliderRow = [this, container](const QString& icon, const QString& name,
                                             int minVal, int maxVal, int defaultVal,
                                             const QString& suffix,
                                             QSlider*& sliderOut, QLabel*& valLabelOut,
                                             QPushButton*& applyBtnOut) {
        auto* row = new QVBoxLayout();
        row->setSpacing(4);

        auto* headerRow = new QHBoxLayout();
        auto* iconLabel = new QLabel(icon, container);
        iconLabel->setStyleSheet("font-size: 14px; color: #3b82f6; background: transparent;");

        auto* nameLabel = new QLabel(name, container);
        nameLabel->setStyleSheet("font-size: 12px; font-weight: 500; color: #cbd5e1; background: transparent;");

        valLabelOut = new QLabel(QString("%1%2").arg(defaultVal > 0 ? "+" : "").arg(defaultVal).arg(suffix), container);
        valLabelOut->setStyleSheet("font-size: 11px; color: #8b9bb4; background: transparent;");

        headerRow->addWidget(iconLabel);
        headerRow->addSpacing(4);
        headerRow->addWidget(nameLabel);
        headerRow->addStretch(1);
        headerRow->addWidget(valLabelOut);

        auto* sliderRow = new QHBoxLayout();
        sliderRow->setSpacing(8);

        sliderOut = new QSlider(Qt::Horizontal, container);
        sliderOut->setRange(minVal, maxVal);
        sliderOut->setValue(defaultVal);
        sliderOut->setStyleSheet(kAdjSliderStyle);

        applyBtnOut = new QPushButton("Apply", container);
        applyBtnOut->setStyleSheet(kApplyBtnStyle);
        applyBtnOut->setCursor(Qt::PointingHandCursor);
        applyBtnOut->setEnabled(false);
        applyBtnOut->setToolTip(QString("Commit %1 to the active target").arg(name));

        sliderRow->addWidget(sliderOut, 1);
        sliderRow->addWidget(applyBtnOut);

        row->addLayout(headerRow);
        row->addLayout(sliderRow);
        return row;
    };

    // --- Brightness ---
    auto* brightLayout = createSliderRow("\xE2\x98\x80", "Brightness", -100, 100, 0, "%",
                                         m_brightnessSlider, m_brightnessValLabel, m_applyBrightnessBtn);
    layout->addLayout(brightLayout);
    connect(m_brightnessSlider, &QSlider::valueChanged, this, &MainWindow::onBrightnessPreview);
    connect(m_applyBrightnessBtn, &QPushButton::clicked, this, &MainWindow::onApplyBrightness);

    // --- Contrast ---
    auto* contrastLayout = createSliderRow("\xE2\x97\x90", "Contrast", -100, 100, 0, "%",
                                           m_contrastSlider, m_contrastValLabel, m_applyContrastBtn);
    layout->addLayout(contrastLayout);
    connect(m_contrastSlider, &QSlider::valueChanged, this, &MainWindow::onContrastPreview);
    connect(m_applyContrastBtn, &QPushButton::clicked, this, &MainWindow::onApplyContrast);

    // --- Saturation ---
    auto* satLayout = createSliderRow("\xF0\x9F\x8E\xA8", "Saturation", -100, 100, 0, "%",
                                      m_saturationSlider, m_saturationValLabel, m_applySaturationBtn);
    layout->addLayout(satLayout);
    connect(m_saturationSlider, &QSlider::valueChanged, this, &MainWindow::onSaturationPreview);
    connect(m_applySaturationBtn, &QPushButton::clicked, this, &MainWindow::onApplySaturation);

    // --- Hue ---
    auto* hueLayout = createSliderRow("\xE2\x8A\x9B", "Hue", -180, 180, 0, "\xC2\xB0",
                                      m_hueSlider, m_hueValLabel, m_applyHueBtn);
    layout->addLayout(hueLayout);
    connect(m_hueSlider, &QSlider::valueChanged, this, &MainWindow::onHuePreview);
    connect(m_applyHueBtn, &QPushButton::clicked, this, &MainWindow::onApplyHue);

    layout->addSpacing(4);

    // --- Free Rotation (0°–360°) ---
    {
        auto* rotRow = new QVBoxLayout();
        rotRow->setSpacing(4);

        auto* rotHeader = new QHBoxLayout();
        auto* rotIcon = new QLabel("\xF0\x9F\x94\x84", container);
        rotIcon->setStyleSheet("font-size: 14px; color: #3b82f6; background: transparent;");
        auto* rotName = new QLabel("Free Rotation", container);
        rotName->setStyleSheet("font-size: 12px; font-weight: 500; color: #cbd5e1; background: transparent;");
        m_freeRotateValLabel = new QLabel("0\xC2\xB0", container);
        m_freeRotateValLabel->setStyleSheet("font-size: 11px; color: #8b9bb4; background: transparent;");
        rotHeader->addWidget(rotIcon);
        rotHeader->addSpacing(4);
        rotHeader->addWidget(rotName);
        rotHeader->addStretch(1);
        rotHeader->addWidget(m_freeRotateValLabel);

        auto* rotSliderRow = new QHBoxLayout();
        rotSliderRow->setSpacing(8);

        m_freeRotateSlider = new QSlider(Qt::Horizontal, container);
        m_freeRotateSlider->setRange(0, 360);
        m_freeRotateSlider->setValue(0);
        m_freeRotateSlider->setStyleSheet(kAdjSliderStyle);
        m_freeRotateSlider->setToolTip("Free rotate: 0\xC2\xB0 \xe2\x80\x93 360\xC2\xB0");

        m_applyFreeRotateBtn = new QPushButton("Apply", container);
        m_applyFreeRotateBtn->setStyleSheet(kApplyBtnStyle);
        m_applyFreeRotateBtn->setCursor(Qt::PointingHandCursor);
        m_applyFreeRotateBtn->setEnabled(false);
        m_applyFreeRotateBtn->setToolTip("Apply free rotation to the current image");

        connect(m_freeRotateSlider, &QSlider::valueChanged, this, &MainWindow::onFreeRotatePreview);
        connect(m_applyFreeRotateBtn, &QPushButton::clicked, this, &MainWindow::onApplyFreeRotate);

        rotSliderRow->addWidget(m_freeRotateSlider, 1);
        rotSliderRow->addWidget(m_applyFreeRotateBtn);

        rotRow->addLayout(rotHeader);
        rotRow->addLayout(rotSliderRow);
        layout->addLayout(rotRow);
    }

    layout->addSpacing(4);

    // --- Selection bar ---
    auto* selBar = createSelectionBar();
    layout->addWidget(selBar);


    layout->addSpacing(4);

    // --- Sticker bar ---
    auto* stkBar = createStickerBar();
    layout->addWidget(stkBar);

    layout->addSpacing(8);

    // --- Quick Filters ---
    m_quickFiltersPanel = new FiltersPanel(m_registry, "Filters", container);
    connect(m_quickFiltersPanel, &FiltersPanel::filterSelected, this, &MainWindow::onFilterSelected);
    layout->addWidget(m_quickFiltersPanel);

    scroll->setWidget(container);
    return scroll;
}

QWidget* MainWindow::createStickerBar()
{
    auto* box = new QWidget(this);
    box->setStyleSheet("background: #0c101c; border: 1px solid #1a2336; border-radius: 8px;");

    auto* lay = new QVBoxLayout(box);
    lay->setContentsMargins(10, 8, 10, 8);
    lay->setSpacing(6);

    auto* hdr = new QLabel("\xE2\xAD\x90  Sticker Controls", box);
    hdr->setStyleSheet("color: #38bdf8; font-size: 11px; font-weight: 700; background: transparent;");
    lay->addWidget(hdr);

    auto* row1 = new QHBoxLayout();
    row1->setSpacing(6);

    auto* addStickerBtn = new QPushButton("+ Add Sticker", box);
    addStickerBtn->setStyleSheet(kSmallActionBtnStyle);
    addStickerBtn->setCursor(Qt::PointingHandCursor);
    addStickerBtn->setToolTip("Import and place a sticker on the canvas");
    connect(addStickerBtn, &QPushButton::clicked, this, [this]() {
        QString path = QFileDialog::getOpenFileName(
            this, "Import Sticker", {},
            "Images (*.png *.jpg *.jpeg *.bmp *.tga);;All Files (*)");
        if (!path.isEmpty()) {
            QImage img(path);
            if (!img.isNull()) {
                m_assetExplorer->addSticker(path, img);
                placeSticker(path);
            }
        }
    });
    row1->addWidget(addStickerBtn, 1);

    m_removeStickerBtn = new QPushButton("\xF0\x9F\x97\x91 Remove", box);
    m_removeStickerBtn->setStyleSheet(kSmallActionBtnStyle);
    m_removeStickerBtn->setCursor(Qt::PointingHandCursor);
    m_removeStickerBtn->setEnabled(false);
    m_removeStickerBtn->setToolTip("Remove focused sticker");
    connect(m_removeStickerBtn, &QPushButton::clicked, this, &MainWindow::onRemoveSelectedSticker);
    row1->addWidget(m_removeStickerBtn);

    lay->addLayout(row1);

    // Layer ordering row: Move Up / Move Down
    auto* row2 = new QHBoxLayout();
    row2->setSpacing(6);

    m_moveUpStickerBtn = new QPushButton("\xE2\x96\xB2 Move Up", box);
    m_moveUpStickerBtn->setStyleSheet(kSmallActionBtnStyle);
    m_moveUpStickerBtn->setCursor(Qt::PointingHandCursor);
    m_moveUpStickerBtn->setEnabled(false);
    m_moveUpStickerBtn->setToolTip("Bring focused sticker one layer higher");
    connect(m_moveUpStickerBtn, &QPushButton::clicked, this, &MainWindow::onMoveUpSticker);
    row2->addWidget(m_moveUpStickerBtn);

    m_moveDownStickerBtn = new QPushButton("\xE2\x96\xBC Move Down", box);
    m_moveDownStickerBtn->setStyleSheet(kSmallActionBtnStyle);
    m_moveDownStickerBtn->setCursor(Qt::PointingHandCursor);
    m_moveDownStickerBtn->setEnabled(false);
    m_moveDownStickerBtn->setToolTip("Send focused sticker one layer lower");
    connect(m_moveDownStickerBtn, &QPushButton::clicked, this, &MainWindow::onMoveDownSticker);
    row2->addWidget(m_moveDownStickerBtn);

    lay->addLayout(row2);

    m_stickerStatusLabel = new QLabel("No sticker selected", box);
    m_stickerStatusLabel->setStyleSheet("color: #64748b; font-size: 10px; background: transparent;");
    lay->addWidget(m_stickerStatusLabel);

    return box;
}

QWidget* MainWindow::createSelectionBar()
{
    auto* box = new QWidget(this);
    box->setStyleSheet("background: #0c101c; border: 1px solid #1a2336; border-radius: 8px;");

    auto* lay = new QVBoxLayout(box);
    lay->setContentsMargins(10, 8, 10, 8);
    lay->setSpacing(6);

    auto* hdr = new QLabel("\xE2\xAC\x9B  Region Filter", box);
    hdr->setStyleSheet("color: #38bdf8; font-size: 11px; font-weight: 700; background: transparent;");
    lay->addWidget(hdr);

    auto* row1 = new QHBoxLayout();
    row1->setSpacing(6);

    // Select Area toggle button with active/inactive visual state
    m_selectionToggle = new QPushButton("\xE2\x96\xA2 Select Area", box);
    m_selectionToggle->setCheckable(true);
    m_selectionToggle->setStyleSheet(kSmallActionBtnStyle);
    m_selectionToggle->setCursor(Qt::PointingHandCursor);
    m_selectionToggle->setToolTip("Activate region selection tool to draw an area on the canvas");
    connect(m_selectionToggle, &QPushButton::toggled, this, [this](bool checked) {
        // Visual active state: green border when active
        if (checked) {
            m_selectionToggle->setStyleSheet(QString(kSmallActionBtnStyle) +
                R"(
                QPushButton {
                    border: 2px solid #22c55e;
                    color: #22c55e;
                }
                QPushButton:hover {
                    border: 2px solid #4ade80;
                    color: #4ade80;
                }
                )");
        } else {
            m_selectionToggle->setStyleSheet(kSmallActionBtnStyle);
        }
        onToggleSelectionTool(checked);
    });
    row1->addWidget(m_selectionToggle, 1);

    auto* cropBtn = new QPushButton("Crop", box);
    cropBtn->setStyleSheet(kSmallActionBtnStyle);
    cropBtn->setCursor(Qt::PointingHandCursor);
    cropBtn->setToolTip("Crop image to selected region");
    connect(cropBtn, &QPushButton::clicked, this, [this]() {
        if (!m_service->hasImage()) return;
        if (!m_selectionTool || !m_selectionTool->hasSelection()) {
            if (m_selectionToggle) m_selectionToggle->setChecked(true);
            return;
        }
        QRectF r = m_selectionTool->selectionRect();
        int x = qMax(0, qRound(r.x()));
        int y = qMax(0, qRound(r.y()));
        int w = qRound(r.width());
        int h = qRound(r.height());
        if (x + w > m_service->imageWidth())  w = m_service->imageWidth() - x;
        if (y + h > m_service->imageHeight()) h = m_service->imageHeight() - y;
        if (w > 0 && h > 0) {
            m_service->applyCrop(x, y, w, h);
            m_selectionTool->clearSelection();
            if (m_selectionToggle) m_selectionToggle->setChecked(false);
        }
    });
    row1->addWidget(cropBtn);

    auto* clearSelBtn = new QPushButton("Clear", box);
    clearSelBtn->setStyleSheet(kSmallActionBtnStyle);
    clearSelBtn->setCursor(Qt::PointingHandCursor);
    clearSelBtn->setToolTip("Clear the active region selection");
    connect(clearSelBtn, &QPushButton::clicked, this, [this]() {
        if (m_selectionTool) m_selectionTool->clearSelection();
        if (m_selectionToggle) {
            m_selectionToggle->blockSignals(true);
            m_selectionToggle->setChecked(false);
            m_selectionToggle->blockSignals(false);
            m_selectionToggle->setStyleSheet(kSmallActionBtnStyle);
        }
        if (m_selectionTool) m_selectionTool->setActive(false);
    });
    row1->addWidget(clearSelBtn);

    lay->addLayout(row1);

    auto* row2 = new QHBoxLayout();
    row2->setSpacing(6);

    m_selectionShapeCombo = new QComboBox(box);
    m_selectionShapeCombo->addItem("Rectangle",    SelectionTool::Rectangle);
    m_selectionShapeCombo->addItem("Ellipse",      SelectionTool::Ellipse);
    m_selectionShapeCombo->addItem("Magic Wand",   SelectionTool::MagicWand);
    m_selectionShapeCombo->setStyleSheet(R"(
        QComboBox {
            background: #141c2e; color: #cbd5e1;
            border: 1px solid #243049; border-radius: 6px;
            padding: 3px 6px; font-size: 11px;
        }
    )");
    connect(m_selectionShapeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int idx) {
        if (m_selectionTool) {
            auto shape = static_cast<SelectionTool::Shape>(
                m_selectionShapeCombo->itemData(idx).toInt());
            m_selectionTool->setShape(shape);
            if (m_toleranceSlider) {
                m_selectionTool->setTolerance(m_toleranceSlider->value());
            }
        }
    });
    row2->addWidget(m_selectionShapeCombo, 1);

    m_selectionModeCombo = new QComboBox(box);
    m_selectionModeCombo->addItem("Inside",  SelectionTool::InsideSelection);
    m_selectionModeCombo->addItem("Outside", SelectionTool::OutsideSelection);
    m_selectionModeCombo->setStyleSheet(R"(
        QComboBox {
            background: #141c2e; color: #cbd5e1;
            border: 1px solid #243049; border-radius: 6px;
            padding: 3px 6px; font-size: 11px;
        }
    )");
    row2->addWidget(m_selectionModeCombo, 1);

    lay->addLayout(row2);

    auto* tolRow = new QHBoxLayout();
    tolRow->setSpacing(6);

    auto* tolLabel = new QLabel("Tolerance:", box);
    tolLabel->setStyleSheet("color: #94a3b8; font-size: 11px; font-weight: 500; background: transparent;");
    tolRow->addWidget(tolLabel);

    m_toleranceSlider = new QSlider(Qt::Horizontal, box);
    m_toleranceSlider->setObjectName("magicWandToleranceSlider");
    m_toleranceSlider->setRange(0, 100);
    m_toleranceSlider->setValue(42);
    m_toleranceSlider->setTracking(true);
    m_toleranceSlider->setStyleSheet(kAdjSliderStyle);
    m_toleranceSlider->setToolTip("Magic Wand color tolerance (0% \xe2\x80\x93 100%)");
    connect(m_toleranceSlider, &QSlider::valueChanged, this, [this](int val) {
        if (m_toleranceValLabel) {
            m_toleranceValLabel->setText(QString("%1%").arg(val));
        }
        if (m_selectionTool) {
            m_selectionTool->setTolerance(val);
        }
    });
    tolRow->addWidget(m_toleranceSlider, 1);

    m_toleranceValLabel = new QLabel("42%", box);
    m_toleranceValLabel->setObjectName("magicWandToleranceValLabel");
    m_toleranceValLabel->setStyleSheet("color: #cbd5e1; font-size: 11px; font-weight: 600; min-width: 32px; background: transparent;");
    tolRow->addWidget(m_toleranceValLabel);

    lay->addLayout(tolRow);

    auto* row3 = new QHBoxLayout();
    row3->setSpacing(6);

    m_freeCropToggle = new QPushButton("\xE2\x9C\x82  Free Crop", box);
    m_freeCropToggle->setCheckable(true);
    m_freeCropToggle->setStyleSheet(kSmallActionBtnStyle);
    m_freeCropToggle->setCursor(Qt::PointingHandCursor);
    m_freeCropToggle->setToolTip("Activate interactive free crop box on the canvas");
    connect(m_freeCropToggle, &QPushButton::toggled, this, &MainWindow::onToggleFreeCrop);
    row3->addWidget(m_freeCropToggle, 1);

    m_applyFreeCropBtn = new QPushButton("Apply", box);
    m_applyFreeCropBtn->setStyleSheet(kSmallActionBtnStyle);
    m_applyFreeCropBtn->setCursor(Qt::PointingHandCursor);
    m_applyFreeCropBtn->setEnabled(false);
    m_applyFreeCropBtn->setToolTip("Apply interactive crop to current image");
    connect(m_applyFreeCropBtn, &QPushButton::clicked, this, &MainWindow::onApplyFreeCrop);
    row3->addWidget(m_applyFreeCropBtn);

    auto* resizeBtn = new QPushButton("\xE2\xA4\xA2  Resize", box);
    resizeBtn->setStyleSheet(kSmallActionBtnStyle);
    resizeBtn->setCursor(Qt::PointingHandCursor);
    resizeBtn->setToolTip("Resize image dimensions with optional aspect ratio preservation");
    connect(resizeBtn, &QPushButton::clicked, this, &MainWindow::onOpenResizeDialog);
    row3->addWidget(resizeBtn, 1);

    lay->addLayout(row3);
    return box;
}

// ---------------------------------------------------------------------------
// Focus System
// ---------------------------------------------------------------------------
void MainWindow::onFocusChanged()
{
    if (m_target->isSticker()) {
        StickerItem* focused = m_target->sticker();
        for (auto* s : m_stickers) {
            s->setSelectedVisual(s == focused);
        }
        m_processedView->setActiveGlow(false);
        m_removeStickerBtn->setEnabled(true);
        updateStickerLayerButtons();
        m_topBar->setUndoEnabled(focused->canUndo());
        m_topBar->setRedoEnabled(focused->canRedo());
        m_stickerStatusLabel->setText("Sticker focused");
    } else {
        // Base image focused
        for (auto* s : m_stickers) {
            s->setSelectedVisual(false);
        }
        m_processedView->setActiveGlow(m_service->hasImage());
        m_removeStickerBtn->setEnabled(false);
        if (m_moveUpStickerBtn) m_moveUpStickerBtn->setEnabled(false);
        if (m_moveDownStickerBtn) m_moveDownStickerBtn->setEnabled(false);
        m_topBar->setUndoEnabled(m_service->canUndo());
        m_topBar->setRedoEnabled(m_service->canRedo());
        m_stickerStatusLabel->setText(m_stickers.isEmpty() ? "No stickers" :
            QString("%1 sticker(s) on canvas").arg(m_stickers.size()));
    }
}

// ---------------------------------------------------------------------------
// Image I/O & Document Management
// ---------------------------------------------------------------------------
void MainWindow::onLoadImage()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "Open Image", {},
        "Images (*.png *.jpg *.jpeg *.bmp *.tga);;All Files (*)");
    if (!filePath.isEmpty()) {
        m_service->loadImage(filePath);
        QImage preview(filePath);
        if (!preview.isNull())
            m_assetExplorer->addImage(filePath, preview);
    }
}

void MainWindow::onSaveImage()
{
    if (!m_service->hasImage()) return;

    SaveImageDialog dlg(m_service->loadedFilePath(), m_service->imageFormat(), this);
    if (dlg.exec() == QDialog::Accepted) {
        QString savePath = dlg.selectedFilePath();
        if (savePath.isEmpty()) return;

        // Final raster compositing at base image pixel dimensions
        QImage baseImg = m_service->currentCommittedQImage();
        if (baseImg.isNull()) return;

        QImage finalImage(baseImg.size(), QImage::Format_ARGB32);
        finalImage.fill(Qt::transparent);

        QPainter p(&finalImage);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setRenderHint(QPainter::SmoothPixmapTransform, true);
        p.drawImage(0, 0, baseImg);

        // Sort stickers by z-order for correct top-to-bottom layering
        auto sortedStickers = m_stickers;
        std::sort(sortedStickers.begin(), sortedStickers.end(),
                  [](StickerItem* a, StickerItem* b) { return a->zValue() < b->zValue(); });

        for (auto* s : sortedStickers) {
            p.save();
            p.translate(s->pos());
            p.translate(s->transformOriginPoint());
            p.rotate(s->rotation());
            p.scale(s->scale(), s->scale());
            p.translate(-s->transformOriginPoint());
            p.drawImage(0, 0, s->committedImage());
            p.restore();
        }
        p.end();

        // Save final raster image directly
        bool ok = finalImage.save(savePath);
        if (!ok) {
            // Fallback via service
            m_service->setProcessedFromQImage(finalImage);
            m_service->saveImage(savePath);
        }
    }
}

void MainWindow::onClear()
{
    m_service->clear();
    m_originalView->clearImage();
    m_processedView->clearImage();
    m_processedView->setActiveGlow(false);

    clearAllStickers();
    m_target->focusBaseImage();

    if (m_selectionTool) {
        m_selectionTool->clearSelection();
        if (m_selectionToggle) m_selectionToggle->setChecked(false);
    }
    onCancelFreeCrop();

    if (m_quickFiltersPanel) m_quickFiltersPanel->clearSelection();
    if (m_filtersTabPanel)   m_filtersTabPanel->clearSelection();
    if (m_effectsTabPanel)   m_effectsTabPanel->clearSelection();
    if (m_toolsTabPanel)     m_toolsTabPanel->clearSelection();

    resetAdjustmentSliders();
}

void MainWindow::onUndo()
{
    if (m_target->isSticker()) {
        StickerItem* s = m_target->sticker();
        s->undo();
        m_topBar->setUndoEnabled(s->canUndo());
        m_topBar->setRedoEnabled(s->canRedo());
    } else {
        m_service->undo();
    }
}

void MainWindow::onRedo()
{
    if (m_target->isSticker()) {
        StickerItem* s = m_target->sticker();
        s->redo();
        m_topBar->setUndoEnabled(s->canUndo());
        m_topBar->setRedoEnabled(s->canRedo());
    } else {
        m_service->redo();
    }
}

// ---------------------------------------------------------------------------
// Filter Selection (Target-Aware + Original Reset)
// ---------------------------------------------------------------------------
void MainWindow::onFilterSelected(int index)
{
    if (index < 0) {
        // "Original" card clicked
        if (m_target->isSticker()) {
            StickerItem* sticker = m_target->sticker();
            sticker->pushSnapshot();
            sticker->resetFilter();
            m_topBar->setUndoEnabled(sticker->canUndo());
            m_topBar->setRedoEnabled(sticker->canRedo());
        } else {
            m_service->resetToOriginal();
        }
        return;
    }

    const auto& allFilters = m_registry->filters();
    if (index >= 0 && index < allFilters.size() && allFilters[index].name.contains("Crop")) {
        if (m_selectionTool && m_selectionTool->hasSelection() && m_service->hasImage()) {
            QRectF r = m_selectionTool->selectionRect();
            int x = qMax(0, qRound(r.x()));
            int y = qMax(0, qRound(r.y()));
            int w = qRound(r.width());
            int h = qRound(r.height());
            if (x + w > m_service->imageWidth())  w = m_service->imageWidth() - x;
            if (y + h > m_service->imageHeight()) h = m_service->imageHeight() - y;
            if (w > 0 && h > 0) {
                m_service->applyCrop(x, y, w, h);
                m_selectionTool->clearSelection();
                if (m_selectionToggle) m_selectionToggle->setChecked(false);
            }
        } else {
            if (m_selectionToggle) m_selectionToggle->setChecked(true);
        }
        return;
    }

    if (index >= 0 && index < allFilters.size() && allFilters[index].name.contains("Resize")) {
        onOpenResizeDialog();
        return;
    }

    if (m_target->isSticker()) {
        onApplyFilterToSticker(index);
        return;
    }

    // Base image focused
    if (m_selectionTool && m_selectionTool->hasSelection() && m_service->hasImage()) {
        onApplyFilterToRegion(index);
        return;
    }

    m_service->applyFilter(index);

    if (m_quickFiltersPanel) m_quickFiltersPanel->selectFilter(index);
    if (m_filtersTabPanel)   m_filtersTabPanel->selectFilter(index);
    if (m_effectsTabPanel)   m_effectsTabPanel->selectFilter(index);
    if (m_toolsTabPanel)     m_toolsTabPanel->selectFilter(index);
}

// ---------------------------------------------------------------------------
// Adjustment Sliders (Preview on Slider Change, Commit on Apply)
// ---------------------------------------------------------------------------
void MainWindow::onBrightnessPreview(int value)
{
    m_brightnessValLabel->setText(QString("%1%2%").arg(value > 0 ? "+" : "").arg(value));
    m_applyBrightnessBtn->setEnabled(value != 0);

    if (m_target->isSticker()) {
        StickerItem* s = m_target->sticker();
        if (value == 0) {
            s->clearPreview();
        } else {
            QImage prev = m_service->applyBrightnessToQImage(value, s->committedImage());
            s->setPreviewImage(prev);
        }
    } else {
        if (!m_service->hasImage()) return;
        if (value == 0) {
            m_processedView->setImage(m_service->currentCommittedQImage());
        } else {
            QImage prev = m_service->applyBrightnessToQImage(value, m_service->currentCommittedQImage());
            m_processedView->setImage(prev);
        }
    }
}

void MainWindow::onApplyBrightness()
{
    int val = m_brightnessSlider->value();
    if (val == 0) return;

    if (m_target->isSticker()) {
        StickerItem* s = m_target->sticker();
        s->pushSnapshot();
        QImage committed = m_service->applyBrightnessToQImage(val, s->committedImage());
        s->setCommittedImage(committed);
        m_topBar->setUndoEnabled(s->canUndo());
        m_topBar->setRedoEnabled(s->canRedo());
    } else {
        m_service->applyBrightness(val);
    }

    m_brightnessSlider->blockSignals(true);
    m_brightnessSlider->setValue(0);
    m_brightnessSlider->blockSignals(false);
    m_brightnessValLabel->setText("0%");
    m_applyBrightnessBtn->setEnabled(false);
}

void MainWindow::onContrastPreview(int value)
{
    m_contrastValLabel->setText(QString("%1%2%").arg(value > 0 ? "+" : "").arg(value));
    m_applyContrastBtn->setEnabled(value != 0);

    if (m_target->isSticker()) {
        StickerItem* s = m_target->sticker();
        if (value == 0) {
            s->clearPreview();
        } else {
            QImage prev = m_service->applyContrastToQImage(value, s->committedImage());
            s->setPreviewImage(prev);
        }
    } else {
        if (!m_service->hasImage()) return;
        if (value == 0) {
            m_processedView->setImage(m_service->currentCommittedQImage());
        } else {
            QImage prev = m_service->applyContrastToQImage(value, m_service->currentCommittedQImage());
            m_processedView->setImage(prev);
        }
    }
}

void MainWindow::onApplyContrast()
{
    int val = m_contrastSlider->value();
    if (val == 0) return;

    if (m_target->isSticker()) {
        StickerItem* s = m_target->sticker();
        s->pushSnapshot();
        QImage committed = m_service->applyContrastToQImage(val, s->committedImage());
        s->setCommittedImage(committed);
        m_topBar->setUndoEnabled(s->canUndo());
        m_topBar->setRedoEnabled(s->canRedo());
    } else {
        m_service->applyContrast(val);
    }

    m_contrastSlider->blockSignals(true);
    m_contrastSlider->setValue(0);
    m_contrastSlider->blockSignals(false);
    m_contrastValLabel->setText("0%");
    m_applyContrastBtn->setEnabled(false);
}

void MainWindow::onSaturationPreview(int value)
{
    m_saturationValLabel->setText(QString("%1%2%").arg(value > 0 ? "+" : "").arg(value));
    m_applySaturationBtn->setEnabled(value != 0);

    if (m_target->isSticker()) {
        StickerItem* s = m_target->sticker();
        if (value == 0) {
            s->clearPreview();
        } else {
            QImage prev = m_service->applySaturationToQImage(value, s->committedImage());
            s->setPreviewImage(prev);
        }
    } else {
        if (!m_service->hasImage()) return;
        if (value == 0) {
            m_processedView->setImage(m_service->currentCommittedQImage());
        } else {
            QImage prev = m_service->applySaturationToQImage(value, m_service->currentCommittedQImage());
            m_processedView->setImage(prev);
        }
    }
}

void MainWindow::onApplySaturation()
{
    int val = m_saturationSlider->value();
    if (val == 0) return;

    if (m_target->isSticker()) {
        StickerItem* s = m_target->sticker();
        s->pushSnapshot();
        QImage committed = m_service->applySaturationToQImage(val, s->committedImage());
        s->setCommittedImage(committed);
        m_topBar->setUndoEnabled(s->canUndo());
        m_topBar->setRedoEnabled(s->canRedo());
    } else {
        m_service->applySaturation(val);
    }

    m_saturationSlider->blockSignals(true);
    m_saturationSlider->setValue(0);
    m_saturationSlider->blockSignals(false);
    m_saturationValLabel->setText("0%");
    m_applySaturationBtn->setEnabled(false);
}

void MainWindow::onHuePreview(int value)
{
    m_hueValLabel->setText(QString("%1\xC2\xB0").arg(value));
    m_applyHueBtn->setEnabled(value != 0);

    if (m_target->isSticker()) {
        StickerItem* s = m_target->sticker();
        if (value == 0) {
            s->clearPreview();
        } else {
            QImage prev = m_service->applyHueToQImage(value, s->committedImage());
            s->setPreviewImage(prev);
        }
    } else {
        if (!m_service->hasImage()) return;
        if (value == 0) {
            m_processedView->setImage(m_service->currentCommittedQImage());
        } else {
            QImage prev = m_service->applyHueToQImage(value, m_service->currentCommittedQImage());
            m_processedView->setImage(prev);
        }
    }
}

void MainWindow::onApplyHue()
{
    int val = m_hueSlider->value();
    if (val == 0) return;

    if (m_target->isSticker()) {
        StickerItem* s = m_target->sticker();
        s->pushSnapshot();
        QImage committed = m_service->applyHueToQImage(val, s->committedImage());
        s->setCommittedImage(committed);
        m_topBar->setUndoEnabled(s->canUndo());
        m_topBar->setRedoEnabled(s->canRedo());
    } else {
        m_service->applyHue(val);
    }

    m_hueSlider->blockSignals(true);
    m_hueSlider->setValue(0);
    m_hueSlider->blockSignals(false);
    m_hueValLabel->setText("0\xC2\xB0");
    m_applyHueBtn->setEnabled(false);
}

void MainWindow::onFreeRotatePreview(int angle)
{
    if (m_freeRotateValLabel)
        m_freeRotateValLabel->setText(QString("%1\xC2\xB0").arg(angle));
    if (m_applyFreeRotateBtn)
        m_applyFreeRotateBtn->setEnabled(angle != 0);

    if (m_target->isSticker()) {
        StickerItem* s = m_target->sticker();
        if (s) {
            QImage prev = (angle == 0)
                ? s->committedImage()
                : m_service->applyRotationToQImage(angle, s->committedImage());
            s->setPreviewImage(prev);
        }
    } else {
        if (!m_service->hasImage()) return;
        QImage prev = (angle == 0)
            ? m_service->currentCommittedQImage()
            : m_service->applyRotationToQImage(angle, m_service->currentCommittedQImage());
        if (m_processedView) {
            m_processedView->setImage(prev);
        }
    }
}

void MainWindow::onApplyFreeRotate()
{
    int angle = m_freeRotateSlider ? m_freeRotateSlider->value() : 0;
    if (angle == 0) return;

    if (m_target->isSticker()) {
        StickerItem* s = m_target->sticker();
        if (!s) return;
        s->pushSnapshot();
        QImage committed = m_service->applyRotationToQImage(angle, s->committedImage());
        s->setCommittedImage(committed);
        m_topBar->setUndoEnabled(s->canUndo());
        m_topBar->setRedoEnabled(s->canRedo());
    } else {
        m_service->applyFreeRotation(angle);
    }

    if (m_freeRotateSlider) {
        m_freeRotateSlider->blockSignals(true);
        m_freeRotateSlider->setValue(0);
        m_freeRotateSlider->blockSignals(false);
    }
    if (m_freeRotateValLabel) m_freeRotateValLabel->setText("0\xC2\xB0");
    if (m_applyFreeRotateBtn) m_applyFreeRotateBtn->setEnabled(false);
}

void MainWindow::onToggleFreeCrop(bool checked)
{
    if (checked) {
        // Deactivate region selection tool to avoid conflicts
        if (m_selectionToggle && m_selectionToggle->isChecked()) {
            m_selectionToggle->setChecked(false);
        }
        if (!m_cropTool) {
            m_cropTool = new CropTool(m_processedView->graphicsView(), m_processedView->scene(), this);
            m_processedView->setCropTool(m_cropTool);
        }
        if (m_service->hasImage()) {
            m_processedView->fitToView();
            QRectF bounds(0, 0, m_service->imageWidth(), m_service->imageHeight());
            m_cropTool->initCrop(bounds);
        }
        if (m_freeCropToggle) {
            m_freeCropToggle->setStyleSheet(
                "QPushButton { background: #052e16; color: #4ade80; border: 2px solid #22c55e; border-radius: 6px; padding: 5px 12px; font-weight: 600; font-size: 11px; }"
                "QPushButton:hover { background: #14532d; border-color: #4ade80; }"
            );
        }
        if (m_applyFreeCropBtn) m_applyFreeCropBtn->setEnabled(true);
    } else {
        onCancelFreeCrop();
    }
}

void MainWindow::onApplyFreeCrop()
{
    if (!m_cropTool || !m_cropTool->isActive() || !m_service->hasImage()) return;
    QRectF r = m_cropTool->cropRect();
    int x = qMax(0, qRound(r.x()));
    int y = qMax(0, qRound(r.y()));
    int w = qRound(r.width());
    int h = qRound(r.height());
    if (x + w > m_service->imageWidth())  w = m_service->imageWidth() - x;
    if (y + h > m_service->imageHeight()) h = m_service->imageHeight() - y;

    if (w > 0 && h > 0) {
        m_service->applyCrop(x, y, w, h);
    }
    onCancelFreeCrop();
}

void MainWindow::onCancelFreeCrop()
{
    if (m_cropTool) {
        m_cropTool->reset();
        m_cropTool->setActive(false);
    }
    if (m_freeCropToggle) {
        m_freeCropToggle->blockSignals(true);
        m_freeCropToggle->setChecked(false);
        m_freeCropToggle->blockSignals(false);
        m_freeCropToggle->setStyleSheet(kSmallActionBtnStyle);
    }
    if (m_applyFreeCropBtn) m_applyFreeCropBtn->setEnabled(false);
}

void MainWindow::onOpenResizeDialog()
{
    if (!m_service->hasImage()) return;
    int curW = m_service->imageWidth();
    int curH = m_service->imageHeight();

    QDialog dlg(this);
    dlg.setWindowTitle("Resize Image");
    dlg.setFixedWidth(320);
    dlg.setStyleSheet("background: #0f1524; color: #cbd5e1; font-size: 12px;");

    auto* layout = new QVBoxLayout(&dlg);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(14);

    auto* titleLbl = new QLabel("Resize Image Dimensions", &dlg);
    titleLbl->setStyleSheet("font-size: 14px; font-weight: 700; color: #38bdf8;");
    layout->addWidget(titleLbl);

    auto* form = new QFormLayout();
    form->setSpacing(10);

    auto* widthSpin = new QSpinBox(&dlg);
    widthSpin->setRange(1, 10000);
    widthSpin->setValue(curW);
    widthSpin->setFixedHeight(32);
    widthSpin->setStyleSheet("background: #141c2e; color: #f1f5f9; border: 1px solid #243049; border-radius: 6px; padding: 4px;");

    auto* heightSpin = new QSpinBox(&dlg);
    heightSpin->setRange(1, 10000);
    heightSpin->setValue(curH);
    heightSpin->setFixedHeight(32);
    heightSpin->setStyleSheet("background: #141c2e; color: #f1f5f9; border: 1px solid #243049; border-radius: 6px; padding: 4px;");

    auto* keepAspect = new QCheckBox("Keep Aspect Ratio", &dlg);
    keepAspect->setChecked(true);
    keepAspect->setStyleSheet("color: #94a3b8; font-size: 11px;");

    const double aspect = (curH > 0) ? (static_cast<double>(curW) / curH) : 1.0;

    connect(widthSpin, QOverload<int>::of(&QSpinBox::valueChanged), [&dlg, heightSpin, keepAspect, aspect](int w) {
        if (keepAspect->isChecked() && aspect > 0) {
            heightSpin->blockSignals(true);
            heightSpin->setValue(qMax(1, qRound(w / aspect)));
            heightSpin->blockSignals(false);
        }
    });

    connect(heightSpin, QOverload<int>::of(&QSpinBox::valueChanged), [&dlg, widthSpin, keepAspect, aspect](int h) {
        if (keepAspect->isChecked() && aspect > 0) {
            widthSpin->blockSignals(true);
            widthSpin->setValue(qMax(1, qRound(h * aspect)));
            widthSpin->blockSignals(false);
        }
    });

    form->addRow("Width (px):", widthSpin);
    form->addRow("Height (px):", heightSpin);
    form->addRow("", keepAspect);
    layout->addLayout(form);

    auto* btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    btnBox->button(QDialogButtonBox::Ok)->setText("Apply");
    btnBox->button(QDialogButtonBox::Ok)->setStyleSheet(kApplyBtnStyle);
    btnBox->button(QDialogButtonBox::Cancel)->setStyleSheet(kSmallActionBtnStyle);
    connect(btnBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    layout->addWidget(btnBox);

    if (dlg.exec() == QDialog::Accepted) {
        int newW = widthSpin->value();
        int newH = heightSpin->value();
        m_service->applyResize(newW, newH);
    }
}

void MainWindow::handleWorkspaceImageDropped(const QString& path)
{
    QImage img(path);
    if (!img.isNull()) {
        m_service->loadImage(path);
        if (m_assetExplorer) m_assetExplorer->addImage(path, img);
    }
}

void MainWindow::resetAdjustmentSliders()
{
    auto resetOne = [](QSlider* s, QLabel* l, QPushButton* b, const QString& def) {
        if (s) {
            s->blockSignals(true);
            s->setValue(0);
            s->blockSignals(false);
        }
        if (l) l->setText(def);
        if (b) b->setEnabled(false);
    };
    resetOne(m_brightnessSlider, m_brightnessValLabel, m_applyBrightnessBtn, "0%");
    resetOne(m_contrastSlider,   m_contrastValLabel,   m_applyContrastBtn,   "0%");
    resetOne(m_saturationSlider, m_saturationValLabel, m_applySaturationBtn, "0%");
    resetOne(m_hueSlider,        m_hueValLabel,        m_applyHueBtn,        "0\xC2\xB0");
    resetOne(m_freeRotateSlider, m_freeRotateValLabel, m_applyFreeRotateBtn, "0\xC2\xB0");
}

// ---------------------------------------------------------------------------
// Service Event Handlers
// ---------------------------------------------------------------------------
void MainWindow::onImageLoaded()
{
    QImage orig = m_service->originalQImage();
    m_originalView->setImage(orig);

    // Initial state: both panels display the loaded image cleanly
    m_processedView->setImage(orig);
    m_processedView->setActiveGlow(true);

    clearAllStickers();
    m_target->focusBaseImage();
    onCancelFreeCrop();

    // Update selection tool's source image for magic wand
    if (m_selectionTool)
        m_selectionTool->setSourceImage(orig);

    if (m_quickFiltersPanel) m_quickFiltersPanel->updateThumbnails(m_service);
    if (m_filtersTabPanel)   m_filtersTabPanel->updateThumbnails(m_service);
    if (m_effectsTabPanel)   m_effectsTabPanel->updateThumbnails(m_service);
    if (m_toolsTabPanel)     m_toolsTabPanel->updateThumbnails(m_service);

    m_statusBar->setImageInfo(m_service->imageWidth(), m_service->imageHeight(),
                              m_service->imageFormat());
    m_topBar->setSaveEnabled(true);
    m_topBar->setClearEnabled(true);

    resetAdjustmentSliders();
}

void MainWindow::onImageProcessed()
{
    QImage processed = m_service->processedQImage();
    if (processed.isNull()) {
        if (m_service->hasImage()) {
            m_processedView->setImage(m_service->originalQImage());
            m_processedView->setActiveGlow(m_target->isBaseImage());
        } else {
            m_processedView->clearImage();
            m_processedView->setActiveGlow(false);
        }
    } else {
        m_processedView->setImage(processed);
        m_processedView->setActiveGlow(m_target->isBaseImage());
    }

    if (m_selectionTool)
        m_selectionTool->setSourceImage(m_service->currentCommittedQImage());

    m_statusBar->setImageInfo(m_service->imageWidth(), m_service->imageHeight(),
                              m_service->imageFormat());
}

void MainWindow::onHistoryChanged()
{
    if (m_target->isBaseImage()) {
        m_topBar->setUndoEnabled(m_service->canUndo());
        m_topBar->setRedoEnabled(m_service->canRedo());
    }
    m_topBar->setClearEnabled(m_service->hasImage());
    m_topBar->setSaveEnabled(m_service->hasImage());
}

void MainWindow::onError(const QString& message)
{
    QMessageBox::warning(this, "GYYM Studio", message);
}

// ---------------------------------------------------------------------------
// Asset Explorer Activation
// ---------------------------------------------------------------------------
void MainWindow::onImageActivated(const QString& filePath)
{
    m_service->loadImage(filePath);
}

void MainWindow::onStickerActivated(const QString& filePath)
{
    if (!m_service->hasImage()) {
        QMessageBox::information(this, "GYYM Studio",
            "Please load a base image first before placing a sticker.");
        return;
    }
    placeSticker(filePath);
}

// ---------------------------------------------------------------------------
// Sticker Management
// ---------------------------------------------------------------------------
void MainWindow::placeSticker(const QString& filePath)
{
    if (!m_service->hasImage()) return;

    QImage img(filePath);
    if (img.isNull()) {
        onError(QString("Cannot load sticker: %1").arg(filePath));
        return;
    }

    m_processedScene = m_processedView->scene();
    if (!m_processedScene) return;

    int maxW = qMax(1, m_service->imageWidth()  / 4);
    int maxH = qMax(1, m_service->imageHeight() / 4);
    if (img.width() > maxW || img.height() > maxH)
        img = img.scaled(maxW, maxH, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    auto* item = new StickerItem(img);
    item->setSourcePath(filePath);
    item->setPos(m_service->imageWidth()  / 2.0 - img.width()  / 2.0,
                 m_service->imageHeight() / 2.0 - img.height() / 2.0);
    item->setZValue(10 + m_stickers.size());

    // Hook up focus-on-click callback
    item->onClicked = [this](StickerItem* s) {
        m_target->focusSticker(s);
    };

    m_processedScene->addItem(item);
    m_stickers.append(item);

    m_target->focusSticker(item);
}

void MainWindow::clearAllStickers()
{
    if (m_processedScene) {
        for (auto* s : m_stickers) {
            m_processedScene->removeItem(s);
            delete s;
        }
    }
    m_stickers.clear();

    if (m_removeStickerBtn)   m_removeStickerBtn->setEnabled(false);
    if (m_moveUpStickerBtn)   m_moveUpStickerBtn->setEnabled(false);
    if (m_moveDownStickerBtn) m_moveDownStickerBtn->setEnabled(false);
    if (m_stickerStatusLabel) m_stickerStatusLabel->setText("No stickers");
}

void MainWindow::onRemoveSelectedSticker()
{
    if (!m_target->isSticker()) return;
    StickerItem* sticker = m_target->sticker();
    if (!sticker || !m_processedScene) return;

    m_processedScene->removeItem(sticker);
    m_stickers.removeAll(sticker);
    m_target->stickerRemoved(sticker);
    delete sticker;

    if (!m_stickers.isEmpty()) {
        m_target->focusSticker(m_stickers.last());
    } else {
        m_target->focusBaseImage();
    }
}

void MainWindow::onMoveUpSticker()
{
    if (!m_target->isSticker()) return;
    StickerItem* cur = m_target->sticker();
    int idx = m_stickers.indexOf(cur);
    if (idx < 0 || idx >= m_stickers.size() - 1) return;

    cur->pushSnapshot();
    m_stickers[idx + 1]->pushSnapshot();

    m_stickers.swapItemsAt(idx, idx + 1);
    for (int i = 0; i < m_stickers.size(); ++i) {
        m_stickers[i]->setZValue(10 + i);
    }
    updateStickerLayerButtons();
}

void MainWindow::onMoveDownSticker()
{
    if (!m_target->isSticker()) return;
    StickerItem* cur = m_target->sticker();
    int idx = m_stickers.indexOf(cur);
    if (idx <= 0) return;

    cur->pushSnapshot();
    m_stickers[idx - 1]->pushSnapshot();

    m_stickers.swapItemsAt(idx, idx - 1);
    for (int i = 0; i < m_stickers.size(); ++i) {
        m_stickers[i]->setZValue(10 + i);
    }
    updateStickerLayerButtons();
}

void MainWindow::updateStickerLayerButtons()
{
    if (!m_target->isSticker()) {
        if (m_moveUpStickerBtn)   m_moveUpStickerBtn->setEnabled(false);
        if (m_moveDownStickerBtn) m_moveDownStickerBtn->setEnabled(false);
        return;
    }
    int idx = m_stickers.indexOf(m_target->sticker());
    if (m_moveUpStickerBtn)   m_moveUpStickerBtn->setEnabled(idx >= 0 && idx < m_stickers.size() - 1);
    if (m_moveDownStickerBtn) m_moveDownStickerBtn->setEnabled(idx > 0);
}

void MainWindow::onApplyFilterToSticker(int filterIndex)
{
    if (!m_target->isSticker()) return;
    StickerItem* sticker = m_target->sticker();

    sticker->pushSnapshot();
    QImage filtered = m_service->applyFilterToQImage(filterIndex, sticker->currentImage());
    if (!filtered.isNull()) {
        sticker->applyFilter(filtered);
    }
    m_topBar->setUndoEnabled(sticker->canUndo());
    m_topBar->setRedoEnabled(sticker->canRedo());
}

// ---------------------------------------------------------------------------
// Region Filtering
// ---------------------------------------------------------------------------
void MainWindow::onToggleSelectionTool(bool checked)
{
    if (m_selectionToggle) {
        if (checked) {
            onCancelFreeCrop();
            m_selectionToggle->setStyleSheet(
                "QPushButton { background: #052e16; color: #4ade80; border: 2px solid #22c55e; border-radius: 6px; padding: 5px 12px; font-weight: 600; font-size: 11px; }"
                "QPushButton:hover { background: #14532d; border-color: #4ade80; }"
            );
        } else {
            m_selectionToggle->setStyleSheet(kSmallActionBtnStyle);
        }
    }

    if (checked && !m_selectionTool) {
        QGraphicsView*  gv = m_processedView->graphicsView();
        QGraphicsScene* gs = m_processedView->scene();
        if (gv && gs) {
            m_selectionTool = new SelectionTool(gv, gs, this);
            m_processedView->setSelectionTool(m_selectionTool);
        }
    }

    if (m_selectionTool) {
        m_selectionTool->setSourceImage(m_service->hasImage() ? m_service->currentCommittedQImage() : QImage());
        int shapeVal = m_selectionShapeCombo->currentData().toInt();
        m_selectionTool->setShape(static_cast<SelectionTool::Shape>(shapeVal));
        if (m_toleranceSlider) {
            m_selectionTool->setTolerance(m_toleranceSlider->value());
        }
        m_selectionTool->setActive(checked);
    }
}

void MainWindow::onSelectionModeChanged(int /*index*/)
{
}

void MainWindow::onApplyFilterToRegion(int filterIndex)
{
    if (!m_selectionTool || !m_selectionTool->hasSelection()) return;
    if (!m_service->hasImage()) return;

    QImage baseImage = m_service->currentCommittedQImage();
    QImage filteredFull = m_service->applyFilterToQImage(filterIndex, baseImage);

    int modeVal = m_selectionModeCombo->currentData().toInt();
    auto mode   = static_cast<SelectionTool::Mode>(modeVal);
    QImage composited = m_selectionTool->compositeMasked(baseImage, filteredFull, mode);

    if (composited.isNull()) return;

    m_service->setProcessedFromQImage(composited);
}

// ---------------------------------------------------------------------------
// View Rotation
// ---------------------------------------------------------------------------
void MainWindow::onToggleRotateView(bool checked)
{
    m_rotateViewActive = checked;
    m_originalView->setRotateViewEnabled(checked);
    m_processedView->setRotateViewEnabled(checked);
    m_rotateViewBtn->setText(checked ? "\xF0\x9F\x94\x84  Exit Rotate" : "\xF0\x9F\x94\x84  Rotate View");
}

// ---------------------------------------------------------------------------
// Drag & Drop
// ---------------------------------------------------------------------------
void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        const auto urls = event->mimeData()->urls();
        for (const auto& url : urls) {
            const QString lp = url.toLocalFile().toLower();
            if (lp.endsWith(".png") || lp.endsWith(".jpg") || lp.endsWith(".jpeg")
                    || lp.endsWith(".bmp") || lp.endsWith(".tga")) {
                event->acceptProposedAction();
                return;
            }
        }
    }
    event->ignore();
}

void MainWindow::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
    else
        event->ignore();
}

void MainWindow::dropEvent(QDropEvent* event)
{
    const auto urls = event->mimeData()->urls();
    if (urls.isEmpty()) return;

    QString localPath = urls.first().toLocalFile();
    if (localPath.isEmpty()) return;

    // Validate file type
    const QString lower = localPath.toLower();
    if (!lower.endsWith(".png") && !lower.endsWith(".jpg") && !lower.endsWith(".jpeg")
            && !lower.endsWith(".bmp") && !lower.endsWith(".tga"))
        return;

    // Determine drop target: if drop is inside the asset explorer area, treat as asset
    QPoint globalPos = event->position().toPoint();
    QPoint localPos  = m_assetExplorer ? m_assetExplorer->mapFromGlobal(mapToGlobal(globalPos)) : QPoint(-1,-1);
    bool   dropOnAssets = m_assetExplorer && m_assetExplorer->rect().contains(localPos);

    QImage img(localPath);
    if (img.isNull()) return;

    if (dropOnAssets) {
        // Import as sticker asset
        m_assetExplorer->addSticker(localPath, img);
    } else {
        // Import as main image
        m_service->loadImage(localPath);
        m_assetExplorer->addImage(localPath, img);
    }

    event->acceptProposedAction();
}

// ---------------------------------------------------------------------------
// Zoom Panel Slots
// ---------------------------------------------------------------------------
void MainWindow::onZoomSliderChanged(int value)
{
    if (m_zoomUpdating) return;
    m_zoomUpdating = true;

    if (m_zoomPctLabel)
        m_zoomPctLabel->setText(QString("%1%").arg(value));

    if (m_originalView)  m_originalView->setZoom(static_cast<double>(value));
    if (m_processedView) m_processedView->setZoom(static_cast<double>(value));

    m_zoomUpdating = false;
}

void MainWindow::onViewZoomChanged(double percent)
{
    updateZoomSliderFromView(percent);
}

void MainWindow::updateZoomSliderFromView(double percent)
{
    if (m_zoomUpdating) return;
    m_zoomUpdating = true;

    int sliderVal = qBound(5, qRound(percent), 500);
    if (m_zoomSlider)   m_zoomSlider->setValue(sliderVal);
    if (m_zoomPctLabel) m_zoomPctLabel->setText(QString("%1%").arg(sliderVal));

    m_zoomUpdating = false;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
}
