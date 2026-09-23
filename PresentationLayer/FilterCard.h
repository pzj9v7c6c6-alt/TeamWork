#pragma once

#include <QWidget>
#include <QPixmap>

class QLabel;

/**
 * @brief A single filter card in the filter grid, matching the reference image.
 *
 * Displays a thumbnail image preview (or colored icon badge if no image loaded),
 * filter name, hover glow, and a selected state with accent border and checkmark.
 */
class FilterCard : public QWidget {
    Q_OBJECT

public:
    FilterCard(const QString& name, const QString& iconColor, int index,
               QWidget* parent = nullptr);

    void setSelected(bool selected);
    bool isSelected() const;
    int filterIndex() const;

    void setThumbnail(const QPixmap& pixmap);
    void clearThumbnail();

signals:
    void clicked(int filterIndex);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void updateStyle();

    QString m_name;
    QString m_iconColor;
    int m_index;
    bool m_selected = false;
    bool m_hovered = false;
    QPixmap m_thumbnail;

    QLabel* m_iconLabel = nullptr;
    QLabel* m_nameLabel = nullptr;
};
