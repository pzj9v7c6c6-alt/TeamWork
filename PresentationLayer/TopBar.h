#pragma once

#include <QWidget>

class QPushButton;

/**
 * @brief Top application bar with GYYM branding and action buttons.
 */
class TopBar : public QWidget {
    Q_OBJECT

public:
    explicit TopBar(QWidget* parent = nullptr);

    void setUndoEnabled(bool enabled);
    void setRedoEnabled(bool enabled);
    void setSaveEnabled(bool enabled);
    void setClearEnabled(bool enabled);

signals:
    void loadClicked();
    void saveClicked();
    void undoClicked();
    void redoClicked();
    void clearClicked();

private:
    QPushButton* m_loadBtn  = nullptr;
    QPushButton* m_saveBtn  = nullptr;
    QPushButton* m_undoBtn  = nullptr;
    QPushButton* m_redoBtn  = nullptr;
    QPushButton* m_clearBtn = nullptr;

    QPushButton* createActionButton(const QString& text, const QString& iconChar,
                                    const QString& accentColor = {});
};
