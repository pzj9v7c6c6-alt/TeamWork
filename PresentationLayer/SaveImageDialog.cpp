#include "SaveImageDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>
#include <QGraphicsDropShadowEffect>

SaveImageDialog::SaveImageDialog(const QString& defaultPath,
                                 const QString& currentFormat,
                                 QWidget* parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(440, 420);

    setupUi(defaultPath, currentFormat);
}

void SaveImageDialog::setupUi(const QString& defaultPath, const QString& currentFormat)
{
    // Outer container with border and drop shadow
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(10, 10, 10, 10);

    auto* container = new QWidget(this);
    container->setStyleSheet(R"(
        QWidget {
            background-color: #151d30;
            border: 1px solid #2a3754;
            border-radius: 12px;
        }
        QLabel {
            color: #cbd5e1;
            font-size: 13px;
            font-weight: 500;
            background: transparent;
            border: none;
        }
        QLineEdit {
            background-color: #0d1322;
            color: #f1f5f9;
            border: 1px solid #243049;
            border-radius: 8px;
            padding: 8px 12px;
            font-size: 13px;
        }
        QLineEdit:focus {
            border: 1px solid #6366f1;
        }
        QComboBox {
            background-color: #0d1322;
            color: #f1f5f9;
            border: 1px solid #243049;
            border-radius: 8px;
            padding: 8px 12px;
            font-size: 13px;
        }
        QComboBox::drop-down {
            border: none;
            width: 24px;
        }
        QComboBox QAbstractItemView {
            background-color: #151d30;
            color: #f1f5f9;
            selection-background-color: #2563eb;
            border: 1px solid #2a3754;
            border-radius: 6px;
            padding: 4px;
        }
        QCheckBox {
            color: #cbd5e1;
            font-size: 13px;
            background: transparent;
            border: none;
            spacing: 8px;
        }
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border-radius: 4px;
            border: 1px solid #3b82f6;
            background: #0d1322;
        }
        QCheckBox::indicator:checked {
            background: #2563eb;
            image: none;
        }
    )");

    auto* shadow = new QGraphicsDropShadowEffect(container);
    shadow->setBlurRadius(25);
    shadow->setColor(QColor(0, 0, 0, 180));
    shadow->setOffset(0, 8);
    container->setGraphicsEffect(shadow);

    auto* layout = new QVBoxLayout(container);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(16);

    // --- Header ---
    auto* headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);

    auto* titleIcon = new QLabel("\xF0\x9F\x92\xBE", container); // 💾
    titleIcon->setStyleSheet("font-size: 18px; color: #a5b4fc;");

    auto* titleLabel = new QLabel("Save Image", container);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: 700; color: #f8fafc;");

    auto* closeBtn = new QPushButton("\xE2\x9C\x95", container); // ✕
    closeBtn->setFixedSize(28, 28);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            color: #64748b;
            border: none;
            font-size: 14px;
            border-radius: 6px;
        }
        QPushButton:hover {
            background: #1e293b;
            color: #e2e8f0;
        }
    )");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);

    headerLayout->addWidget(titleIcon);
    headerLayout->addSpacing(8);
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch(1);
    headerLayout->addWidget(closeBtn);

    layout->addLayout(headerLayout);

    // --- Fields ---
    // File name
    auto* nameLabel = new QLabel("File Name:", container);
    layout->addWidget(nameLabel);

    QString defaultBaseName = "edited_image";
    QString defaultExt = currentFormat.isEmpty() ? "png" : currentFormat.toLower();
    if (!defaultPath.isEmpty()) {
        QFileInfo fi(defaultPath);
        defaultBaseName = fi.completeBaseName() + "_filtered";
        if (!fi.suffix().isEmpty())
            defaultExt = fi.suffix().toLower();
    }

    m_fileNameEdit = new QLineEdit(defaultBaseName + "." + defaultExt, container);
    layout->addWidget(m_fileNameEdit);

    // Save Location
    auto* locLabel = new QLabel("Save Location:", container);
    layout->addWidget(locLabel);

    auto* locLayout = new QHBoxLayout();
    locLayout->setSpacing(8);

    QString defaultDir = defaultPath.isEmpty()
        ? QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
        : QFileInfo(defaultPath).absolutePath();

    m_locationEdit = new QLineEdit(defaultDir, container);
    m_locationEdit->setReadOnly(true);

    m_browseBtn = new QPushButton("\xF0\x9F\x93\x81", container); // 📁
    m_browseBtn->setFixedSize(38, 34);
    m_browseBtn->setCursor(Qt::PointingHandCursor);
    m_browseBtn->setStyleSheet(R"(
        QPushButton {
            background: #1e293b;
            color: #f1f5f9;
            border: 1px solid #334155;
            border-radius: 8px;
            font-size: 14px;
        }
        QPushButton:hover {
            background: #334155;
            border-color: #6366f1;
        }
    )");
    connect(m_browseBtn, &QPushButton::clicked, this, &SaveImageDialog::onBrowse);

    locLayout->addWidget(m_locationEdit, 1);
    locLayout->addWidget(m_browseBtn);
    layout->addLayout(locLayout);

    // Format
    auto* formatLabel = new QLabel("Format:", container);
    layout->addWidget(formatLabel);

    m_formatCombo = new QComboBox(container);
    m_formatCombo->addItem("PNG (*.png)", "png");
    m_formatCombo->addItem("JPEG (*.jpg)", "jpg");
    m_formatCombo->addItem("BMP (*.bmp)", "bmp");
    m_formatCombo->addItem("TGA (*.tga)", "tga");

    // Match extension
    for (int i = 0; i < m_formatCombo->count(); ++i) {
        if (m_formatCombo->itemData(i).toString().compare(defaultExt, Qt::CaseInsensitive) == 0) {
            m_formatCombo->setCurrentIndex(i);
            break;
        }
    }
    connect(m_formatCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SaveImageDialog::onFormatChanged);
    layout->addWidget(m_formatCombo);

    // Quality slider
    auto* qualityRow = new QHBoxLayout();
    auto* qHeader = new QLabel("Quality:", container);
    m_qualityLabel = new QLabel("100%", container);
    m_qualityLabel->setStyleSheet("color: #94a3b8; font-size: 12px;");
    qualityRow->addWidget(qHeader);
    qualityRow->addStretch(1);
    qualityRow->addWidget(m_qualityLabel);
    layout->addLayout(qualityRow);

    m_qualitySlider = new QSlider(Qt::Horizontal, container);
    m_qualitySlider->setRange(10, 100);
    m_qualitySlider->setValue(100);
    m_qualitySlider->setStyleSheet(R"(
        QSlider::groove:horizontal {
            height: 4px;
            background: #243049;
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
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                         stop:0 #3b82f6, stop:1 #8b5cf6);
            border-radius: 2px;
        }
    )");
    connect(m_qualitySlider, &QSlider::valueChanged, this, &SaveImageDialog::onQualityChanged);
    layout->addWidget(m_qualitySlider);

    // History checkbox
    m_historyCheckBox = new QCheckBox("Include Edit History", container);
    m_historyCheckBox->setChecked(true);
    layout->addWidget(m_historyCheckBox);

    layout->addSpacing(8);

    // --- Buttons ---
    auto* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);

    m_cancelBtn = new QPushButton("Cancel", container);
    m_cancelBtn->setCursor(Qt::PointingHandCursor);
    m_cancelBtn->setStyleSheet(R"(
        QPushButton {
            background: #1e293b;
            color: #cbd5e1;
            border: 1px solid #334155;
            border-radius: 8px;
            padding: 8px 20px;
            font-size: 13px;
            font-weight: 500;
            min-height: 20px;
        }
        QPushButton:hover {
            background: #334155;
            color: #f8fafc;
        }
    )");
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    m_saveBtn = new QPushButton("Save", container);
    m_saveBtn->setCursor(Qt::PointingHandCursor);
    m_saveBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                         stop:0 #3b82f6, stop:1 #8b5cf6);
            color: #ffffff;
            border: none;
            border-radius: 8px;
            padding: 8px 24px;
            font-size: 13px;
            font-weight: 600;
            min-height: 20px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                         stop:0 #60a5fa, stop:1 #a78bfa);
        }
        QPushButton:pressed {
            background: #2563eb;
        }
    )");
    connect(m_saveBtn, &QPushButton::clicked, this, &SaveImageDialog::onSaveClicked);

    btnLayout->addStretch(1);
    btnLayout->addWidget(m_cancelBtn);
    btnLayout->addWidget(m_saveBtn);

    layout->addLayout(btnLayout);

    rootLayout->addWidget(container);
}

void SaveImageDialog::onBrowse()
{
    QString dir = QFileDialog::getExistingDirectory(
        this, "Select Save Directory", m_locationEdit->text());
    if (!dir.isEmpty()) {
        m_locationEdit->setText(dir);
    }
}

void SaveImageDialog::onFormatChanged(int index)
{
    QString ext = m_formatCombo->itemData(index).toString();
    QString currentName = m_fileNameEdit->text();
    int dotIdx = currentName.lastIndexOf('.');
    if (dotIdx != -1) {
        currentName = currentName.left(dotIdx) + "." + ext;
    } else {
        currentName += "." + ext;
    }
    m_fileNameEdit->setText(currentName);
}

void SaveImageDialog::onQualityChanged(int value)
{
    m_qualityLabel->setText(QString("%1%").arg(value));
}

void SaveImageDialog::onSaveClicked()
{
    QString fileName = m_fileNameEdit->text().trimmed();
    if (fileName.isEmpty()) {
        fileName = "edited_image.png";
    }

    QString dir = m_locationEdit->text().trimmed();
    QDir targetDir(dir);
    m_finalPath = targetDir.filePath(fileName);

    accept();
}

QString SaveImageDialog::selectedFilePath() const
{
    return m_finalPath;
}

int SaveImageDialog::quality() const
{
    return m_qualitySlider->value();
}

bool SaveImageDialog::includeHistory() const
{
    return m_historyCheckBox->isChecked();
}
