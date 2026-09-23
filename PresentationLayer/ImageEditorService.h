#pragma once

#include <QObject>
#include <QImage>
#include <QString>
#include <QStringList>
#include <vector>
#include <memory>

class FilterRegistry;
class IImageFilter;

/**
 * @brief Application-level service that coordinates image loading, saving,
 *        filter application, and undo/redo history.
 *
 * This is the single point of contact between the presentation layer and
 * the backend Image/IImageFilter system. No UI class should directly
 * manipulate Image objects or call filter algorithms.
 *
 * Internally owns Image objects and converts to QImage for display.
 */
class ImageEditorService : public QObject {
    Q_OBJECT

public:
    explicit ImageEditorService(FilterRegistry* registry, QObject* parent = nullptr);
    ~ImageEditorService() override;

    // --- Image I/O ---
    bool loadImage(const QString& filePath);
    bool saveImage(const QString& filePath);

    // --- Filter application ---
    bool applyFilter(int filterIndex);
    bool applyBrightness(int delta);

    // --- History ---
    bool canUndo() const;
    bool canRedo() const;
    void undo();
    void redo();

    // --- State ---
    void clear();
    bool hasImage() const;
    bool hasProcessedImage() const;

    // --- Display ---
    QImage originalQImage() const;
    QImage processedQImage() const;

    int imageWidth() const;
    int imageHeight() const;
    QString imageFormat() const;
    QString loadedFilePath() const;

    // --- Thumbnail preview generation ---
    QImage generateThumbnail(int filterIndex, int width = 80, int height = 52) const;

signals:
    void imageLoaded();
    void imageProcessed();
    void imageSaved();
    void historyChanged();
    void imageCleared();
    void errorOccurred(const QString& message);

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
