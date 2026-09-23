#include "FilterCard.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>

FilterCard::FilterCard(const QString& name, const QString& iconColor, int index,
                       QWidget* parent)
    : QWidget(parent)
    , m_name(name)
    , m_iconColor(iconColor.isEmpty() ? "#6366f1" : iconColor)
    , m_index(index)
{
    setFixedSize(96, 92);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_Hover);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(4);
    layout->setAlignment(Qt::AlignCenter);

    // --- Icon / Thumbnail placeholder badge ---
    m_iconLabel = new QLabel(this);
    m_iconLabel->setFixedSize(80, 52);
    m_iconLabel->setAlignment(Qt::AlignCenter);

    QString initial = name.isEmpty() ? "?" : name.left(1).toUpper();
    m_iconLabel->setText(initial);
    layout->addWidget(m_iconLabel, 0, Qt::AlignCenter);

    // --- Name label ---
    m_nameLabel = new QLabel(name, this);
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setWordWrap(false);
    m_nameLabel->setMaximumWidth(88);
    layout->addWidget(m_nameLabel, 0, Qt::AlignCenter);

    updateStyle();
}

void FilterCard::setSelected(bool selected)
{
    if (m_selected == selected) return;
    m_selected = selected;
    updateStyle();
    update();
}

bool FilterCard::isSelected() const { return m_selected; }
int FilterCard::filterIndex() const { return m_index; }

void FilterCard::setThumbnail(const QPixmap& pixmap)
{
    m_thumbnail = pixmap;
    if (!m_thumbnail.isNull()) {
        QPixmap scaled = m_thumbnail.scaled(80, 52, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        // Crop to 80x52
        int x = (scaled.width() - 80) / 2;
        int y = (scaled.height() - 52) / 2;
        QPixmap cropped = scaled.copy(x, y, 80, 52);

        // Make rounded thumbnail
        QPixmap rounded(80, 52);
        rounded.fill(Qt::transparent);
        {
            QPainter p(&rounded);
            p.setRenderHint(QPainter::Antialiasing);
            QPainterPath path;
            path.addRoundedRect(0, 0, 80, 52, 6, 6);
            p.setClipPath(path);
            p.drawPixmap(0, 0, cropped);
        }

        m_iconLabel->setPixmap(rounded);
        m_iconLabel->setStyleSheet("background: transparent; border: none; border-radius: 6px;");
    } else {
        clearThumbnail();
    }
}

void FilterCard::clearThumbnail()
{
    m_thumbnail = QPixmap();
    m_iconLabel->clear();
    QString initial = m_name.isEmpty() ? "?" : m_name.left(1).toUpper();
    m_iconLabel->setText(initial);
    updateStyle();
}

void FilterCard::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked(m_index);
    }
    QWidget::mousePressEvent(event);
}

void FilterCard::enterEvent(QEnterEvent* event)
{
    m_hovered = true;
    updateStyle();
    update();
    QWidget::enterEvent(event);
}

void FilterCard::leaveEvent(QEvent* event)
{
    m_hovered = false;
    updateStyle();
    update();
    QWidget::leaveEvent(event);
}

void FilterCard::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QColor bg;
    QColor border;

    if (m_selected) {
        bg = QColor("#1e1b4b");
        border = QColor("#6366f1");
    } else if (m_hovered) {
        bg = QColor("#1f293d");
        border = QColor("#3b82f6");
    } else {
        bg = QColor("#141b2b");
        border = QColor("#222f46");
    }

    QPainterPath path;
    path.addRoundedRect(rect().adjusted(1, 1, -1, -1), 8, 8);

    p.fillPath(path, bg);

    QPen pen(border, m_selected ? 2.0 : 1.0);
    p.setPen(pen);
    p.drawPath(path);

    // Selected checkmark circle on top-right badge (exactly like reference image)
    if (m_selected) {
        p.setBrush(QColor("#6366f1"));
        p.setPen(Qt::NoPen);
        p.drawEllipse(QPointF(width() - 11, 11), 8, 8);

        p.setPen(QPen(Qt::white, 1.8));
        p.drawLine(QPointF(width() - 14, 11), QPointF(width() - 11, 14));
        p.drawLine(QPointF(width() - 11, 14), QPointF(width() - 8, 8));
    }
}

void FilterCard::updateStyle()
{
    if (m_thumbnail.isNull()) {
        QString iconStyle = QString(R"(
            QLabel {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                             stop:0 %1, stop:1 %2);
                border-radius: 6px;
                color: white;
                font-size: 16px;
                font-weight: bold;
            }
        )").arg(m_iconColor, QColor(m_iconColor).darker(160).name());
        m_iconLabel->setStyleSheet(iconStyle);
    }

    QString nameColor = m_selected ? "#ffffff" : (m_hovered ? "#f1f5f9" : "#94a3b8");
    m_nameLabel->setStyleSheet(QString(R"(
        QLabel {
            color: %1;
            font-size: 10px;
            font-weight: %2;
            background: transparent;
        }
    )").arg(nameColor, m_selected ? "700" : "500"));
}
