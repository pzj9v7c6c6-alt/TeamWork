#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QColor>

class QApplication;

/**
 * @brief Manages application-wide colour themes.
 *
 * Three themes: Dark, Light, Spring.
 * Applies both QPalette and a comprehensive stylesheet so all
 * widgets (including those with local stylesheets) are themed.
 */
class ThemeManager : public QObject {
    Q_OBJECT

public:
    enum Theme {
        Dark = 0,
        Light,
        Spring
    };

    static ThemeManager* instance();

    QStringList themeNames() const;
    Theme currentTheme() const;

    void applyTheme(Theme theme);
    void applyTheme(int index);

    // Palette query helpers for widgets that need to build dynamic styles
    QColor windowColor() const;
    QColor panelColor() const;
    QColor borderColor() const;
    QColor textColor() const;
    QColor dimTextColor() const;
    QColor accentColor() const;
    QColor hoverBg() const;
    QColor inputBg() const;

signals:
    void themeChanged(Theme theme);

private:
    explicit ThemeManager(QObject* parent = nullptr);

    static QString buildStylesheet(Theme theme);

    Theme m_current = Dark;
};
