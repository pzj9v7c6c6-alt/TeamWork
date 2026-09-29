#include "ImageViewWidget.h"
#include "SelectionTool.h"
#include "CropTool.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QLabel>
#include <QPushButton>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QScrollBar>
#include <QResizeEvent>
#include <QTransform>
#include <QtMath>
#include <QStyle>

ImageViewWidget::ImageViewWidget(const QString& title, bool isFilteredPanel, QWidget* parent)
    : QWidget(parent)
    , m_isFilteredPanel(isFilteredPanel)
{
    setMinimumSize(240, 240);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 10, 12, 10);
    mainLayout->setSpacing(8);

    // --- Header row with badge ---
    auto* headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);

    m_titleLabel = new QLabel(title, this);
    // Title style will be set by theme stylesheet; keep object name for specificity
    m_titleLabel->setObjectName(isFilteredPanel ? "filteredBadge" : "originalBadge");
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch(1);
    mainLayout->addLayout(headerLayout);

    // --- Graphics View ---
    m_scene = new QGraphicsScene(this);
    m_view  = new QGraphicsView(m_scene, this);
    m_view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_view->setDragMode(QGraphicsView::ScrollHandDrag);
    // Use FullViewportUpdate to prevent ghost trail artifacts with overlays
    m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    m_view->setAcceptDrops(true);
    m_view->viewport()->setAcceptDrops(true);
    m_view->viewport()->installEventFilter(this);

    mainLayout->addWidget(m_view, 1);

    // --- Bottom row ---
    auto* bottomBar = new QHBoxLayout();
    bottomBar->setContentsMargins(2, 0, 2, 0);

    m_dimensionLabel = new QLabel(this);
    m_dimensionLabel->setObjectName("dimLabel");
    m_dimensionLabel->setVisible(false);

    m_zoomLabel = new QLabel(this);
    m_zoomLabel->setObjectName("zoomLabel");
    m_zoomLabel->setVisible(false);

    // Magnifying-glass zoom icon
    QPixmap zoomPix(20, 20);
    zoomPix.fill(Qt::transparent);
    {
        QPainter p(&zoomPix);
        p.setRenderHint(QPainter::Antialiasing);
        QPen pen(QColor("#94a3b8"), 2.0);
        p.setPen(pen);
        p.drawEllipse(3, 3, 9, 9);
        p.drawLine(10, 10, 16, 16);
    }

    m_quickZoomBtn = new QPushButton(this);
    m_quickZoomBtn->setIcon(QIcon(zoomPix));
    m_quickZoomBtn->setIconSize(QSize(18, 18));
    m_quickZoomBtn->setFixedSize(30, 26);
    m_quickZoomBtn->setCursor(Qt::PointingHandCursor);
    m_quickZoomBtn->setToolTip("Reset Zoom (100%)");
    m_quickZoomBtn->setStyleSheet(R"(
        QPushButton {
            background: #1e293b;
            border: 1px solid #334155;
            border-radius: 6px;
            padding: 0;
        }
        QPushButton:hover {
            background: #334155;
            border-color: #3b82f6;
        }
    )");
    connect(m_quickZoomBtn, &QPushButton::clicked, this, [this]() {
        setZoom(100.0);
    });

    bottomBar->addWidget(m_dimensionLabel);
    bottomBar->addStretch(1);
    bottomBar->addWidget(m_zoomLabel);
    bottomBar->addSpacing(4);
    bottomBar->addWidget(m_quickZoomBtn);

    mainLayout->addLayout(bottomBar);

    // --- Empty state overlay ---
    m_emptyOverlay = new QWidget(m_view);
    m_emptyOverlay->setStyleSheet("background: transparent;");

    auto* emptyLayout = new QVBoxLayout(m_emptyOverlay);
    emptyLayout->setAlignment(Qt::AlignCenter);
    emptyLayout->setSpacing(8);

    m_emptyIcon = new QLabel(m_emptyOverlay);
    m_emptyIcon->setAlignment(Qt::AlignCenter);
    m_emptyIcon->setText("\xF0\x9F\x96\xBC"); // 🖼

    m_emptyPrimary = new QLabel("No image loaded", m_emptyOverlay);
    m_emptyPrimary->setAlignment(Qt::AlignCenter);

    m_emptySecondary = new QLabel("Load an image to get started", m_emptyOverlay);
    m_emptySecondary->setAlignment(Qt::AlignCenter);

    emptyLayout->addWidget(m_emptyIcon);
    emptyLayout->addWidget(m_emptyPrimary);
    emptyLayout->addWidget(m_emptySecondary);

    showEmptyState(true);
}

// ---------------------------------------------------------------------------
void ImageViewWidget::setActiveGlow(bool active)
{
    if (m_hasActiveGlow == active) return;
    m_hasActiveGlow = active;
    // Glow is now handled through a simple border style change
    if (active) {
        setProperty("activeGlow", true);
    } else {
        setProperty("activeGlow", false);
    }
    style()->unpolish(this);
    style()->polish(this);
}

void ImageViewWidget::setImage(const QImage& image)
{
    m_currentImage = image;

    if (!m_currentImage.isNull()) {
        if (!m_pixmapItem) {
            m_pixmapItem = m_scene->addPixmap(QPixmap::fromImage(m_currentImage));
            m_pixmapItem->setZValue(0);
            fitToView();
        } else {
            m_pixmapItem->setPixmap(QPixmap::fromImage(m_currentImage));
        }
        m_scene->setSceneRect(m_pixmapItem->boundingRect());
        showEmptyState(false);
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

    QTransform t;
    t.rotate(m_viewRotation);
    t.scale(m_zoomLevel, m_zoomLevel);
    m_view->setTransform(t);
    m_view->centerOn(m_pixmapItem);

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

void ImageViewWidget::setRotateViewEnabled(bool enabled)
{
    m_rotateViewEnabled = enabled;
    if (!enabled) {
        m_viewRotation = 0.0;
        QTransform t;
        t.scale(m_zoomLevel, m_zoomLevel);
        m_view->setTransform(t);
    }
    m_view->setDragMode(enabled ? QGraphicsView::NoDrag : QGraphicsView::ScrollHandDrag);
}

bool ImageViewWidget::rotateViewEnabled() const
{
    return m_rotateViewEnabled;
}

void ImageViewWidget::setSelectionTool(SelectionTool* tool)
{
    m_selectionTool = tool;
}

void ImageViewWidget::setCropTool(CropTool* tool)
{
    m_cropTool = tool;
}

// ---------------------------------------------------------------------------
// Event filter — drag/drop + wheel zoom + rotate-view drag + crop tool + selection tool + focus clicks
// ---------------------------------------------------------------------------
bool ImageViewWidget::eventFilter(QObject* obj, QEvent* event)
{
    if (obj != m_view->viewport()) return QWidget::eventFilter(obj, event);

    // ── External drag & drop ─────────────────────────────────────────────
    if (event->type() == QEvent::DragEnter) {
        auto* de = static_cast<QDragEnterEvent*>(event);
        if (de->mimeData()->hasUrls()) {
            for (const auto& url : de->mimeData()->urls()) {
                const QString lp = url.toLocalFile().toLower();
                if (lp.endsWith(".png") || lp.endsWith(".jpg") || lp.endsWith(".jpeg")
                    || lp.endsWith(".bmp") || lp.endsWith(".tga")) {
                    de->acceptProposedAction();
                    return true;
                }
            }
        }
        de->ignore();
        return true;
    }
    if (event->type() == QEvent::DragMove) {
        auto* dme = static_cast<QDragMoveEvent*>(event);
        if (dme->mimeData()->hasUrls()) {
            dme->acceptProposedAction();
            return true;
        }
    }
    if (event->type() == QEvent::Drop) {
        auto* de = static_cast<QDropEvent*>(event);
        for (const auto& url : de->mimeData()->urls()) {
            const QString lp = url.toLocalFile();
            const QString lower = lp.toLower();
            if (lower.endsWith(".png") || lower.endsWith(".jpg") || lower.endsWith(".jpeg")
                || lower.endsWith(".bmp") || lower.endsWith(".tga")) {
                de->acceptProposedAction();
                emit fileDropped(lp);
                return true;
            }
        }
    }

    // ── Crop tool mouse events (highest priority when active) ────────────
    if (m_cropTool && m_cropTool->isActive() && m_pixmapItem) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto* me = static_cast<QMouseEvent*>(event);
            if (me->button() == Qt::LeftButton) {
                QPointF sp = m_view->mapToScene(me->pos());
                if (m_cropTool->handleMousePress(sp)) return true;
            }
        }
        if (event->type() == QEvent::MouseMove) {
            auto* me = static_cast<QMouseEvent*>(event);
            QPointF sp = m_view->mapToScene(me->pos());
            if (m_cropTool->handleMouseMove(sp)) return true;
        }
        if (event->type() == QEvent::MouseButtonRelease) {
            auto* me = static_cast<QMouseEvent*>(event);
            if (me->button() == Qt::LeftButton) {
                QPointF sp = m_view->mapToScene(me->pos());
                if (m_cropTool->handleMouseRelease(sp)) return true;
            }
        }
    }

    // ── Wheel → zoom centred on cursor ───────────────────────────────────
    if (event->type() == QEvent::Wheel && m_pixmapItem) {
        auto* we = static_cast<QWheelEvent*>(event);

        QPointF scenePt = m_view->mapToScene(we->position().toPoint());

        double factor = (we->angleDelta().y() > 0) ? 1.15 : (1.0 / 1.15);
        double newZoom = m_zoomLevel * factor;
        if (newZoom < 0.05) newZoom = 0.05;
        if (newZoom > 25.0) newZoom = 25.0;

        m_zoomLevel = newZoom;

        QTransform t;
        t.rotate(m_viewRotation);
        t.scale(m_zoomLevel, m_zoomLevel);
        m_view->setTransform(t);

        QPointF newViewPt = m_view->mapFromScene(scenePt);
        QPointF delta = we->position() - newViewPt;
        m_view->horizontalScrollBar()->setValue(
            m_view->horizontalScrollBar()->value() - static_cast<int>(delta.x()));
        m_view->verticalScrollBar()->setValue(
            m_view->verticalScrollBar()->value() - static_cast<int>(delta.y()));

        updateZoomLabel();
        emit zoomChanged(m_zoomLevel * 100.0);
        return true;
    }

    // ── Rotate view: Ctrl+drag (mouse press / move / release) ────────────
    if (m_rotateViewEnabled) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto* me = static_cast<QMouseEvent*>(event);
            if (me->button() == Qt::LeftButton) {
                m_rotDragging = true;
                m_rotLastPos  = me->pos();
                m_view->viewport()->setCursor(Qt::ClosedHandCursor);
                return true;
            }
        }
        if (event->type() == QEvent::MouseMove && m_rotDragging) {
            auto* me = static_cast<QMouseEvent*>(event);
            QPoint delta = me->pos() - m_rotLastPos;
            m_viewRotation += delta.x() * 0.5;
            m_rotLastPos = me->pos();

            QTransform t;
            t.rotate(m_viewRotation);
            t.scale(m_zoomLevel, m_zoomLevel);
            m_view->setTransform(t);
            return true;
        }
        if (event->type() == QEvent::MouseButtonRelease && m_rotDragging) {
            m_rotDragging = false;
            m_view->viewport()->setCursor(Qt::ArrowCursor);
            return true;
        }
    }

    // ── Selection tool mouse events ───────────────────────────────────────
    if (m_selectionTool && m_selectionTool->isActive() && m_pixmapItem) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto* me = static_cast<QMouseEvent*>(event);
            if (me->button() == Qt::LeftButton) {
                QPointF sp = m_view->mapToScene(me->pos());
                if (m_pixmapItem) sp = m_pixmapItem->mapFromScene(sp);
                m_selectionTool->handleMousePress(sp);
                return true;
            }
        }
        if (event->type() == QEvent::MouseMove) {
            auto* me = static_cast<QMouseEvent*>(event);
            if (me->buttons() & Qt::LeftButton) {
                QPointF sp = m_view->mapToScene(me->pos());
                if (m_pixmapItem) sp = m_pixmapItem->mapFromScene(sp);
                m_selectionTool->handleMouseMove(sp);
                return true;
            }
        }
        if (event->type() == QEvent::MouseButtonRelease) {
            auto* me = static_cast<QMouseEvent*>(event);
            if (me->button() == Qt::LeftButton) {
                QPointF sp = m_view->mapToScene(me->pos());
                if (m_pixmapItem) sp = m_pixmapItem->mapFromScene(sp);
                m_selectionTool->handleMouseRelease(sp);
                return true;
            }
        }
    }

    // ── Click-to-focus: if a left click on the filtered panel doesn't hit
    //    a sticker (sticker items handle their own press), emit baseImageClicked
    if (m_isFilteredPanel && event->type() == QEvent::MouseButtonPress && m_pixmapItem) {
        auto* me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::LeftButton) {
            QPointF sp = m_view->mapToScene(me->pos());
            QGraphicsItem* hit = m_scene->itemAt(sp, m_view->transform());
            // If the hit item is the base pixmap or nothing, signal base image focus
            if (!hit || hit == m_pixmapItem) {
                emit baseImageClicked();
            }
        }
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
