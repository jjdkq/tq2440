/********************************************************************************
** Form generated from reading UI file 'mydialog.ui'
**
** Created: Wed Jun 19 17:05:42 2019
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYDIALOG_H
#define UI_MYDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QTableView>

QT_BEGIN_NAMESPACE

class QLineEditIM;

class Ui_MyDialog
{
public:
    QPushButton *pushButton;
    QPushButton *pushButton_4;
    QPushButton *pushButton_10;
    QLabel *label_3;
    QPushButton *pushButton_6;
    QPushButton *pushButton_5;
    QPushButton *pushButton_2;
    QPushButton *pushButton_7;
    QPushButton *pushButton_9;
    QTableView *tableView;
    QLineEditIM *lineEdit;
    QPushButton *pushButton_8;
    QLabel *label_2;
    QLabel *label;

    void setupUi(QDialog *MyDialog)
    {
        if (MyDialog->objectName().isEmpty())
            MyDialog->setObjectName(QString::fromUtf8("MyDialog"));
        MyDialog->resize(480, 272);
        pushButton = new QPushButton(MyDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(60, 10, 99, 27));
        pushButton_4 = new QPushButton(MyDialog);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setGeometry(QRect(370, 50, 99, 27));
        pushButton_10 = new QPushButton(MyDialog);
        pushButton_10->setObjectName(QString::fromUtf8("pushButton_10"));
        pushButton_10->setGeometry(QRect(360, 170, 111, 31));
        label_3 = new QLabel(MyDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(180, 10, 67, 17));
        pushButton_6 = new QPushButton(MyDialog);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));
        pushButton_6->setGeometry(QRect(360, 110, 111, 31));
        pushButton_5 = new QPushButton(MyDialog);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));
        pushButton_5->setGeometry(QRect(360, 80, 111, 31));
        pushButton_2 = new QPushButton(MyDialog);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(230, 10, 51, 27));
        pushButton_7 = new QPushButton(MyDialog);
        pushButton_7->setObjectName(QString::fromUtf8("pushButton_7"));
        pushButton_7->setGeometry(QRect(190, 210, 99, 27));
        pushButton_9 = new QPushButton(MyDialog);
        pushButton_9->setObjectName(QString::fromUtf8("pushButton_9"));
        pushButton_9->setGeometry(QRect(360, 140, 111, 31));
        tableView = new QTableView(MyDialog);
        tableView->setObjectName(QString::fromUtf8("tableView"));
        tableView->setGeometry(QRect(10, 40, 351, 161));
        lineEdit = new QLineEditIM(MyDialog);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(60, 210, 113, 27));
        pushButton_8 = new QPushButton(MyDialog);
        pushButton_8->setObjectName(QString::fromUtf8("pushButton_8"));
        pushButton_8->setGeometry(QRect(310, 210, 99, 27));
        label_2 = new QLabel(MyDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 10, 67, 17));
        label = new QLabel(MyDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 210, 67, 17));

        retranslateUi(MyDialog);

        QMetaObject::connectSlotsByName(MyDialog);
    } // setupUi

    void retranslateUi(QDialog *MyDialog)
    {
        MyDialog->setWindowTitle(QApplication::translate("MyDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("MyDialog", "PushButton", 0, QApplication::UnicodeUTF8));
        pushButton_4->setText(QApplication::translate("MyDialog", "\345\210\240\351\231\244\351\200\211\344\270\255\350\241\214", 0, QApplication::UnicodeUTF8));
        pushButton_10->setText(QApplication::translate("MyDialog", "\346\214\211\346\271\277\345\272\246\351\231\215\345\272\217\346\216\222\345\210\227", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MyDialog", " \346\230\237\346\234\237:", 0, QApplication::UnicodeUTF8));
        pushButton_6->setText(QApplication::translate("MyDialog", "\346\214\211\346\270\251\345\272\246\351\231\215\345\272\217\346\216\222\345\210\227", 0, QApplication::UnicodeUTF8));
        pushButton_5->setText(QApplication::translate("MyDialog", "\346\214\211\346\270\251\345\272\246\345\215\207\345\272\217\346\216\222\345\210\227", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("MyDialog", "PushButton", 0, QApplication::UnicodeUTF8));
        pushButton_7->setText(QApplication::translate("MyDialog", "\346\237\245\350\257\242", 0, QApplication::UnicodeUTF8));
        pushButton_9->setText(QApplication::translate("MyDialog", "\346\214\211\346\271\277\345\272\246\345\215\207\345\272\217\346\216\222\345\210\227", 0, QApplication::UnicodeUTF8));
        pushButton_8->setText(QApplication::translate("MyDialog", "\346\230\276\347\244\272\345\205\250\350\241\250", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MyDialog", "\346\227\245\346\234\237:", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MyDialog", "\346\227\266\351\227\264:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MyDialog: public Ui_MyDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYDIALOG_H
