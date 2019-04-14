#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QTimer>
#include "dht11.h"

namespace Ui {
class MyWidget;
}

class MyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyWidget(QWidget *parent = 0);
    void dht11_display();
    ~MyWidget();

private:
    Ui::MyWidget *ui;
    dht11 *dht11obj;
    int data[5];
    char buf[20];
    QTimer *timer;          //定时器
private slots:
    void timerUpDate();

};

#endif // MYWIDGET_H
