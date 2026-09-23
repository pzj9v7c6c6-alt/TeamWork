#pragma once

#include <QString>
#include <QVector>
#include <QStringList>
#include <memory>

class IImageFilter;

/**
 * @brief Metadata descriptor for a single filter, used by the presentation layer.
 *
 * This is the presentation-side adapter that pairs a backend IImageFilter
 * with display metadata. The UI never touches concrete filter classes directly.
 */
struct FilterDescriptor {
    QString name;           ///< Human-readable display name
    QString category;       ///< Category tab: "Filters", "Effects", "Adjustments", "Tools"
    QString iconColor;      ///< Hex color for the auto-generated icon badge
    std::shared_ptr<IImageFilter> filter; ///< The actual backend filter instance
};

/**
 * @brief Central registry of all available filters.
 *
 * Populated once at startup in main.cpp with the backend filters.
 * The FiltersPanel and ControlPanel query this registry dynamically.
 * Adding/removing a filter only requires changing the registration code —
 * no UI layout changes needed.
 */
class FilterRegistry {
public:
    void registerFilter(FilterDescriptor descriptor) {
        m_filters.append(std::move(descriptor));
    }

    const QVector<FilterDescriptor>& filters() const {
        return m_filters;
    }

    QVector<FilterDescriptor> filtersByCategory(const QString& category) const {
        QVector<FilterDescriptor> result;
        for (const auto& f : m_filters) {
            if (f.category == category)
                result.append(f);
        }
        return result;
    }

    QStringList categories() const {
        QStringList cats;
        for (const auto& f : m_filters) {
            if (!cats.contains(f.category))
                cats.append(f.category);
        }
        return cats;
    }

private:
    QVector<FilterDescriptor> m_filters;
};
