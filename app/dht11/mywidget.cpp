#include "mywidget.h"
#include "ui_mywidget.h"
#include "dht11.h"

MyWidget::MyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyWidget)
{
    ui->setupUi(this);
    dht11obj = new dht11();
    timer = new QTimer(this);
    /* 关联信号和槽
     * 信号:timer->timeout();
     * 槽:timerUpDate()
     */
    connect(timer, SIGNAL(timeout()), this, SLOT(timerUpDate()));
    timer->start(2000);                         //2s定时刷新一次数据
}

void MyWidget::dht11_display()
{
    dht11obj->dht11_read(data);
    sprintf(buf, "%d ℃", data[2]);
    ui->pushButton->setText(tr(buf));
    sprintf(buf, "%d %%", data[0]);
    ui->pushButton_2->setText(tr(buf));
}

//timer的槽函数
void MyWidget::timerUpDate()
{
    dht11_display();                        //定时器刷新时执行该函数
}


MyWidget::~MyWidget()
{
    delete ui;
}
