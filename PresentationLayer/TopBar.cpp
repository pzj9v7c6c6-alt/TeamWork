#include "TopBar.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>

namespace {

const char* kBrandingStyle = R"(
    QLabel#brandTitle {
        color: #f8fafc;
        font-size: 19px;
        font-weight: 700;
        letter-spacing: 0.5px;
    }
    QLabel#brandSubtitle {
        color: #8b9bb4;
        font-size: 11px;
        font-weight: 400;
    }
)";

const char* kActionBtnBase = R"(
    QPushButton {
        background: #141c2e;
        color: #cbd5e1;
        border: 1px solid #243049;
        border-radius: 8px;
        padding: 7px 16px;
        font-size: 12px;
        font-weight: 500;
        min-height: 18px;
    }
    QPushButton:hover {
        background: #1e293b;
        color: #f1f5f9;
        border-color: #3b82f6;
    }
    QPushButton:pressed {
        background: #243049;
    }
    QPushButton:disabled {
        background: #0b0f19;
        color: #475569;
        border-color: #1a2234;
    }
)";

// Blue pill for Load Image (matching reference image)
const char* kLoadBtnStyle = R"(
    QPushButton {
        background: #2563eb;
        color: #ffffff;
        border: none;
        border-radius: 8px;
        padding: 7px 18px;
        font-size: 12px;
        font-weight: 600;
        min-height: 18px;
    }
    QPushButton:hover {
        background: #3b82f6;
    }
    QPushButton:pressed {
        background: #1d4ed8;
    }
)";

// Purple gradient pill for Save (matching reference image)
const char* kSaveBtnStyle = R"(
    QPushButton {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                     stop:0 #6366f1, stop:1 #8b5cf6);
        color: #ffffff;
        border: none;
        border-radius: 8px;
        padding: 7px 18px;
        font-size: 12px;
        font-weight: 600;
        min-height: 18px;
    }
    QPushButton:hover {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                     stop:0 #818cf8, stop:1 #a78bfa);
    }
    QPushButton:pressed {
        background: #4f46e5;
    }
    QPushButton:disabled {
        background: #141c2e;
        color: #475569;
    }
)";

} // anon namespace

TopBar::TopBar(QWidget* parent)
    : QWidget(parent)
{
    setFixedHeight(64);
    setStyleSheet("TopBar { background: #0c101c; border-bottom: 1px solid #1a2336; }");

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(18, 0, 18, 0);
    layout->setSpacing(10);

    // --- Branding ---
    auto* brandLayout = new QVBoxLayout();
    brandLayout->setSpacing(1);
    brandLayout->setContentsMargins(0, 0, 0, 0);

    auto* iconLabel = new QLabel(this);
    iconLabel->setFixedSize(36, 36);
    iconLabel->setStyleSheet(R"(
        background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                     stop:0 #3b82f6, stop:1 #8b5cf6);
        border-radius: 8px;
        color: white;
        font-size: 18px;
        font-weight: bold;
    )");
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setText("\xF0\x9F\x96\xBC"); // 🖼

    auto* titleLabel = new QLabel("Image Studio", this);
    titleLabel->setObjectName("brandTitle");
    titleLabel->setStyleSheet(kBrandingStyle);

    auto* subtitleLabel = new QLabel("Edit  \xE2\x80\xA2  Enhance  \xE2\x80\xA2  Make it Yours", this);
    subtitleLabel->setObjectName("brandSubtitle");
    subtitleLabel->setStyleSheet(kBrandingStyle);

    brandLayout->addWidget(titleLabel);
    brandLayout->addWidget(subtitleLabel);

    auto* brandRow = new QHBoxLayout();
    brandRow->setSpacing(10);
    brandRow->addWidget(iconLabel);
    brandRow->addLayout(brandLayout);

    layout->addLayout(brandRow);
    layout->addStretch(1);

    // --- Action Buttons ---
    m_loadBtn = createActionButton("Load Image", "\xF0\x9F\x93\x82", "load");
    m_saveBtn = createActionButton("Save", "\xF0\x9F\x92\xBE", "save");
    m_undoBtn = createActionButton("Undo", "\xE2\x86\xA9", "default");
    m_redoBtn = createActionButton("Redo", "\xE2\x86\xAA", "default");
    m_clearBtn = createActionButton("Clear", "\xF0\x9F\x97\x91", "default");

    layout->addWidget(m_loadBtn);
    layout->addWidget(m_saveBtn);

    auto* sep1 = new QWidget(this);
    sep1->setFixedSize(1, 28);
    sep1->setStyleSheet("background: #243049;");
    layout->addWidget(sep1);

    layout->addWidget(m_undoBtn);
    layout->addWidget(m_redoBtn);

    auto* sep2 = new QWidget(this);
    sep2->setFixedSize(1, 28);
    sep2->setStyleSheet("background: #243049;");
    layout->addWidget(sep2);

    layout->addWidget(m_clearBtn);

    // Initial state
    m_saveBtn->setEnabled(false);
    m_undoBtn->setEnabled(false);
    m_redoBtn->setEnabled(false);
    m_clearBtn->setEnabled(false);

    // Connect signals
    connect(m_loadBtn, &QPushButton::clicked, this, &TopBar::loadClicked);
    connect(m_saveBtn, &QPushButton::clicked, this, &TopBar::saveClicked);
    connect(m_undoBtn, &QPushButton::clicked, this, &TopBar::undoClicked);
    connect(m_redoBtn, &QPushButton::clicked, this, &TopBar::redoClicked);
    connect(m_clearBtn, &QPushButton::clicked, this, &TopBar::clearClicked);
}

QPushButton* TopBar::createActionButton(const QString& text, const QString& iconChar,
                                         const QString& styleType)
{
    auto* btn = new QPushButton(this);
    btn->setText(QString("%1  %2").arg(iconChar, text));
    btn->setCursor(Qt::PointingHandCursor);

    if (styleType == "load")
        btn->setStyleSheet(kLoadBtnStyle);
    else if (styleType == "save")
        btn->setStyleSheet(kSaveBtnStyle);
    else
        btn->setStyleSheet(kActionBtnBase);

    return btn;
}

void TopBar::setUndoEnabled(bool enabled)  { m_undoBtn->setEnabled(enabled); }
void TopBar::setRedoEnabled(bool enabled)  { m_redoBtn->setEnabled(enabled); }
void TopBar::setSaveEnabled(bool enabled)  { m_saveBtn->setEnabled(enabled); }
void TopBar::setClearEnabled(bool enabled) { m_clearBtn->setEnabled(enabled); }
