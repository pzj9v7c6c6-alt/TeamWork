#include "ImageViewWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QLabel>
#include <QPushButton>
#include <QWheelEvent>
#include <QScrollBar>
#include <QResizeEvent>

ImageViewWidget::ImageViewWidget(const QString& title, bool isFilteredPanel, QWidget* parent)
    : QWidget(parent)
    , m_isFilteredPanel(isFilteredPanel)
{
    setMinimumSize(240, 240);
    updatePanelStyle();

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 10, 12, 10);
    mainLayout->setSpacing(8);

    // --- Header row with badge ---
    auto* headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);

    m_titleLabel = new QLabel(title, this);
    if (isFilteredPanel) {
        m_titleLabel->setStyleSheet(R"(
            QLabel {
                background: rgba(99, 102, 241, 0.18);
                color: #c7d2fe;
                border: 1px solid rgba(99, 102, 241, 0.4);
                border-radius: 6px;
                padding: 4px 12px;
                font-size: 11px;
                font-weight: 600;
            }
        )");
    } else {
        m_titleLabel->setStyleSheet(R"(
            QLabel {
                background: rgba(255, 255, 255, 0.07);
                color: #cbd5e1;
                border: 1px solid #2a3754;
                border-radius: 6px;
                padding: 4px 12px;
                font-size: 11px;
                font-weight: 600;
            }
        )");
    }
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch(1);
    mainLayout->addLayout(headerLayout);

    // --- Graphics View ---
    m_scene = new QGraphicsScene(this);
    m_view = new QGraphicsView(m_scene, this);
    m_view->setStyleSheet(R"(
        QGraphicsView {
            background: #090d16;
            border: none;
            border-radius: 8px;
        }
        QScrollBar:vertical {
            background: #090d16; width: 6px; border-radius: 3px;
        }
        QScrollBar::handle:vertical {
            background: #1e293b; border-radius: 3px; min-height: 24px;
        }
        QScrollBar:horizontal {
            background: #090d16; height: 6px; border-radius: 3px;
        }
        QScrollBar::handle:horizontal {
            background: #1e293b; border-radius: 3px; min-width: 24px;
        }
        QScrollBar::add-line, QScrollBar::sub-line,
        QScrollBar::add-page, QScrollBar::sub-page {
            background: none; border: none;
        }
    )");
    m_view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_view->setDragMode(QGraphicsView::ScrollHandDrag);
    m_view->viewport()->installEventFilter(this);

    mainLayout->addWidget(m_view, 1);

    // --- Bottom row: dimension pill + quick tools ---
    auto* bottomBar = new QHBoxLayout();
    bottomBar->setContentsMargins(2, 0, 2, 0);

    m_dimensionLabel = new QLabel(this);
    m_dimensionLabel->setStyleSheet(R"(
        QLabel {
            background: rgba(15, 23, 42, 0.9);
            color: #94a3b8;
            border: 1px solid #1e293b;
            border-radius: 4px;
            padding: 3px 8px;
            font-size: 11px;
            font-weight: 500;
        }
    )");
    m_dimensionLabel->setVisible(false);

    m_zoomLabel = new QLabel(this);
    m_zoomLabel->setStyleSheet("color: #64748b; font-size: 11px; padding: 2px 6px;");
    m_zoomLabel->setVisible(false);

    // Quick icon buttons (Search 🔍 and Fit ⛶)
    m_quickZoomBtn = new QPushButton("\xF0\x9F\x94\x8D", this); // 🔍
    m_quickZoomBtn->setFixedSize(26, 26);
    m_quickZoomBtn->setCursor(Qt::PointingHandCursor);
    m_quickZoomBtn->setToolTip("Reset Zoom (100%)");
    m_quickZoomBtn->setStyleSheet(R"(
        QPushButton {
            background: #141c2e;
            color: #94a3b8;
            border: 1px solid #243049;
            border-radius: 4px;
            font-size: 11px;
        }
        QPushButton:hover {
            background: #1e293b;
            color: #f1f5f9;
        }
    )");
    connect(m_quickZoomBtn, &QPushButton::clicked, this, [this]() {
        setZoom(100.0);
    });

    m_quickFitBtn = new QPushButton("\xE2\x9B\xB6", this); // ⛶
    m_quickFitBtn->setFixedSize(26, 26);
    m_quickFitBtn->setCursor(Qt::PointingHandCursor);
    m_quickFitBtn->setToolTip("Fit to View");
    m_quickFitBtn->setStyleSheet(R"(
        QPushButton {
            background: #141c2e;
            color: #94a3b8;
            border: 1px solid #243049;
            border-radius: 4px;
            font-size: 11px;
        }
        QPushButton:hover {
            background: #1e293b;
            color: #f1f5f9;
        }
    )");
    connect(m_quickFitBtn, &QPushButton::clicked, this, &ImageViewWidget::fitToView);

    bottomBar->addWidget(m_dimensionLabel);
    bottomBar->addStretch(1);
    bottomBar->addWidget(m_zoomLabel);
    bottomBar->addSpacing(4);
    bottomBar->addWidget(m_quickZoomBtn);
    bottomBar->addWidget(m_quickFitBtn);

    mainLayout->addLayout(bottomBar);

    // --- Empty state overlay ---
    m_emptyOverlay = new QWidget(m_view);
    m_emptyOverlay->setStyleSheet("background: transparent;");

    auto* emptyLayout = new QVBoxLayout(m_emptyOverlay);
    emptyLayout->setAlignment(Qt::AlignCenter);
    emptyLayout->setSpacing(8);

    m_emptyIcon = new QLabel(m_emptyOverlay);
    m_emptyIcon->setAlignment(Qt::AlignCenter);
    m_emptyIcon->setStyleSheet("color: #243049; font-size: 44px;");
    m_emptyIcon->setText("\xF0\x9F\x96\xBC"); // 🖼

    m_emptyPrimary = new QLabel("No image loaded", m_emptyOverlay);
    m_emptyPrimary->setAlignment(Qt::AlignCenter);
    m_emptyPrimary->setStyleSheet("color: #64748b; font-size: 13px; font-weight: 600;");

    m_emptySecondary = new QLabel("Load an image to get started", m_emptyOverlay);
    m_emptySecondary->setAlignment(Qt::AlignCenter);
    m_emptySecondary->setStyleSheet("color: #475569; font-size: 11px;");

    emptyLayout->addWidget(m_emptyIcon);
    emptyLayout->addWidget(m_emptyPrimary);
    emptyLayout->addWidget(m_emptySecondary);

    showEmptyState(true);
}

void ImageViewWidget::updatePanelStyle()
{
    if (m_hasActiveGlow) {
        setStyleSheet(R"(
            ImageViewWidget {
                background: #101625;
                border: 1.5px solid #6366f1;
                border-radius: 12px;
            }
        )");
    } else {
        setStyleSheet(R"(
            ImageViewWidget {
                background: #101625;
                border: 1px solid #1a2336;
                border-radius: 12px;
            }
        )");
    }
}

void ImageViewWidget::setActiveGlow(bool active)
{
    if (m_hasActiveGlow == active) return;
    m_hasActiveGlow = active;
    updatePanelStyle();
}

void ImageViewWidget::setImage(const QImage& image)
{
    m_currentImage = image;

    if (m_pixmapItem) {
        m_scene->removeItem(m_pixmapItem);
        delete m_pixmapItem;
        m_pixmapItem = nullptr;
    }

    if (!m_currentImage.isNull()) {
        m_pixmapItem = m_scene->addPixmap(QPixmap::fromImage(m_currentImage));
        m_scene->setSceneRect(m_pixmapItem->boundingRect());
        showEmptyState(false);
        fitToView();
        updateDimensionLabel();
    } else {
        clearImage();
    }
}

void ImageViewWidget::clearImage()
{
    m_currentImage = QImage();
    if (m_pixmapItem) {
        m_scene->removeItem(m_pixmapItem);
        delete m_pixmapItem;
        m_pixmapItem = nullptr;
    }
    m_scene->setSceneRect(QRectF());
    m_dimensionLabel->setVisible(false);
    m_zoomLabel->setVisible(false);
    setActiveGlow(false);
    showEmptyState(true);
}

void ImageViewWidget::fitToView()
{
    if (!m_pixmapItem) return;
    m_view->fitInView(m_pixmapItem, Qt::KeepAspectRatio);

    QTransform t = m_view->transform();
    m_zoomLevel = t.m11();
    updateZoomLabel();
}

double ImageViewWidget::zoomPercent() const
{
    return m_zoomLevel * 100.0;
}

void ImageViewWidget::setZoom(double percent)
{
    if (!m_pixmapItem) return;
    m_zoomLevel = percent / 100.0;
    m_view->resetTransform();
    m_view->scale(m_zoomLevel, m_zoomLevel);
    updateZoomLabel();
    emit zoomChanged(percent);
}

void ImageViewWidget::zoomIn()
{
    setZoom(zoomPercent() * 1.2);
}

void ImageViewWidget::zoomOut()
{
    setZoom(zoomPercent() / 1.2);
}

void ImageViewWidget::setEmptyStateText(const QString& primary, const QString& secondary)
{
    m_emptyPrimary->setText(primary);
    if (!secondary.isEmpty())
        m_emptySecondary->setText(secondary);
}

bool ImageViewWidget::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_view->viewport() && event->type() == QEvent::Wheel && m_pixmapItem) {
        auto* we = static_cast<QWheelEvent*>(event);
        double factor = (we->angleDelta().y() > 0) ? 1.15 : (1.0 / 1.15);
        double newZoom = m_zoomLevel * factor;

        if (newZoom < 0.05) newZoom = 0.05;
        if (newZoom > 25.0) newZoom = 25.0;

        m_zoomLevel = newZoom;
        m_view->resetTransform();
        m_view->scale(m_zoomLevel, m_zoomLevel);
        updateZoomLabel();
        emit zoomChanged(m_zoomLevel * 100.0);
        return true;
    }
    return QWidget::eventFilter(obj, event);
}

void ImageViewWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (m_emptyOverlay && m_emptyOverlay->isVisible()) {
        m_emptyOverlay->setGeometry(m_view->rect());
    } else if (m_pixmapItem) {
        fitToView();
    }
}

void ImageViewWidget::updateDimensionLabel()
{
    if (m_currentImage.isNull()) {
        m_dimensionLabel->setVisible(false);
        return;
    }
    m_dimensionLabel->setText(
        QString("%1 \xC3\x97 %2").arg(m_currentImage.width()).arg(m_currentImage.height()));
    m_dimensionLabel->setVisible(true);
}

void ImageViewWidget::updateZoomLabel()
{
    m_zoomLabel->setText(QString("%1%").arg(qRound(m_zoomLevel * 100)));
    m_zoomLabel->setVisible(true);
}

void ImageViewWidget::showEmptyState(bool show)
{
    m_emptyOverlay->setVisible(show);
    if (show) {
        m_emptyOverlay->setGeometry(m_view->rect());
    }
}
