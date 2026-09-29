#include "TopBar.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>

TopBar::TopBar(QWidget* parent)
    : QWidget(parent)
{
    setFixedHeight(64);
    setObjectName("TopBar");

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(18, 0, 18, 0);
    layout->setSpacing(10);

    // --- Branding: GYYM Studio ---
    auto* brandLayout = new QVBoxLayout();
    brandLayout->setSpacing(1);
    brandLayout->setContentsMargins(0, 0, 0, 0);

    auto* titleLabel = new QLabel("GYYM Studio", this);
    titleLabel->setObjectName("brandTitle");

    auto* subtitleLabel = new QLabel("Edit  \xE2\x80\xA2  Enhance  \xE2\x80\xA2  Make it Yours", this);
    subtitleLabel->setObjectName("brandSubtitle");

    brandLayout->addWidget(titleLabel);
    brandLayout->addWidget(subtitleLabel);

    layout->addLayout(brandLayout);
    layout->addStretch(1);


    auto makeBtn = [this](const QString& text) -> QPushButton* {
        auto* btn = new QPushButton(text, this);
        btn->setCursor(Qt::PointingHandCursor);
        return btn;
    };

    m_loadBtn  = makeBtn("\xF0\x9F\x93\x82  Load Image");
    m_saveBtn  = makeBtn("\xF0\x9F\x92\xBE  Save");
    m_undoBtn  = makeBtn("\xE2\x86\xA9  Undo");
    m_redoBtn  = makeBtn("\xE2\x86\xAA  Redo");
    m_clearBtn = makeBtn("\xF0\x9F\x97\x91  Clear");

    layout->addWidget(m_loadBtn);
    layout->addWidget(m_saveBtn);

    auto* sep1 = new QWidget(this);
    sep1->setFixedSize(1, 28);
    layout->addWidget(sep1);

    layout->addWidget(m_undoBtn);
    layout->addWidget(m_redoBtn);

    auto* sep2 = new QWidget(this);
    sep2->setFixedSize(1, 28);
    layout->addWidget(sep2);

    layout->addWidget(m_clearBtn);

    // Initial state
    m_saveBtn->setEnabled(false);
    m_undoBtn->setEnabled(false);
    m_redoBtn->setEnabled(false);
    m_clearBtn->setEnabled(false);

    // Connect signals
    connect(m_loadBtn,  &QPushButton::clicked, this, &TopBar::loadClicked);
    connect(m_saveBtn,  &QPushButton::clicked, this, &TopBar::saveClicked);
    connect(m_undoBtn,  &QPushButton::clicked, this, &TopBar::undoClicked);
    connect(m_redoBtn,  &QPushButton::clicked, this, &TopBar::redoClicked);
    connect(m_clearBtn, &QPushButton::clicked, this, &TopBar::clearClicked);
}

QPushButton* TopBar::createActionButton(const QString& text, const QString& iconChar,
                                         const QString& /*styleType*/)
{
    auto* btn = new QPushButton(this);
    btn->setText(QString("%1  %2").arg(iconChar, text));
    btn->setCursor(Qt::PointingHandCursor);
    return btn;
}

void TopBar::setUndoEnabled(bool enabled)  { m_undoBtn->setEnabled(enabled); }
void TopBar::setRedoEnabled(bool enabled)  { m_redoBtn->setEnabled(enabled); }
void TopBar::setSaveEnabled(bool enabled)  { m_saveBtn->setEnabled(enabled); }
void TopBar::setClearEnabled(bool enabled) { m_clearBtn->setEnabled(enabled); }
