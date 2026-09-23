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

// Backend filter includes — only in this file and ImageEditorService.cpp
#include "ImageProcessing/Filters/BlackAndWhite/BlackAndWhiteFilter.h"
#include "ImageProcessing/Filters/Brightness/BrightnessFilter.h"
#include "ImageProcessing/Filters/Invert/InvertFilter.h"
#include "ImageProcessing/Filters/Rotate/RotateFilter.h"
#include "ImageProcessing/Filters/DetectEdges/DetectEdgesFilter.h"
#include "ImageProcessing/Filters/TVScanlines/TVScanlinesFilter.h"
#include "ImageProcessing/Filters/WanoPurple/WanoPurpleFilter.h"
#include "ImageProcessing/Filters/Crop/CropFilter.h"

#include <QApplication>
#include <QPalette>
#include <QFont>

// ---------------------------------------------------------------------------
// Application-wide dark palette
// ---------------------------------------------------------------------------
static void applyDarkTheme(QApplication& app)
{
    // Font
    QFont font("Segoe UI", 10);
    font.setHintingPreference(QFont::PreferFullHinting);
    app.setFont(font);

    // Dark palette
    QPalette p;
    p.setColor(QPalette::Window,          QColor("#0a0e1a"));
    p.setColor(QPalette::WindowText,      QColor("#f1f5f9"));
    p.setColor(QPalette::Base,            QColor("#111827"));
    p.setColor(QPalette::AlternateBase,   QColor("#1a2035"));
    p.setColor(QPalette::ToolTipBase,     QColor("#1e293b"));
    p.setColor(QPalette::ToolTipText,     QColor("#e2e8f0"));
    p.setColor(QPalette::Text,            QColor("#f1f5f9"));
    p.setColor(QPalette::Button,          QColor("#1e293b"));
    p.setColor(QPalette::ButtonText,      QColor("#e2e8f0"));
    p.setColor(QPalette::BrightText,      QColor("#ffffff"));
    p.setColor(QPalette::Link,            QColor("#6366f1"));
    p.setColor(QPalette::Highlight,       QColor("#6366f1"));
    p.setColor(QPalette::HighlightedText, QColor("#ffffff"));
    p.setColor(QPalette::Disabled, QPalette::Text,       QColor("#475569"));
    p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#475569"));
    app.setPalette(p);

    // Global stylesheet for tooltips and common elements
    app.setStyleSheet(R"(
        QToolTip {
            background: #1e293b;
            color: #e2e8f0;
            border: 1px solid #334155;
            border-radius: 4px;
            padding: 4px 8px;
            font-size: 12px;
        }
        QMessageBox {
            background: #111827;
        }
        QMessageBox QLabel {
            color: #e2e8f0;
        }
        QMessageBox QPushButton {
            background: #1e293b;
            color: #e2e8f0;
            border: 1px solid #334155;
            border-radius: 6px;
            padding: 6px 16px;
            min-width: 70px;
        }
        QMessageBox QPushButton:hover {
            background: #334155;
        }
        QFileDialog {
            background: #111827;
        }
    )");
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
        "Invert Colors", "Filters", "#f472b6",
        std::make_shared<InvertFilter>()
    });
    registry.registerFilter({
        "Purple Tone", "Filters", "#a78bfa",
        std::make_shared<WanoPurpleFilter>()
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

    // ── Adjustments ──
    registry.registerFilter({
        "Lighten", "Adjustments", "#fbbf24",
        std::make_shared<LightenFilter>()
    });
    registry.registerFilter({
        "Darken", "Adjustments", "#334155",
        std::make_shared<DarkenFilter>()
    });

    // ── Tools (rotations) ──
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

    // Note: CropFilter requires interactive (x, y, w, h) parameters.
    // It is not registered here because there is no interactive crop
    // UI yet. When a crop tool is built, it should be added here with
    // a dialog or interactive selection mechanism.
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Image Studio");
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
