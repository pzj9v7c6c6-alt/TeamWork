#pragma once

#include <QWidget>

class QLabel;
class QSlider;
class QPushButton;

/**
 * @brief Bottom status bar with image info and zoom controls.
 */
class StatusBar : public QWidget {
    Q_OBJECT

public:
    explicit StatusBar(QWidget* parent = nullptr);

    void setImageInfo(int width, int height, const QString& format);
    void setZoomPercent(double percent);
    void clearInfo();

signals:
    void zoomInClicked();
    void zoomOutClicked();
    void fitToViewClicked();
    void zoomValueChanged(double percent);

private:
    QLabel* m_infoLabel = nullptr;
    QLabel* m_zoomLabel = nullptr;
    QSlider* m_zoomSlider = nullptr;
    QPushButton* m_zoomInBtn = nullptr;
    QPushButton* m_zoomOutBtn = nullptr;
    QPushButton* m_fitBtn = nullptr;
};
