/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Wed Jun 19 16:54:28 2019
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QPushButton *pushButton_enlagre;
    QPushButton *pushButton_reduce;
    QPushButton *pushButton_resume;
    QPushButton *pushButton_locate;
    QPushButton *pushButton_navigation;
    QPushButton *pushButton_weather;
    QPushButton *pushButton_storage;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(480, 272);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayoutWidget = new QWidget(centralWidget);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(110, 0, 361, 201));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        pushButton_enlagre = new QPushButton(centralWidget);
        pushButton_enlagre->setObjectName(QString::fromUtf8("pushButton_enlagre"));
        pushButton_enlagre->setGeometry(QRect(0, 0, 99, 27));
        pushButton_enlagre->setLayoutDirection(Qt::LeftToRight);
        pushButton_reduce = new QPushButton(centralWidget);
        pushButton_reduce->setObjectName(QString::fromUtf8("pushButton_reduce"));
        pushButton_reduce->setGeometry(QRect(0, 30, 99, 27));
        pushButton_resume = new QPushButton(centralWidget);
        pushButton_resume->setObjectName(QString::fromUtf8("pushButton_resume"));
        pushButton_resume->setGeometry(QRect(0, 60, 99, 27));
        pushButton_resume->setLayoutDirection(Qt::LeftToRight);
        pushButton_locate = new QPushButton(centralWidget);
        pushButton_locate->setObjectName(QString::fromUtf8("pushButton_locate"));
        pushButton_locate->setGeometry(QRect(0, 90, 99, 27));
        pushButton_navigation = new QPushButton(centralWidget);
        pushButton_navigation->setObjectName(QString::fromUtf8("pushButton_navigation"));
        pushButton_navigation->setGeometry(QRect(0, 120, 99, 27));
        pushButton_weather = new QPushButton(centralWidget);
        pushButton_weather->setObjectName(QString::fromUtf8("pushButton_weather"));
        pushButton_weather->setGeometry(QRect(0, 150, 99, 27));
        pushButton_storage = new QPushButton(centralWidget);
        pushButton_storage->setObjectName(QString::fromUtf8("pushButton_storage"));
        pushButton_storage->setGeometry(QRect(0, 180, 99, 27));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 480, 31));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        pushButton_enlagre->setText(QApplication::translate("MainWindow", "\346\224\276\345\244\247", 0, QApplication::UnicodeUTF8));
        pushButton_reduce->setText(QApplication::translate("MainWindow", "\347\274\251\345\260\217", 0, QApplication::UnicodeUTF8));
        pushButton_resume->setText(QApplication::translate("MainWindow", "\351\273\230\350\256\244\345\244\247\345\260\217", 0, QApplication::UnicodeUTF8));
        pushButton_locate->setText(QApplication::translate("MainWindow", "\345\256\232\344\275\215", 0, QApplication::UnicodeUTF8));
        pushButton_navigation->setText(QApplication::translate("MainWindow", "\345\257\274\350\210\252", 0, QApplication::UnicodeUTF8));
        pushButton_weather->setText(QApplication::translate("MainWindow", "\346\260\224\350\261\241\344\277\241\346\201\257", 0, QApplication::UnicodeUTF8));
        pushButton_storage->setText(QApplication::translate("MainWindow", "\344\277\241\346\201\257\345\255\230\345\202\250", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
