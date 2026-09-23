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

#include <QFileInfo>

// ---------------------------------------------------------------------------
// PIMPL
// ---------------------------------------------------------------------------
struct ImageEditorService::Impl {
    FilterRegistry* registry = nullptr;

    // The original loaded image (never modified after load)
    std::unique_ptr<Image> originalImage;

    // The current processed image (result of applying filters)
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

    void pushUndo(const QImage& snapshot) {
        if (undoStack.size() >= static_cast<size_t>(kMaxHistory))
            undoStack.erase(undoStack.begin());
        undoStack.push_back(snapshot);
        redoStack.clear();
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
// Filter application
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
        // Save current state for undo (snapshot of processed or original)
        QImage snapshot = d->processedImage
                              ? Impl::toQImage(*d->processedImage)
                              : Impl::toQImage(*d->originalImage);
        d->pushUndo(snapshot);

        // Always apply on a fresh copy of the original
        auto working = Impl::cloneImage(*d->originalImage);
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

        auto working = Impl::cloneImage(*d->originalImage);
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
        // Reconstruct Image from QImage
        auto restored = std::make_unique<Image>(prev.width(), prev.height());
        for (int y = 0; y < prev.height(); ++y) {
            const unsigned char* srcLine = prev.constScanLine(y);
            for (int x = 0; x < prev.width(); ++x) {
                restored->setPixel(x, y, 0, srcLine[x * 3 + 0]);
                restored->setPixel(x, y, 1, srcLine[x * 3 + 1]);
                restored->setPixel(x, y, 2, srcLine[x * 3 + 2]);
            }
        }
        d->processedImage = std::move(restored);
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

    auto restored = std::make_unique<Image>(next.width(), next.height());
    for (int y = 0; y < next.height(); ++y) {
        const unsigned char* srcLine = next.constScanLine(y);
        for (int x = 0; x < next.width(); ++x) {
            restored->setPixel(x, y, 0, srcLine[x * 3 + 0]);
            restored->setPixel(x, y, 1, srcLine[x * 3 + 1]);
            restored->setPixel(x, y, 2, srcLine[x * 3 + 2]);
        }
    }
    d->processedImage = std::move(restored);

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
