#pragma once

#include <QWidget>
#include <QVector>
#include <QImage>
#include <QString>
#include <functional>

class QListWidget;
class QTabWidget;
class QPushButton;
class QListWidgetItem;

/**
 * @brief Left-side asset explorer panel.
 *
 * Two tabs: Images and Stickers.
 * Only shows assets imported during the current editing session.
 * Emits signals when an asset is activated (double-click or button).
 * Supports deleting assets from the session list with in-use protection.
 */
class AssetExplorer : public QWidget {
    Q_OBJECT

public:
    explicit AssetExplorer(QWidget* parent = nullptr);

    void addImage(const QString& filePath, const QImage& thumbnail);
    void addSticker(const QString& filePath, const QImage& thumbnail);

    void setAssetInUseCallback(std::function<bool(const QString& filePath)> cb);
    bool hasFocusedList() const;
    void deleteSelectedAsset();

signals:
    void imageActivated(const QString& filePath);    ///< Use as main document image
    void stickerActivated(const QString& filePath);  ///< Place sticker on canvas

private slots:
    void onImportImage();
    void onImportSticker();
    void onImageDoubleClicked(QListWidgetItem* item);
    void onStickerDoubleClicked(QListWidgetItem* item);

private:
    QTabWidget*   m_tabWidget        = nullptr;
    QListWidget*  m_imageList        = nullptr;
    QListWidget*  m_stickerList      = nullptr;
    QPushButton*  m_importImageBtn   = nullptr;
    QPushButton*  m_importStickerBtn = nullptr;
    QPushButton*  m_deleteImageBtn   = nullptr;
    QPushButton*  m_deleteStickerBtn = nullptr;

    std::function<bool(const QString& filePath)> m_isAssetInUse;
};
