#pragma once

#include <QWidget>
#include <QVector>

class FilterRegistry;
class FilterCard;
class ImageEditorService;
class QVBoxLayout;
class QGridLayout;
class QScrollArea;
class QLabel;

/**
 * @brief Dynamic filter panel that generates filter cards from the FilterRegistry.
 *
 * Automatically adapts to whatever filters exist in the backend.
 * Can filter by a specific category (e.g. "Filters", "Adjustments", "Effects", "Tools")
 * or display all categories with section headers.
 * Supports live thumbnail previews generated from the currently loaded image.
 */
class FiltersPanel : public QWidget {
    Q_OBJECT

public:
    explicit FiltersPanel(FilterRegistry* registry,
                          const QString& categoryFilter = "",
                          QWidget* parent = nullptr);

    void rebuild();
    void clearSelection();
    void selectFilter(int globalIndex);
    void updateThumbnails(ImageEditorService* service);
    void clearThumbnails();

signals:
    void filterSelected(int globalIndex);

private slots:
    void onCardClicked(int filterIndex);

private:
    void addCategorySection(const QString& category,
                            const QVector<struct FilterDescriptor>& filters,
                            QVBoxLayout* container,
                            bool isQuickFilters = false);

    FilterRegistry* m_registry = nullptr;
    QString m_categoryFilter;
    QScrollArea* m_scrollArea = nullptr;
    QVector<FilterCard*> m_cards;
    FilterCard* m_originalCard = nullptr;
    int m_selectedIndex = -1;
};
