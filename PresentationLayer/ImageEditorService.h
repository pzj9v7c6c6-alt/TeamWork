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
 *
 * FILTER STACKING:
 * applyFilter() / applyBrightness() / applyContrast() / applySaturation() / applyHue()
 * all operate on the CURRENT processed state (not always reset from original).
 * This allows stacking multiple filters in sequence.
 *
 * applyFilterToQImage() is exposed so the presentation layer can apply a
 * filter to an arbitrary QImage (e.g., a sticker) without touching backend
 * architecture.
 */
class ImageEditorService : public QObject {
    Q_OBJECT

public:
    explicit ImageEditorService(FilterRegistry* registry, QObject* parent = nullptr);
    ~ImageEditorService() override;

    // --- Image I/O ---
    bool loadImage(const QString& filePath);
    bool saveImage(const QString& filePath);

    // --- Filter application (stacks on current state) ---
    bool applyFilter(int filterIndex);
    bool applyBrightness(int delta);
    bool applyContrast(int value);
    bool applySaturation(int value);
    bool applyHue(int degrees);
    bool applyCrop(int x, int y, int width, int height);
    bool applyFreeRotation(int angle);
    bool applyResize(int newWidth, int newHeight);
    bool applySkew(float skewX, float skewY);
    bool applyOilPainting(int radius = 4, int levels = 20);

    /**
     * @brief Apply a registered filter to an arbitrary QImage (for stickers).
     * Returns the filtered QImage. Does NOT affect the main image state or history.
     */
    QImage applyFilterToQImage(int filterIndex, const QImage& source) const;
    QImage applyBrightnessToQImage(int delta, const QImage& source) const;
    QImage applyContrastToQImage(int value, const QImage& source) const;
    QImage applySaturationToQImage(int value, const QImage& source) const;
    QImage applyHueToQImage(int degrees, const QImage& source) const;
    QImage applyRotationToQImage(int angle, const QImage& source) const;
    QImage applySkewToQImage(float skewX, float skewY, const QImage& source) const;
    QImage applyOilPaintingToQImage(int radius, int levels, const QImage& source) const;

    // --- History ---
    bool canUndo() const;
    bool canRedo() const;
    void undo();
    void redo();

    // --- State ---
    void clear();
    bool resetToOriginal();
    bool hasImage() const;
    bool hasProcessedImage() const;
    QImage currentCommittedQImage() const;

    /**
     * @brief Push an externally-composited QImage as the new processed state.
     * Used by region-filtering (SelectionTool) in the Presentation Layer.
     * Pushes an undo snapshot first. Does NOT modify IImageFilter or Image_Class.
     */
    void setProcessedFromQImage(const QImage& img);

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
