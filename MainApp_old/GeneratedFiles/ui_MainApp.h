/********************************************************************************
** Form generated from reading UI file 'MainApp.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINAPP_H
#define UI_MAINAPP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainAppClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainAppClass)
    {
        if (MainAppClass->objectName().isEmpty())
            MainAppClass->setObjectName(QStringLiteral("MainAppClass"));
        MainAppClass->resize(600, 400);
        menuBar = new QMenuBar(MainAppClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        MainAppClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainAppClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainAppClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(MainAppClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        MainAppClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainAppClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainAppClass->setStatusBar(statusBar);

        retranslateUi(MainAppClass);

        QMetaObject::connectSlotsByName(MainAppClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainAppClass)
    {
        MainAppClass->setWindowTitle(QApplication::translate("MainAppClass", "MainApp", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainAppClass: public Ui_MainAppClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINAPP_H
