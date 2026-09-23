#include "FiltersPanel.h"
#include "FilterCard.h"
#include "FilterRegistry.h"
#include "ImageEditorService.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>

FiltersPanel::FiltersPanel(FilterRegistry* registry,
                           const QString& categoryFilter,
                           QWidget* parent)
    : QWidget(parent)
    , m_registry(registry)
    , m_categoryFilter(categoryFilter)
{
    setStyleSheet("FiltersPanel { background: transparent; }");

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setStyleSheet(R"(
        QScrollArea {
            background: transparent;
            border: none;
        }
        QScrollBar:vertical {
            background: #0b0f19; width: 6px; border-radius: 3px;
        }
        QScrollBar::handle:vertical {
            background: #243049; border-radius: 3px; min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background: #3b82f6;
        }
        QScrollBar::add-line, QScrollBar::sub-line,
        QScrollBar::add-page, QScrollBar::sub-page {
            background: none; border: none;
        }
    )");

    outerLayout->addWidget(m_scrollArea);

    rebuild();
}

void FiltersPanel::rebuild()
{
    m_cards.clear();
    m_originalCard = nullptr;
    m_selectedIndex = -1;

    auto* container = new QWidget();
    container->setStyleSheet("background: transparent;");
    auto* containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(8, 8, 8, 8);
    containerLayout->setSpacing(14);

    QVector<FilterDescriptor> displayFilters;
    if (m_categoryFilter.isEmpty()) {
        displayFilters = m_registry->filters();
    } else {
        displayFilters = m_registry->filtersByCategory(m_categoryFilter);
    }

    if (displayFilters.isEmpty() && m_categoryFilter != "Filters") {
        // Empty state
        auto* emptyLabel = new QLabel(container);
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setText(
            QString(
                "<div style='text-align:center; padding:30px;'>"
                "<p style='color:#3b82f6; font-size:32px;'>\xE2\x9C\xA8</p>"
                "<p style='color:#cbd5e1; font-size:13px; font-weight:600;'>No %1 available</p>"
                "<p style='color:#64748b; font-size:11px;'>Items will appear here when registered</p>"
                "</div>"
            ).arg(m_categoryFilter.isEmpty() ? "filters" : m_categoryFilter)
        );
        containerLayout->addWidget(emptyLabel);
        containerLayout->addStretch(1);
    } else {
        if (!m_categoryFilter.isEmpty()) {
            // Single category section
            bool isQuick = (m_categoryFilter == "Filters");
            addCategorySection(m_categoryFilter, displayFilters, containerLayout, isQuick);
        } else {
            // Group by category
            QStringList categories = m_registry->categories();
            for (const auto& cat : categories) {
                auto catFilters = m_registry->filtersByCategory(cat);
                if (!catFilters.isEmpty()) {
                    addCategorySection(cat, catFilters, containerLayout, cat == "Filters");
                }
            }
        }
        containerLayout->addStretch(1);
    }

    m_scrollArea->setWidget(container);
}

void FiltersPanel::addCategorySection(const QString& category,
                                      const QVector<FilterDescriptor>& filters,
                                      QVBoxLayout* container,
                                      bool isQuickFilters)
{
    // Section header with modern icon & styling
    auto* headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(4, 4, 4, 2);

    auto* headerIcon = new QLabel(this);
    headerIcon->setStyleSheet("font-size: 13px; color: #818cf8;");
    if (category == "Filters") headerIcon->setText("\xE2\x97\x86"); // ◆
    else if (category == "Adjustments") headerIcon->setText("\xE2\x98\x80"); // ☀
    else if (category == "Effects") headerIcon->setText("\xE2\x9C\xA8"); // ✨
    else headerIcon->setText("\xF0\x9F\x94\xA7"); // 🔧

    auto* header = new QLabel(isQuickFilters ? "Quick Filters" : category, this);
    header->setStyleSheet(R"(
        QLabel {
            color: #cbd5e1;
            font-size: 13px;
            font-weight: 700;
            background: transparent;
        }
    )");

    headerLayout->addWidget(headerIcon);
    headerLayout->addSpacing(4);
    headerLayout->addWidget(header);
    headerLayout->addStretch(1);
    container->addLayout(headerLayout);

    // Grid of cards
    auto* grid = new QGridLayout();
    grid->setSpacing(8);
    grid->setContentsMargins(0, 0, 0, 0);

    const int columns = 3;
    int gridIndex = 0;

    // If this is the Filters section, add "Original" as the first card (as in reference image!)
    if (isQuickFilters) {
        m_originalCard = new FilterCard("Original", "#3b82f6", -1, this);
        m_originalCard->setSelected(true); // default selected
        connect(m_originalCard, &FilterCard::clicked, this, &FiltersPanel::onCardClicked);
        grid->addWidget(m_originalCard, 0, 0, Qt::AlignCenter);
        m_cards.append(m_originalCard);
        gridIndex = 1;
    }

    const auto& allFilters = m_registry->filters();
    for (int i = 0; i < filters.size(); ++i) {
        const auto& desc = filters[i];

        // Global index
        int globalIndex = -1;
        for (int j = 0; j < allFilters.size(); ++j) {
            if (allFilters[j].name == desc.name && allFilters[j].category == desc.category) {
                globalIndex = j;
                break;
            }
        }

        auto* card = new FilterCard(desc.name, desc.iconColor, globalIndex, this);
        connect(card, &FilterCard::clicked, this, &FiltersPanel::onCardClicked);

        grid->addWidget(card, gridIndex / columns, gridIndex % columns, Qt::AlignCenter);
        m_cards.append(card);
        gridIndex++;
    }

    container->addLayout(grid);
}

void FiltersPanel::clearSelection()
{
    for (auto* card : m_cards) {
        card->setSelected(card->filterIndex() == -1);
    }
    m_selectedIndex = -1;
}

void FiltersPanel::selectFilter(int globalIndex)
{
    m_selectedIndex = globalIndex;
    for (auto* card : m_cards) {
        card->setSelected(card->filterIndex() == globalIndex);
    }
}

void FiltersPanel::onCardClicked(int filterIndex)
{
    selectFilter(filterIndex);
    emit filterSelected(filterIndex);
}

void FiltersPanel::updateThumbnails(ImageEditorService* service)
{
    if (!service || !service->hasImage()) {
        clearThumbnails();
        return;
    }

    for (auto* card : m_cards) {
        QImage thumb = service->generateThumbnail(card->filterIndex(), 80, 52);
        if (!thumb.isNull()) {
            card->setThumbnail(QPixmap::fromImage(thumb));
        }
    }
}

void FiltersPanel::clearThumbnails()
{
    for (auto* card : m_cards) {
        card->clearThumbnail();
    }
}
