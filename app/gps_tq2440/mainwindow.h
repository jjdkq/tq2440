#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QDebug>
#include <QGraphicsView>

#include <QPainter>
#include <QPaintEvent>

#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setScrollBar(int, int);                    //设置滚动条
    void locate();                                  //定位

private slots:

    void on_pushButton_enlagre_clicked();

    void on_pushButton_reduce_clicked();

    void on_pushButton_resume_clicked();

    void on_pushButton_locate_clicked();

    void on_pushButton_navigation_clicked();

    void timerUpDate();                             //定时器,用来每隔10s更新当前位置

    void on_pushButton_weather_clicked();

    void on_pushButton_storage_clicked();

private:
    Ui::MainWindow *ui;
    QGraphicsView *view;
    QGraphicsRectItem *rectItem;

    QTimer *timer;
};

#endif // MAINWINDOW_H
