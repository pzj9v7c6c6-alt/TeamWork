#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

/**
 * @brief Polished modal dialog for saving images, styled to match the GAMM UI.
 *
 * Compared to the original:
 *  - Quality slider REMOVED
 *  - "Include Edit History" checkbox REMOVED
 *  - File name field is fully editable (QLineEdit, gets focus on open)
 *  - Selecting a directory immediately stores & displays the full absolute path
 *  - Format change updates the filename extension
 */
class SaveImageDialog : public QDialog {
    Q_OBJECT

public:
    explicit SaveImageDialog(const QString& defaultPath,
                             const QString& currentFormat,
                             QWidget* parent = nullptr);

    QString selectedFilePath() const;

private slots:
    void onBrowse();
    void onFormatChanged(int index);
    void onSaveClicked();

private:
    void setupUi(const QString& defaultPath, const QString& currentFormat);

    QLineEdit*   m_fileNameEdit  = nullptr;
    QLineEdit*   m_locationEdit  = nullptr;
    QComboBox*   m_formatCombo   = nullptr;
    QPushButton* m_browseBtn     = nullptr;
    QPushButton* m_saveBtn       = nullptr;
    QPushButton* m_cancelBtn     = nullptr;

    QString m_finalPath;
};
