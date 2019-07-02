#ifndef MyDialog_H
#define MyDialog_H

#include <QDialog>

#include <QTimer>

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

//下面两个头文件使用输入法的时候用到
#include "syszuxpinyin.h"
#include "syszuxim.h"

#include <QLineEdit>
#include <QMouseEvent>
#include <QWidget>

#include "device.h"
#include "rtc.h"

class QLineEditIM : public QLineEdit
{
    Q_OBJECT

public:
    explicit QLineEditIM(QWidget *parent = 0);

protected:
    virtual void mousePressEvent(QMouseEvent *event);

signals:
    void clicked();              //信号

public slots:
    void showInputMethod();

};

bool createConnection();
void connection(time_t_, int, int);
class QSqlTableModel;

namespace Ui {
class MyDialog;
}

class MyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MyDialog(QWidget *parent = 0);
    ~MyDialog();

private:
    Ui::MyDialog *ui;
    QSqlTableModel *model;

    device *dht11;
    int dht11_fd;
    int temperature;
    int humidity;

    device *rtc;
    int rtc_fd;
    time_t_ time;

    device *bmp180;
    int bmp180_fd;
    int pressure;
    double altitude;

    device *windSpeed;
    int windSpeed_fd;
    double wind_speed;

    QTimer *timer;          //定时器

private slots:
    void timerUpDate();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();
};

#endif // MyDialog_H
