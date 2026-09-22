/********************************************************************************
** Form generated from reading UI file 'TeamWork2.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEAMWORK2_H
#define UI_TEAMWORK2_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TeamWork2Class
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *TeamWork2Class)
    {
        if (TeamWork2Class->objectName().isEmpty())
            TeamWork2Class->setObjectName("TeamWork2Class");
        TeamWork2Class->resize(600, 400);
        menuBar = new QMenuBar(TeamWork2Class);
        menuBar->setObjectName("menuBar");
        TeamWork2Class->setMenuBar(menuBar);
        mainToolBar = new QToolBar(TeamWork2Class);
        mainToolBar->setObjectName("mainToolBar");
        TeamWork2Class->addToolBar(mainToolBar);
        centralWidget = new QWidget(TeamWork2Class);
        centralWidget->setObjectName("centralWidget");
        TeamWork2Class->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(TeamWork2Class);
        statusBar->setObjectName("statusBar");
        TeamWork2Class->setStatusBar(statusBar);

        retranslateUi(TeamWork2Class);

        QMetaObject::connectSlotsByName(TeamWork2Class);
    } // setupUi

    void retranslateUi(QMainWindow *TeamWork2Class)
    {
        TeamWork2Class->setWindowTitle(QCoreApplication::translate("TeamWork2Class", "TeamWork2", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TeamWork2Class: public Ui_TeamWork2Class {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEAMWORK2_H
