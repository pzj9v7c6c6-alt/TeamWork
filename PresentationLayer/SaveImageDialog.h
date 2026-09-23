#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>

/**
 * @brief Polished modal dialog for saving images, styled to match the reference UI.
 */
class SaveImageDialog : public QDialog {
    Q_OBJECT

public:
    explicit SaveImageDialog(const QString& defaultPath,
                             const QString& currentFormat,
                             QWidget* parent = nullptr);

    QString selectedFilePath() const;
    int quality() const;
    bool includeHistory() const;

private slots:
    void onBrowse();
    void onFormatChanged(int index);
    void onQualityChanged(int value);
    void onSaveClicked();

private:
    void setupUi(const QString& defaultPath, const QString& currentFormat);

    QLineEdit* m_fileNameEdit = nullptr;
    QLineEdit* m_locationEdit = nullptr;
    QComboBox* m_formatCombo = nullptr;
    QSlider* m_qualitySlider = nullptr;
    QLabel* m_qualityLabel = nullptr;
    QCheckBox* m_historyCheckBox = nullptr;

    QPushButton* m_browseBtn = nullptr;
    QPushButton* m_saveBtn = nullptr;
    QPushButton* m_cancelBtn = nullptr;

    QString m_finalPath;
};
