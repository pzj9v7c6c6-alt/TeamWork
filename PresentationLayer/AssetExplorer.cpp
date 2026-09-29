#include "AssetExplorer.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QTabBar>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QIcon>

namespace {

const char* kPanelStyle = R"(
    AssetExplorer {
        background: #0b0f1a;
        border-right: 1px solid #1a2336;
    }
)";

const char* kTabStyle = R"(
    QTabWidget::pane {
        background: #0b0f1a;
        border: none;
    }
    QTabBar {
        background: #0b0f1a;
        qproperty-drawBase: 0;
    }
    QTabBar::tab {
        background: #141c2e;
        color: #8b9bb4;
        border: 1px solid #243049;
        border-radius: 6px;
        padding: 5px 10px;
        margin: 4px 2px;
        font-size: 11px;
        font-weight: 600;
        min-width: 52px;
    }
    QTabBar::tab:hover {
        background: #1e293b;
        color: #e2e8f0;
    }
    QTabBar::tab:selected {
        background: #2563eb;
        color: #ffffff;
        border: 1px solid #3b82f6;
    }
)";

const char* kListStyle = R"(
    QListWidget {
        background: transparent;
        border: none;
        outline: none;
        padding: 4px;
    }
    QListWidget::item {
        background: #141c2e;
        border: 1px solid #1e293b;
        border-radius: 8px;
        margin: 3px;
        padding: 4px;
        color: #cbd5e1;
    }
    QListWidget::item:hover {
        background: #1e293b;
        border-color: #3b82f6;
    }
    QListWidget::item:selected {
        background: #1e3a5f;
        border-color: #3b82f6;
        color: #f1f5f9;
    }
    QScrollBar:vertical {
        background: #0b0f19; width: 5px; border-radius: 2px;
    }
    QScrollBar::handle:vertical {
        background: #243049; border-radius: 2px; min-height: 24px;
    }
    QScrollBar::add-line, QScrollBar::sub-line { background: none; border: none; }
)";

const char* kImportBtnStyle = R"(
    QPushButton {
        background: #141c2e;
        color: #8b9bb4;
        border: 1px solid #243049;
        border-radius: 6px;
        padding: 5px 8px;
        font-size: 11px;
        font-weight: 500;
    }
    QPushButton:hover {
        background: #1e293b;
        color: #f1f5f9;
        border-color: #3b82f6;
    }
    QPushButton:pressed {
        background: #243049;
    }
)";

const char* kDeleteBtnStyle = R"(
    QPushButton {
        background: #1a1528;
        color: #94a3b8;
        border: 1px solid #2e243d;
        border-radius: 6px;
        padding: 5px 8px;
        font-size: 11px;
        font-weight: 500;
    }
    QPushButton:hover {
        background: #2d1b38;
        color: #f87171;
        border-color: #ef4444;
    }
    QPushButton:pressed {
        background: #3b1828;
    }
)";

} // anon namespace

AssetExplorer::AssetExplorer(QWidget* parent)
    : QWidget(parent)
{
    setStyleSheet(kPanelStyle);
    setMinimumWidth(170);
    setMaximumWidth(240);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // --- Header ---
    auto* header = new QWidget(this);
    header->setStyleSheet("background: #0c101c; border-bottom: 1px solid #1a2336;");
    header->setFixedHeight(46);
    auto* hLay = new QHBoxLayout(header);
    hLay->setContentsMargins(12, 0, 12, 0);

    auto* hIcon = new QLabel("\xF0\x9F\x97\x82", header); // 🗂
    hIcon->setStyleSheet("font-size: 14px; color: #3b82f6; background: transparent;");

    auto* hTitle = new QLabel("Assets", header);
    hTitle->setStyleSheet(
        "font-size: 12px; font-weight: 700; color: #cbd5e1; background: transparent;");

    hLay->addWidget(hIcon);
    hLay->addSpacing(6);
    hLay->addWidget(hTitle);
    hLay->addStretch(1);
    root->addWidget(header);

    // --- Tabs ---
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setStyleSheet(kTabStyle);
    m_tabWidget->setDocumentMode(true);
    m_tabWidget->tabBar()->setExpanding(true);

    // --- Images tab ---
    auto* imgTab = new QWidget();
    imgTab->setStyleSheet("background: #0b0f1a;");
    auto* imgLay = new QVBoxLayout(imgTab);
    imgLay->setContentsMargins(4, 4, 4, 4);
    imgLay->setSpacing(4);

    m_importImageBtn = new QPushButton("\xF0\x9F\x93\x82  Import Image", imgTab);
    m_importImageBtn->setStyleSheet(kImportBtnStyle);
    m_importImageBtn->setCursor(Qt::PointingHandCursor);
    connect(m_importImageBtn, &QPushButton::clicked, this, &AssetExplorer::onImportImage);
    imgLay->addWidget(m_importImageBtn);

    m_imageList = new QListWidget(imgTab);
    m_imageList->setStyleSheet(kListStyle);
    m_imageList->setViewMode(QListWidget::IconMode);
    m_imageList->setIconSize(QSize(80, 60));
    m_imageList->setGridSize(QSize(96, 90));
    m_imageList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_imageList->setSpacing(4);
    m_imageList->setResizeMode(QListWidget::Adjust);
    m_imageList->setWordWrap(true);
    m_imageList->setToolTip("Double-click to load as main image");
    connect(m_imageList, &QListWidget::itemDoubleClicked,
            this, &AssetExplorer::onImageDoubleClicked);
    imgLay->addWidget(m_imageList, 1);

    m_deleteImageBtn = new QPushButton("\xF0\x9F\x97\x91  Delete Asset", imgTab);
    m_deleteImageBtn->setStyleSheet(kDeleteBtnStyle);
    m_deleteImageBtn->setCursor(Qt::PointingHandCursor);
    m_deleteImageBtn->setToolTip("Remove selected asset from session list");
    connect(m_deleteImageBtn, &QPushButton::clicked, this, &AssetExplorer::deleteSelectedAsset);
    imgLay->addWidget(m_deleteImageBtn);

    m_tabWidget->addTab(imgTab, "\xF0\x9F\x96\xBC"); // 🖼

    // --- Stickers tab ---
    auto* stkTab = new QWidget();
    stkTab->setStyleSheet("background: #0b0f1a;");
    auto* stkLay = new QVBoxLayout(stkTab);
    stkLay->setContentsMargins(4, 4, 4, 4);
    stkLay->setSpacing(4);

    m_importStickerBtn = new QPushButton("\xE2\xAD\x90  Import Sticker", stkTab);
    m_importStickerBtn->setStyleSheet(kImportBtnStyle);
    m_importStickerBtn->setCursor(Qt::PointingHandCursor);
    connect(m_importStickerBtn, &QPushButton::clicked, this, &AssetExplorer::onImportSticker);
    stkLay->addWidget(m_importStickerBtn);

    m_stickerList = new QListWidget(stkTab);
    m_stickerList->setStyleSheet(kListStyle);
    m_stickerList->setViewMode(QListWidget::IconMode);
    m_stickerList->setIconSize(QSize(80, 60));
    m_stickerList->setGridSize(QSize(96, 90));
    m_stickerList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_stickerList->setSpacing(4);
    m_stickerList->setResizeMode(QListWidget::Adjust);
    m_stickerList->setWordWrap(true);
    m_stickerList->setToolTip("Double-click to add sticker to canvas");
    connect(m_stickerList, &QListWidget::itemDoubleClicked,
            this, &AssetExplorer::onStickerDoubleClicked);
    stkLay->addWidget(m_stickerList, 1);

    m_deleteStickerBtn = new QPushButton("\xF0\x9F\x97\x91  Delete Asset", stkTab);
    m_deleteStickerBtn->setStyleSheet(kDeleteBtnStyle);
    m_deleteStickerBtn->setCursor(Qt::PointingHandCursor);
    m_deleteStickerBtn->setToolTip("Remove selected sticker from session list");
    connect(m_deleteStickerBtn, &QPushButton::clicked, this, &AssetExplorer::deleteSelectedAsset);
    stkLay->addWidget(m_deleteStickerBtn);

    m_tabWidget->addTab(stkTab, "\xE2\xAD\x90"); // ⭐

    root->addWidget(m_tabWidget, 1);
}

namespace {
QString truncateName(const QString& name, int maxLen = 16) {
    if (name.length() <= maxLen) return name;
    return name.left(maxLen - 3) + "...";
}
} // namespace

void AssetExplorer::addImage(const QString& filePath, const QImage& thumbnail)
{
    QIcon icon(QPixmap::fromImage(
        thumbnail.scaled(80, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    QString fullFileName = QFileInfo(filePath).fileName();
    auto* item = new QListWidgetItem(icon, truncateName(fullFileName), m_imageList);
    item->setSizeHint(QSize(90, 85));
    item->setData(Qt::UserRole, filePath);
    item->setToolTip(fullFileName);
    m_imageList->addItem(item);
}

void AssetExplorer::addSticker(const QString& filePath, const QImage& thumbnail)
{
    QIcon icon(QPixmap::fromImage(
        thumbnail.scaled(80, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    QString fullFileName = QFileInfo(filePath).fileName();
    auto* item = new QListWidgetItem(icon, truncateName(fullFileName), m_stickerList);
    item->setSizeHint(QSize(90, 85));
    item->setData(Qt::UserRole, filePath);
    item->setToolTip(fullFileName);
    m_stickerList->addItem(item);
}

void AssetExplorer::setAssetInUseCallback(std::function<bool(const QString& filePath)> cb)
{
    m_isAssetInUse = std::move(cb);
}

bool AssetExplorer::hasFocusedList() const
{
    return (m_imageList && m_imageList->hasFocus()) ||
           (m_stickerList && m_stickerList->hasFocus());
}

void AssetExplorer::deleteSelectedAsset()
{
    QListWidget* curList = (m_tabWidget->currentIndex() == 0) ? m_imageList : m_stickerList;
    if (!curList) return;

    auto* curItem = curList->currentItem();
    if (!curItem) return;

    QString path = curItem->data(Qt::UserRole).toString();
    if (m_isAssetInUse && m_isAssetInUse(path)) {
        QMessageBox::warning(this, "Asset In Use",
            "This asset is currently in use by the document and cannot be removed.");
        return;
    }

    delete curList->takeItem(curList->row(curItem));
}

// ---------------------------------------------------------------------------
void AssetExplorer::onImportImage()
{
    QString path = QFileDialog::getOpenFileName(
        this, "Import Image", {},
        "Images (*.png *.jpg *.jpeg *.bmp *.tga);;All Files (*)");
    if (path.isEmpty()) return;

    QImage img(path);
    if (img.isNull()) return;

    addImage(path, img);
    emit imageActivated(path);
}

void AssetExplorer::onImportSticker()
{
    QString path = QFileDialog::getOpenFileName(
        this, "Import Sticker", {},
        "Images (*.png *.jpg *.jpeg *.bmp *.tga);;All Files (*)");
    if (path.isEmpty()) return;

    QImage img(path);
    if (img.isNull()) return;

    addSticker(path, img);
    emit stickerActivated(path);
}

void AssetExplorer::onImageDoubleClicked(QListWidgetItem* item)
{
    if (!item) return;
    emit imageActivated(item->data(Qt::UserRole).toString());
}

void AssetExplorer::onStickerDoubleClicked(QListWidgetItem* item)
{
    if (!item) return;
    emit stickerActivated(item->data(Qt::UserRole).toString());
}
