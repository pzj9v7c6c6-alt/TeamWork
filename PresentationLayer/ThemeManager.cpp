#include "ThemeManager.h"

#include <QApplication>
#include <QPalette>

// ---------------------------------------------------------------------------
// Singleton
// ---------------------------------------------------------------------------
ThemeManager* ThemeManager::instance()
{
    static ThemeManager inst;
    return &inst;
}

ThemeManager::ThemeManager(QObject* parent)
    : QObject(parent)
{
}

QStringList ThemeManager::themeNames() const
{
    return { "Dark", "Light", "Spring" };
}

ThemeManager::Theme ThemeManager::currentTheme() const
{
    return m_current;
}

// ---------------------------------------------------------------------------
// Palette query helpers
// ---------------------------------------------------------------------------
QColor ThemeManager::windowColor() const {
    switch (m_current) {
    case Light:  return QColor("#f1f5f9");
    case Spring: return QColor("#0a1a0a");
    default:     return QColor("#0a0e1a");
    }
}
QColor ThemeManager::panelColor() const {
    switch (m_current) {
    case Light:  return QColor("#ffffff");
    case Spring: return QColor("#0c1e0c");
    default:     return QColor("#0f1524");
    }
}
QColor ThemeManager::borderColor() const {
    switch (m_current) {
    case Light:  return QColor("#cbd5e1");
    case Spring: return QColor("#1a3d1c");
    default:     return QColor("#1e293b");
    }
}
QColor ThemeManager::textColor() const {
    switch (m_current) {
    case Light:  return QColor("#0f172a");
    case Spring: return QColor("#d1fae5");
    default:     return QColor("#f1f5f9");
    }
}
QColor ThemeManager::dimTextColor() const {
    switch (m_current) {
    case Light:  return QColor("#64748b");
    case Spring: return QColor("#6ee7b7");
    default:     return QColor("#94a3b8");
    }
}
QColor ThemeManager::accentColor() const {
    switch (m_current) {
    case Light:  return QColor("#2563eb");
    case Spring: return QColor("#10b981");
    default:     return QColor("#3b82f6");
    }
}
QColor ThemeManager::hoverBg() const {
    switch (m_current) {
    case Light:  return QColor("#e2e8f0");
    case Spring: return QColor("#14532d");
    default:     return QColor("#1e293b");
    }
}
QColor ThemeManager::inputBg() const {
    switch (m_current) {
    case Light:  return QColor("#f8fafc");
    case Spring: return QColor("#071207");
    default:     return QColor("#0d1322");
    }
}

// ---------------------------------------------------------------------------
// Apply
// ---------------------------------------------------------------------------
void ThemeManager::applyTheme(int index)
{
    // Clamp to valid range
    if (index < 0) index = 0;
    if (index > 2) index = 2;
    applyTheme(static_cast<Theme>(index));
}

void ThemeManager::applyTheme(Theme theme)
{
    m_current = theme;

    QPalette p;
    switch (theme) {
    // ── Light ──────────────────────────────────────────────────────────────
    case Light:
        p.setColor(QPalette::Window,          QColor("#f1f5f9"));
        p.setColor(QPalette::WindowText,      QColor("#0f172a"));
        p.setColor(QPalette::Base,            QColor("#ffffff"));
        p.setColor(QPalette::AlternateBase,   QColor("#e2e8f0"));
        p.setColor(QPalette::ToolTipBase,     QColor("#e2e8f0"));
        p.setColor(QPalette::ToolTipText,     QColor("#0f172a"));
        p.setColor(QPalette::Text,            QColor("#0f172a"));
        p.setColor(QPalette::Button,          QColor("#e2e8f0"));
        p.setColor(QPalette::ButtonText,      QColor("#0f172a"));
        p.setColor(QPalette::BrightText,      QColor("#000000"));
        p.setColor(QPalette::Link,            QColor("#2563eb"));
        p.setColor(QPalette::Highlight,       QColor("#2563eb"));
        p.setColor(QPalette::HighlightedText, QColor("#ffffff"));
        p.setColor(QPalette::Disabled, QPalette::Text,       QColor("#94a3b8"));
        p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#94a3b8"));
        break;

    // ── Spring ─────────────────────────────────────────────────────────────
    case Spring:
        p.setColor(QPalette::Window,          QColor("#0a1a0a"));
        p.setColor(QPalette::WindowText,      QColor("#d1fae5"));
        p.setColor(QPalette::Base,            QColor("#071207"));
        p.setColor(QPalette::AlternateBase,   QColor("#0f2210"));
        p.setColor(QPalette::ToolTipBase,     QColor("#0f2210"));
        p.setColor(QPalette::ToolTipText,     QColor("#d1fae5"));
        p.setColor(QPalette::Text,            QColor("#d1fae5"));
        p.setColor(QPalette::Button,          QColor("#0f2210"));
        p.setColor(QPalette::ButtonText,      QColor("#d1fae5"));
        p.setColor(QPalette::BrightText,      QColor("#ffffff"));
        p.setColor(QPalette::Link,            QColor("#34d399"));
        p.setColor(QPalette::Highlight,       QColor("#10b981"));
        p.setColor(QPalette::HighlightedText, QColor("#ffffff"));
        p.setColor(QPalette::Disabled, QPalette::Text,       QColor("#4d8060"));
        p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#4d8060"));
        break;

    // ── Dark (default) ─────────────────────────────────────────────────────
    default:
    case Dark:
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
        p.setColor(QPalette::Link,            QColor("#3b82f6"));
        p.setColor(QPalette::Highlight,       QColor("#3b82f6"));
        p.setColor(QPalette::HighlightedText, QColor("#ffffff"));
        p.setColor(QPalette::Disabled, QPalette::Text,       QColor("#475569"));
        p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#475569"));
        break;
    }

    qApp->setPalette(p);
    qApp->setStyleSheet(buildStylesheet(theme));

    emit themeChanged(theme);
}

// ---------------------------------------------------------------------------
// Comprehensive stylesheet builder — covers ALL widgets
// ---------------------------------------------------------------------------
QString ThemeManager::buildStylesheet(Theme theme)
{
    // Define colour tokens per theme
    QString winBg, panelBg, inputBg, borderCol, hoverBg, pressedBg;
    QString textCol, dimText, accentCol, accentHover, disabledText;
    QString selBg;

    switch (theme) {
    case Light:
        winBg       = "#f1f5f9";
        panelBg     = "#ffffff";
        inputBg     = "#f8fafc";
        borderCol   = "#cbd5e1";
        hoverBg     = "#e2e8f0";
        pressedBg   = "#cbd5e1";
        textCol     = "#0f172a";
        dimText     = "#64748b";
        accentCol   = "#2563eb";
        accentHover = "#3b82f6";
        disabledText= "#94a3b8";
        selBg       = "#dbeafe";
        break;

    case Spring:
        winBg       = "#0a1a0a";
        panelBg     = "#0c1e0c";
        inputBg     = "#071207";
        borderCol   = "#1a3d1c";
        hoverBg     = "#14532d";
        pressedBg   = "#1a3d1c";
        textCol     = "#d1fae5";
        dimText     = "#6ee7b7";
        accentCol   = "#10b981";
        accentHover = "#34d399";
        disabledText= "#4d8060";
        selBg       = "#14532d";
        break;

    default: // Dark
        winBg       = "#0a0e1a";
        panelBg     = "#0f1524";
        inputBg     = "#0d1322";
        borderCol   = "#1e293b";
        hoverBg     = "#1e293b";
        pressedBg   = "#243049";
        textCol     = "#f1f5f9";
        dimText     = "#94a3b8";
        accentCol   = "#3b82f6";
        accentHover = "#60a5fa";
        disabledText= "#475569";
        selBg       = "#1e3a5f";
        break;
    }

    return QString(R"(
        /* ── Global ── */
        QMainWindow, QWidget {
            background: %1;
            color: %7;
        }

        /* ── Tooltips ── */
        QToolTip {
            background: %2;
            color: %7;
            border: 1px solid %4;
            border-radius: 4px;
            padding: 4px 8px;
            font-size: 12px;
        }

        /* ── Message boxes ── */
        QMessageBox {
            background: %2;
        }
        QMessageBox QLabel {
            color: %7;
        }
        QMessageBox QPushButton {
            background: %5;
            color: %7;
            border: 1px solid %4;
            border-radius: 6px;
            padding: 6px 16px;
            min-width: 70px;
        }
        QMessageBox QPushButton:hover {
            background: %6;
        }

        /* ── File dialogs ── */
        QFileDialog {
            background: %2;
        }

        /* ── Tab widgets ── */
        QTabWidget::pane {
            background: %2;
            border: none;
        }
        QTabBar {
            background: %1;
            qproperty-drawBase: 0;
        }
        QTabBar::tab {
            background: %3;
            color: %8;
            border: 1px solid %4;
            border-radius: 8px;
            padding: 7px 12px;
            margin-right: 6px;
            font-size: 11px;
            font-weight: 600;
            min-width: 52px;
        }
        QTabBar::tab:hover {
            background: %5;
            color: %7;
            border-color: %9;
        }
        QTabBar::tab:selected {
            background: %9;
            color: #ffffff;
            border: 1px solid %10;
        }

        /* ── Scroll areas ── */
        QScrollArea {
            background: transparent;
            border: none;
        }

        /* ── Scroll bars ── */
        QScrollBar:vertical {
            background: %1;
            width: 6px;
            border-radius: 3px;
        }
        QScrollBar::handle:vertical {
            background: %4;
            border-radius: 3px;
            min-height: 24px;
        }
        QScrollBar::handle:vertical:hover {
            background: %9;
        }
        QScrollBar:horizontal {
            background: %1;
            height: 6px;
            border-radius: 3px;
        }
        QScrollBar::handle:horizontal {
            background: %4;
            border-radius: 3px;
            min-width: 24px;
        }
        QScrollBar::handle:horizontal:hover {
            background: %9;
        }
        QScrollBar::add-line, QScrollBar::sub-line,
        QScrollBar::add-page, QScrollBar::sub-page {
            background: none;
            border: none;
        }

        /* ── Push buttons (default) ── */
        QPushButton {
            background: %3;
            color: %8;
            border: 1px solid %4;
            border-radius: 8px;
            padding: 7px 16px;
            font-size: 12px;
            font-weight: 500;
            min-height: 18px;
        }
        QPushButton:hover {
            background: %5;
            color: %7;
            border-color: %9;
        }
        QPushButton:pressed {
            background: %6;
        }
        QPushButton:disabled {
            background: %1;
            color: %11;
            border-color: %4;
        }
        QPushButton:checked {
            background: %9;
            color: #ffffff;
            border-color: %10;
        }

        /* ── Combo boxes ── */
        QComboBox {
            background: %3;
            color: %8;
            border: 1px solid %4;
            border-radius: 6px;
            padding: 4px 10px;
            font-size: 11px;
            font-weight: 500;
        }
        QComboBox:hover {
            border-color: %9;
            color: %7;
        }
        QComboBox::drop-down {
            border: none;
            width: 18px;
        }
        QComboBox QAbstractItemView {
            background: %3;
            color: %7;
            selection-background-color: %9;
            border: 1px solid %4;
            border-radius: 6px;
            padding: 4px;
            font-size: 11px;
        }

        /* ── Line edits ── */
        QLineEdit {
            background-color: %3;
            color: %7;
            border: 1px solid %4;
            border-radius: 8px;
            padding: 8px 12px;
            font-size: 13px;
            selection-background-color: %9;
        }
        QLineEdit:focus {
            border: 1px solid %9;
        }

        /* ── Sliders ── */
        QSlider::groove:horizontal {
            height: 4px;
            background: %4;
            border-radius: 2px;
        }
        QSlider::handle:horizontal {
            background: #ffffff;
            width: 14px;
            height: 14px;
            margin: -5px 0;
            border-radius: 7px;
        }
        QSlider::handle:horizontal:hover {
            background: %8;
        }
        QSlider::sub-page:horizontal {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                         stop:0 %9, stop:1 %10);
            border-radius: 2px;
        }

        /* ── List widgets ── */
        QListWidget {
            background: transparent;
            border: none;
            outline: none;
            padding: 4px;
        }
        QListWidget::item {
            background: %3;
            border: 1px solid %4;
            border-radius: 8px;
            margin: 3px;
            padding: 4px;
            color: %7;
        }
        QListWidget::item:hover {
            background: %5;
            border-color: %9;
        }
        QListWidget::item:selected {
            background: %12;
            border-color: %9;
            color: %7;
        }

        /* ── Graphics views ── */
        QGraphicsView {
            background: %1;
            border: none;
            border-radius: 8px;
        }

        /* ── Group boxes ── */
        QGroupBox {
            background: %2;
            border: 1px solid %4;
            border-radius: 8px;
            margin-top: 8px;
            padding: 12px;
            color: %7;
        }
        QGroupBox::title {
            color: %8;
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 0 6px;
        }

        /* ── Labels — only override color, preserve backgrounds ── */
        QLabel {
            color: %7;
            background: transparent;
        }

        /* ── Splitters ── */
        QSplitter::handle {
            background: %4;
            width: 1px;
        }
    )")
    .arg(winBg)      // 1
    .arg(panelBg)    // 2
    .arg(inputBg)    // 3
    .arg(borderCol)  // 4
    .arg(hoverBg)    // 5
    .arg(pressedBg)  // 6
    .arg(textCol)    // 7
    .arg(dimText)    // 8
    .arg(accentCol)  // 9
    .arg(accentHover)// 10
    .arg(disabledText)// 11
    .arg(selBg);     // 12
}
