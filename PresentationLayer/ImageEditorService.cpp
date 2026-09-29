// =============================================================================
// ImageEditorService.cpp
// The SOLE translation unit that includes Image_Class.h (which brings in
// stb_image implementations). All Image manipulation is concentrated here.
// =============================================================================

#include "ImageEditorService.h"
#include "FilterRegistry.h"

// Backend includes — Image_Class.h triggers stb implementations.
// This is the ONLY .cpp file in the presentation layer that includes it.
#include "Interfaces/IImageFilter.h"
#include "Models/Image_Class.h"
#include "ImageProcessing/Filters/Brightness/BrightnessFilter.h"
#include "ImageProcessing/Filters/Contrast/ContrastFilter.h"
#include "ImageProcessing/Filters/Saturation/SaturationFilter.h"
#include "ImageProcessing/Filters/Hue/HueFilter.h"
#include "ImageProcessing/Filters/Crop/CropFilter.h"
#include "ImageProcessing/Filters/Rotate/RotateFilter.h"
#include "ImageProcessing/Filters/ResizeFilter/ResizeFilter.h"

#include <QFileInfo>
#include <QTransform>

// ---------------------------------------------------------------------------
// PIMPL
// ---------------------------------------------------------------------------
struct ImageEditorService::Impl {
    FilterRegistry* registry = nullptr;

    // The original loaded image (never modified after load)
    std::unique_ptr<Image> originalImage;

    // The current processed image (result of applying filters, stacked)
    std::unique_ptr<Image> processedImage;

    // Undo / redo stacks (store QImage snapshots to avoid stb in other TUs)
    std::vector<QImage> undoStack;
    std::vector<QImage> redoStack;
    static constexpr int kMaxHistory = 50;

    QString loadedFormat;
    QString loadedPath;

    // Convert backend Image → QImage for display
    static QImage toQImage(const Image& img) {
        if (!img.imageData || img.width <= 0 || img.height <= 0)
            return {};

        QImage qimg(img.width, img.height, QImage::Format_RGB888);
        for (int y = 0; y < img.height; ++y) {
            unsigned char* destLine = qimg.scanLine(y);
            const unsigned char* srcLine =
                img.imageData + y * img.width * img.channels;
            std::memcpy(destLine, srcLine, static_cast<size_t>(img.width * 3));
        }
        return qimg;
    }

    // Deep-copy an Image
    static std::unique_ptr<Image> cloneImage(const Image& src) {
        auto copy = std::make_unique<Image>();
        *copy = src; // uses the existing operator=
        return copy;
    }

    // Build an Image from a QImage (RGB888 assumed)
    static std::unique_ptr<Image> fromQImage(const QImage& qimg) {
        QImage src = qimg.convertToFormat(QImage::Format_RGB888);
        auto img = std::make_unique<Image>(src.width(), src.height());
        img->channels = 3;
        for (int y = 0; y < src.height(); ++y) {
            const unsigned char* srcLine = src.constScanLine(y);
            for (int x = 0; x < src.width(); ++x) {
                img->setPixel(x, y, 0, srcLine[x * 3 + 0]);
                img->setPixel(x, y, 1, srcLine[x * 3 + 1]);
                img->setPixel(x, y, 2, srcLine[x * 3 + 2]);
            }
        }
        return img;
    }

    void pushUndo(const QImage& snapshot) {
        if (undoStack.size() >= static_cast<size_t>(kMaxHistory))
            undoStack.erase(undoStack.begin());
        undoStack.push_back(snapshot);
        redoStack.clear();
    }

    // Returns the current working state as an Image (processedImage if present,
    // else a clone of originalImage). Used for filter stacking.
    std::unique_ptr<Image> currentWorkingImage() const {
        if (processedImage)
            return cloneImage(*processedImage);
        return cloneImage(*originalImage);
    }
};

// ---------------------------------------------------------------------------
// Construction / Destruction
// ---------------------------------------------------------------------------
ImageEditorService::ImageEditorService(FilterRegistry* registry, QObject* parent)
    : QObject(parent), d(std::make_unique<Impl>())
{
    d->registry = registry;
}

ImageEditorService::~ImageEditorService() = default;

// ---------------------------------------------------------------------------
// Image I/O
// ---------------------------------------------------------------------------
bool ImageEditorService::loadImage(const QString& filePath)
{
    try {
        auto img = std::make_unique<Image>();
        img->loadNewImage(filePath.toStdString());

        d->originalImage = std::move(img);
        d->processedImage.reset();
        d->undoStack.clear();
        d->redoStack.clear();

        QFileInfo fi(filePath);
        d->loadedFormat = fi.suffix().toUpper();
        d->loadedPath = filePath;

        emit imageLoaded();
        emit historyChanged();
        return true;
    }
    catch (const std::exception& ex) {
        emit errorOccurred(
            QString("Failed to load image: %1").arg(QString::fromStdString(ex.what())));
        return false;
    }
}

bool ImageEditorService::saveImage(const QString& filePath)
{
    try {
        const Image* target = d->processedImage ? d->processedImage.get()
                                                : d->originalImage.get();
        if (!target || !target->imageData) {
            emit errorOccurred("No image to save.");
            return false;
        }

        // We must save from a non-const Image. Clone to be safe.
        Image copy = *target;
        copy.saveImage(filePath.toStdString());

        emit imageSaved();
        return true;
    }
    catch (const std::exception& ex) {
        emit errorOccurred(
            QString("Failed to save image: %1").arg(QString::fromStdString(ex.what())));
        return false;
    }
}

// ---------------------------------------------------------------------------
// Filter application — STACKED (applies on current processed state)
// ---------------------------------------------------------------------------
bool ImageEditorService::applyFilter(int filterIndex)
{
    if (!d->originalImage || !d->originalImage->imageData) {
        emit errorOccurred("No image loaded. Please load an image first.");
        return false;
    }

    const auto& filters = d->registry->filters();
    if (filterIndex < 0 || filterIndex >= filters.size()) {
        emit errorOccurred("Invalid filter selection.");
        return false;
    }

    try {
        // Save current state for undo
        QImage snapshot = d->processedImage
                              ? Impl::toQImage(*d->processedImage)
                              : Impl::toQImage(*d->originalImage);
        d->pushUndo(snapshot);

        // Apply on top of current state (STACKING behaviour)
        auto working = d->currentWorkingImage();
        filters[filterIndex].filter->apply(*working);

        d->processedImage = std::move(working);

        emit imageProcessed();
        emit historyChanged();
        return true;
    }
    catch (const std::exception& ex) {
        emit errorOccurred(
            QString("Filter failed: %1").arg(QString::fromStdString(ex.what())));
        return false;
    }
}

bool ImageEditorService::applyBrightness(int delta)
{
    if (!d->originalImage || !d->originalImage->imageData) return false;

    try {
        QImage snapshot = d->processedImage
                              ? Impl::toQImage(*d->processedImage)
                              : Impl::toQImage(*d->originalImage);
        d->pushUndo(snapshot);

        // Stack on current state
        auto working = d->currentWorkingImage();
        BrightnessFilter bf(delta);
        bf.apply(*working);

        d->processedImage = std::move(working);

        emit imageProcessed();
        emit historyChanged();
        return true;
    }
    catch (const std::exception& ex) {
        emit errorOccurred(
            QString("Brightness failed: %1").arg(QString::fromStdString(ex.what())));
        return false;
    }
}

bool ImageEditorService::applyContrast(int value)
{
    if (!d->originalImage || !d->originalImage->imageData) return false;

    try {
        QImage snapshot = d->processedImage
                              ? Impl::toQImage(*d->processedImage)
                              : Impl::toQImage(*d->originalImage);
        d->pushUndo(snapshot);

        auto working = d->currentWorkingImage();
        ContrastFilter cf(value);
        cf.apply(*working);

        d->processedImage = std::move(working);

        emit imageProcessed();
        emit historyChanged();
        return true;
    }
    catch (const std::exception& ex) {
        emit errorOccurred(
            QString("Contrast failed: %1").arg(QString::fromStdString(ex.what())));
        return false;
    }
}

bool ImageEditorService::applySaturation(int value)
{
    if (!d->originalImage || !d->originalImage->imageData) return false;

    try {
        QImage snapshot = d->processedImage
                              ? Impl::toQImage(*d->processedImage)
                              : Impl::toQImage(*d->originalImage);
        d->pushUndo(snapshot);

        auto working = d->currentWorkingImage();
        SaturationFilter sf(value);
        sf.apply(*working);

        d->processedImage = std::move(working);

        emit imageProcessed();
        emit historyChanged();
        return true;
    }
    catch (const std::exception& ex) {
        emit errorOccurred(
            QString("Saturation failed: %1").arg(QString::fromStdString(ex.what())));
        return false;
    }
}

bool ImageEditorService::applyHue(int degrees)
{
    if (!d->originalImage || !d->originalImage->imageData) return false;

    try {
        QImage snapshot = d->processedImage
                              ? Impl::toQImage(*d->processedImage)
                              : Impl::toQImage(*d->originalImage);
        d->pushUndo(snapshot);

        auto working = d->currentWorkingImage();
        HueFilter hf(degrees);
        hf.apply(*working);

        d->processedImage = std::move(working);

        emit imageProcessed();
        emit historyChanged();
        return true;
    }
    catch (const std::exception& ex) {
        emit errorOccurred(
            QString("Hue failed: %1").arg(QString::fromStdString(ex.what())));
        return false;
    }
}

bool ImageEditorService::applyCrop(int x, int y, int width, int height)
{
    if (!d->originalImage || !d->originalImage->imageData) return false;

    try {
        QImage snapshot = d->processedImage
                              ? Impl::toQImage(*d->processedImage)
                              : Impl::toQImage(*d->originalImage);
        d->pushUndo(snapshot);

        auto working = d->currentWorkingImage();
        CropFilter cf(x, y, width, height);
        cf.apply(*working);

        d->processedImage = std::move(working);

        emit imageProcessed();
        emit historyChanged();
        return true;
    }
    catch (const std::exception& ex) {
        emit errorOccurred(
            QString("Crop failed: %1").arg(QString::fromStdString(ex.what())));
        return false;
    }
}

bool ImageEditorService::applyFreeRotation(int angle)
{
    if (!hasImage() || angle == 0) return false;
    try {
        QImage current = currentCommittedQImage();
        d->pushUndo(current);
        d->redoStack.clear();

        QTransform tfm;
        tfm.rotate(angle);
        QImage rotated = current.transformed(tfm, Qt::SmoothTransformation);
        if (rotated.isNull()) return false;

        d->processedImage = Impl::fromQImage(rotated);
        emit imageProcessed();
        emit historyChanged();
        return true;
    }
    catch (const std::exception& ex) {
        emit errorOccurred(
            QString("Rotation failed: %1").arg(QString::fromStdString(ex.what())));
        return false;
    }
}

bool ImageEditorService::applyResize(int newWidth, int newHeight)
{
    if (!hasImage() || newWidth <= 0 || newHeight <= 0) return false;
    try {
        QImage current = currentCommittedQImage();
        d->pushUndo(current);
        d->redoStack.clear();

        if (!d->processedImage) {
            d->processedImage = Impl::cloneImage(*d->originalImage);
        }
        ResizeFilter rf(newWidth, newHeight);
        rf.apply(*d->processedImage);

        emit imageProcessed();
        emit historyChanged();
        return true;
    }
    catch (const std::exception& ex) {
        emit errorOccurred(
            QString("Resize failed: %1").arg(QString::fromStdString(ex.what())));
        return false;
    }
}

QImage ImageEditorService::applyFilterToQImage(int filterIndex, const QImage& source) const
{
    if (source.isNull()) return source;

    const auto& filters = d->registry->filters();
    if (filterIndex < 0 || filterIndex >= filters.size()) return source;

    try {
        // Convert QImage → Image, apply, convert back
        auto img = Impl::fromQImage(source);
        filters[filterIndex].filter->apply(*img);
        return Impl::toQImage(*img);
    }
    catch (...) {
        return source;
    }
}

QImage ImageEditorService::applyBrightnessToQImage(int delta, const QImage& source) const
{
    if (source.isNull()) return source;
    try {
        auto img = Impl::fromQImage(source);
        BrightnessFilter bf(delta);
        bf.apply(*img);
        return Impl::toQImage(*img);
    }
    catch (...) {
        return source;
    }
}

QImage ImageEditorService::applyContrastToQImage(int value, const QImage& source) const
{
    if (source.isNull()) return source;
    try {
        auto img = Impl::fromQImage(source);
        ContrastFilter cf(value);
        cf.apply(*img);
        return Impl::toQImage(*img);
    }
    catch (...) {
        return source;
    }
}

QImage ImageEditorService::applySaturationToQImage(int value, const QImage& source) const
{
    if (source.isNull()) return source;
    try {
        auto img = Impl::fromQImage(source);
        SaturationFilter sf(value);
        sf.apply(*img);
        return Impl::toQImage(*img);
    }
    catch (...) {
        return source;
    }
}

QImage ImageEditorService::applyHueToQImage(int degrees, const QImage& source) const
{
    if (source.isNull()) return source;
    try {
        auto img = Impl::fromQImage(source);
        HueFilter hf(degrees);
        hf.apply(*img);
        return Impl::toQImage(*img);
    }
    catch (...) {
        return source;
    }
}

QImage ImageEditorService::applyRotationToQImage(int angle, const QImage& source) const
{
    if (source.isNull() || angle == 0) return source;
    QTransform tfm;
    tfm.rotate(angle);
    return source.transformed(tfm, Qt::SmoothTransformation);
}

QImage ImageEditorService::currentCommittedQImage() const
{
    if (hasProcessedImage())
        return processedQImage();
    return originalQImage();
}

bool ImageEditorService::resetToOriginal()
{
    if (!d->originalImage || !d->originalImage->imageData) return false;
    try {
        QImage snapshot = d->processedImage
                              ? Impl::toQImage(*d->processedImage)
                              : Impl::toQImage(*d->originalImage);
        d->pushUndo(snapshot);
        d->processedImage = Impl::cloneImage(*d->originalImage);
        emit imageProcessed();
        emit historyChanged();
        return true;
    }
    catch (...) {
        return false;
    }
}

// ---------------------------------------------------------------------------
// History
// ---------------------------------------------------------------------------
bool ImageEditorService::canUndo() const { return !d->undoStack.empty(); }
bool ImageEditorService::canRedo() const { return !d->redoStack.empty(); }

void ImageEditorService::undo()
{
    if (d->undoStack.empty()) return;

    // Save current processed state to redo
    if (d->processedImage) {
        d->redoStack.push_back(Impl::toQImage(*d->processedImage));
    }

    // Restore from undo stack
    QImage prev = d->undoStack.back();
    d->undoStack.pop_back();

    // Check if the undo target is essentially the original (no processed image)
    QImage origQImg = Impl::toQImage(*d->originalImage);
    if (prev == origQImg) {
        d->processedImage.reset();
    } else {
        d->processedImage = Impl::fromQImage(prev);
    }

    emit imageProcessed();
    emit historyChanged();
}

void ImageEditorService::redo()
{
    if (d->redoStack.empty()) return;

    // Push current to undo
    if (d->processedImage) {
        d->undoStack.push_back(Impl::toQImage(*d->processedImage));
    } else if (d->originalImage) {
        d->undoStack.push_back(Impl::toQImage(*d->originalImage));
    }

    QImage next = d->redoStack.back();
    d->redoStack.pop_back();

    d->processedImage = Impl::fromQImage(next);

    emit imageProcessed();
    emit historyChanged();
}

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------
void ImageEditorService::clear()
{
    d->processedImage.reset();
    d->undoStack.clear();
    d->redoStack.clear();

    emit imageProcessed();
    emit historyChanged();
}

void ImageEditorService::setProcessedFromQImage(const QImage& img)
{
    if (img.isNull()) return;

    // Push current state to undo stack
    QImage snapshot = d->processedImage
                          ? Impl::toQImage(*d->processedImage)
                          : (d->originalImage ? Impl::toQImage(*d->originalImage) : QImage{});
    if (!snapshot.isNull())
        d->pushUndo(snapshot);

    d->processedImage = Impl::fromQImage(img);

    emit imageProcessed();
    emit historyChanged();
}

bool ImageEditorService::hasImage() const
{
    return d->originalImage && d->originalImage->imageData;
}

bool ImageEditorService::hasProcessedImage() const
{
    return d->processedImage && d->processedImage->imageData;
}

// ---------------------------------------------------------------------------
// Display conversions
// ---------------------------------------------------------------------------
QImage ImageEditorService::originalQImage() const
{
    if (!d->originalImage) return {};
    return Impl::toQImage(*d->originalImage);
}

QImage ImageEditorService::processedQImage() const
{
    if (!d->processedImage) return {};
    return Impl::toQImage(*d->processedImage);
}

int ImageEditorService::imageWidth() const
{
    return d->originalImage ? d->originalImage->width : 0;
}

int ImageEditorService::imageHeight() const
{
    return d->originalImage ? d->originalImage->height : 0;
}

QString ImageEditorService::imageFormat() const
{
    return d->loadedFormat;
}

QString ImageEditorService::loadedFilePath() const
{
    return d->loadedPath;
}

// ---------------------------------------------------------------------------
// Thumbnail generation
// ---------------------------------------------------------------------------
QImage ImageEditorService::generateThumbnail(int filterIndex, int width, int height) const
{
    if (!d->originalImage || !d->originalImage->imageData) return {};

    QImage orig = Impl::toQImage(*d->originalImage);
    if (orig.isNull()) return {};

    QImage scaled = orig.scaled(width, height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    int x = (scaled.width() - width) / 2;
    int y = (scaled.height() - height) / 2;
    scaled = scaled.copy(x, y, width, height);

    if (filterIndex < 0) {
        return scaled; // Original thumbnail
    }

    const auto& filters = d->registry->filters();
    if (filterIndex >= filters.size()) return scaled;

    Image thumbImg(scaled.width(), scaled.height());
    for (int cy = 0; cy < scaled.height(); ++cy) {
        const unsigned char* srcLine = scaled.constScanLine(cy);
        for (int cx = 0; cx < scaled.width(); ++cx) {
            thumbImg.setPixel(cx, cy, 0, srcLine[cx * 3 + 0]);
            thumbImg.setPixel(cx, cy, 1, srcLine[cx * 3 + 1]);
            thumbImg.setPixel(cx, cy, 2, srcLine[cx * 3 + 2]);
        }
    }

    try {
        filters[filterIndex].filter->apply(thumbImg);
        return Impl::toQImage(thumbImg);
    }
    catch (...) {
        return scaled;
    }
}
