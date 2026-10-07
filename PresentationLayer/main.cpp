// =============================================================================
// main.cpp — Application entry point
//
// This file is the SOLE location where:
//   1. Concrete backend filter headers are included
//   2. Filters are instantiated and registered with the FilterRegistry
//
// Adding a new filter to the UI requires ONLY editing this file.
// =============================================================================

#include "mainwindow.h"
#include "FilterRegistry.h"
#include "ImageEditorService.h"
#include "ThemeManager.h"

// Backend filter includes — only in this file and ImageEditorService.cpp
#include "ImageProcessing/Filters/BlackAndWhite/BlackAndWhiteFilter.h"
#include "ImageProcessing/Filters/Brightness/BrightnessFilter.h"
#include "ImageProcessing/Filters/Invert/InvertFilter.h"
#include "ImageProcessing/Filters/Rotate/RotateFilter.h"
#include "ImageProcessing/Filters/DetectEdges/DetectEdgesFilter.h"
#include "ImageProcessing/Filters/TVScanlines/TVScanlinesFilter.h"
#include "ImageProcessing/Filters/WanoPurple/WanoPurpleFilter.h"
#include "ImageProcessing/Filters/Crop/CropFilter.h"
#include "ImageProcessing/Filters/BlurFilter/BlurFilter.h"
#include "ImageProcessing/Filters/FrameFilter/FrameFilter.h"
#include "ImageProcessing/Filters/Gray/Grayfilter.h"
#include "ImageProcessing/Filters/InfraredFilter/InfraredFilter.h"
#include "ImageProcessing/Filters/Flip/FlipHorizontalFilterAndFlipVerticalFilter.h"
// New concrete filters (added following existing IImageFilter pattern)
#include "ImageProcessing/Filters/Contrast/ContrastFilter.h"
#include "ImageProcessing/Filters/Saturation/SaturationFilter.h"
#include "ImageProcessing/Filters/Hue/HueFilter.h"
#include "ImageProcessing/Filters/Wanw/WanoFilter.h"
#include "ImageProcessing/Filters/ResizeFilter/ResizeFilter.h"
#include "ImageProcessing/Filters/SkewFilter/SkewFilter.h"
#include "ImageProcessing/Filters/OilPaintingFilter/OilPaintingFilter.h"

#include <QApplication>
#include <QPalette>
#include <QFont>

// ---------------------------------------------------------------------------
// Application-wide dark palette (default theme)
// ---------------------------------------------------------------------------
static void applyDarkTheme(QApplication& app)
{
    // Font
    QFont font("Segoe UI", 10);
    font.setHintingPreference(QFont::PreferFullHinting);
    app.setFont(font);

    // Apply the dark theme via ThemeManager (keeps ThemeManager as source of truth)
    ThemeManager::instance()->applyTheme(ThemeManager::Dark);
}

// ---------------------------------------------------------------------------
// Filter registration
// ---------------------------------------------------------------------------
static void registerBackendFilters(FilterRegistry& registry)
{
    // ── Filters (one-click image transforms) ──
    registry.registerFilter({
        "Black & White", "Filters", "#64748b",
        std::make_shared<BlackAndWhiteFilter>()
    });
    registry.registerFilter({
        "Grayscale", "Filters", "#94a3b8",
        std::make_shared<Grayfilter>()
    });
    registry.registerFilter({
        "Invert Colors", "Filters", "#f472b6",
        std::make_shared<InvertFilter>()
    });
    registry.registerFilter({
        "Purple Tone", "Filters", "#a78bfa",
        std::make_shared<WanoPurpleFilter>()
    });
    registry.registerFilter({
        "Infrared", "Filters", "#ef4444",
        std::make_shared<InfraredFilter>()
    });
    registry.registerFilter({
        "Warm Tone", "Filters", "#f97316",
        std::make_shared<WanoFilter>()
    });

    // ── Effects ──
    registry.registerFilter({
        "TV Scanlines", "Effects", "#38bdf8",
        std::make_shared<TVScanlinesFilter>()
    });
    registry.registerFilter({
        "Detect Edges", "Effects", "#f59e0b",
        std::make_shared<DetectEdgesFilter>()
    });
    registry.registerFilter({
        "Blur", "Effects", "#60a5fa",
        std::make_shared<BlurFilter>()
    });
    registry.registerFilter({
        "Border Frame", "Effects", "#ec4899",
        std::make_shared<FrameFilter>()
    });
    registry.registerFilter({
        "Oil Painting", "Effects", "#eab308",
        std::make_shared<OilPaintingFilter>(4, 20)
    });

    // ── Adjustments ──
    registry.registerFilter({
        "Lighten", "Adjustments", "#fbbf24",
        std::make_shared<LightenFilter>()
    });
    registry.registerFilter({
        "Darken", "Adjustments", "#334155",
        std::make_shared<DarkenFilter>()
    });

    // ── Tools (rotations, flips, crop, resize) ──
    registry.registerFilter({
        "Rotate 90\xC2\xB0", "Tools", "#22d3ee",
        std::make_shared<RotateFilter>(90)
    });
    registry.registerFilter({
        "Rotate 180\xC2\xB0", "Tools", "#22d3ee",
        std::make_shared<RotateFilter>(180)
    });
    registry.registerFilter({
        "Rotate 270\xC2\xB0", "Tools", "#22d3ee",
        std::make_shared<RotateFilter>(270)
    });
    registry.registerFilter({
        "Flip Horizontal", "Tools", "#22d3ee",
        std::make_shared<FlipHorizontalFilter>()
    });
    registry.registerFilter({
        "Flip Vertical", "Tools", "#22d3ee",
        std::make_shared<FlipVerticalFilter>()
    });
    registry.registerFilter({
        "Crop to Region", "Tools", "#10b981",
        std::make_shared<CropFilter>(0, 0, 100, 100)
    });
    registry.registerFilter({
        "Resize Image", "Tools", "#06b6d4",
        std::make_shared<ResizeFilter>(800, 600)
    });
    registry.registerFilter({
        "Skew Image", "Tools", "#06b6d4",
        std::make_shared<SkewFilter>(0.3f, 0.0f)
    });
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("GYYM Studio");
    app.setOrganizationName("TeamWork");

    applyDarkTheme(app);

    // Create services
    FilterRegistry registry;
    registerBackendFilters(registry);

    ImageEditorService service(&registry);

    // Create and show main window
    MainWindow window(&service, &registry);
    window.show();

    return QApplication::exec();
}
