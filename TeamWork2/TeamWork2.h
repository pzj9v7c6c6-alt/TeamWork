#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TeamWork2.h"

class TeamWork2 : public QMainWindow
{
    Q_OBJECT

public:
    TeamWork2(QWidget *parent = nullptr);
    ~TeamWork2();

private:
    Ui::TeamWork2Class ui;
};

