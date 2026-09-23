#include "StatusBar.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QPushButton>

namespace {

const char* kBarStyle = R"(
    StatusBar {
        background: #0f172a;
        border-top: 1px solid #1e293b;
    }
)";

const char* kSmallBtnStyle = R"(
    QPushButton {
        background: #1e293b;
        color: #94a3b8;
        border: 1px solid #334155;
        border-radius: 4px;
        padding: 2px 8px;
        font-size: 14px;
        min-width: 24px;
        min-height: 24px;
    }
    QPushButton:hover {
        background: #334155;
        color: #e2e8f0;
    }
    QPushButton:pressed {
        background: #475569;
    }
)";

const char* kSliderStyle = R"(
    QSlider::groove:horizontal {
        height: 4px;
        background: #1e293b;
        border-radius: 2px;
    }
    QSlider::handle:horizontal {
        background: #6366f1;
        width: 14px;
        height: 14px;
        margin: -5px 0;
        border-radius: 7px;
    }
    QSlider::handle:horizontal:hover {
        background: #818cf8;
    }
    QSlider::sub-page:horizontal {
        background: #6366f1;
        border-radius: 2px;
    }
)";

} // anon namespace

StatusBar::StatusBar(QWidget* parent)
    : QWidget(parent)
{
    setFixedHeight(36);
    setStyleSheet(kBarStyle);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(16, 0, 16, 0);
    layout->setSpacing(8);

    // --- Image info ---
    m_infoLabel = new QLabel(this);
    m_infoLabel->setStyleSheet("color: #64748b; font-size: 12px;");
    layout->addWidget(m_infoLabel);

    layout->addStretch(1);

    // --- Zoom controls ---
    m_zoomLabel = new QLabel("100%", this);
    m_zoomLabel->setStyleSheet("color: #94a3b8; font-size: 12px; min-width: 42px;");
    m_zoomLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(m_zoomLabel);

    m_zoomOutBtn = new QPushButton("\xE2\x80\x93", this); // –
    m_zoomOutBtn->setStyleSheet(kSmallBtnStyle);
    m_zoomOutBtn->setCursor(Qt::PointingHandCursor);
    m_zoomOutBtn->setToolTip("Zoom Out");
    layout->addWidget(m_zoomOutBtn);

    m_zoomSlider = new QSlider(Qt::Horizontal, this);
    m_zoomSlider->setRange(5, 500);
    m_zoomSlider->setValue(100);
    m_zoomSlider->setFixedWidth(120);
    m_zoomSlider->setStyleSheet(kSliderStyle);
    layout->addWidget(m_zoomSlider);

    m_zoomInBtn = new QPushButton("+", this);
    m_zoomInBtn->setStyleSheet(kSmallBtnStyle);
    m_zoomInBtn->setCursor(Qt::PointingHandCursor);
    m_zoomInBtn->setToolTip("Zoom In");
    layout->addWidget(m_zoomInBtn);

    m_fitBtn = new QPushButton("\xE2\x9B\xB6", this); // ⛶ expand
    m_fitBtn->setStyleSheet(kSmallBtnStyle);
    m_fitBtn->setCursor(Qt::PointingHandCursor);
    m_fitBtn->setToolTip("Fit to View");
    m_fitBtn->setText("Fit");
    layout->addWidget(m_fitBtn);

    // Connect
    connect(m_zoomInBtn, &QPushButton::clicked, this, &StatusBar::zoomInClicked);
    connect(m_zoomOutBtn, &QPushButton::clicked, this, &StatusBar::zoomOutClicked);
    connect(m_fitBtn, &QPushButton::clicked, this, &StatusBar::fitToViewClicked);
    connect(m_zoomSlider, &QSlider::valueChanged, this, [this](int val) {
        m_zoomLabel->setText(QString("%1%").arg(val));
        emit zoomValueChanged(static_cast<double>(val));
    });
}

void StatusBar::setImageInfo(int width, int height, const QString& format)
{
    QString info = QString("\xF0\x9F\x96\xBC  Image: %1 \xC3\x97 %2")
                       .arg(width).arg(height);
    if (!format.isEmpty())
        info += QString("  |  %1").arg(format);
    m_infoLabel->setText(info);
}

void StatusBar::setZoomPercent(double percent)
{
    int val = qRound(percent);
    m_zoomSlider->blockSignals(true);
    m_zoomSlider->setValue(val);
    m_zoomSlider->blockSignals(false);
    m_zoomLabel->setText(QString("%1%").arg(val));
}

void StatusBar::clearInfo()
{
    m_infoLabel->clear();
    m_zoomLabel->setText("100%");
    m_zoomSlider->blockSignals(true);
    m_zoomSlider->setValue(100);
    m_zoomSlider->blockSignals(false);
}
