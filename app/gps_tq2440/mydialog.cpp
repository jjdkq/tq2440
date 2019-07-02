#include "mydialog.h"
#include "ui_mydialog.h"

#include <QSqlTableModel>
#include "connection.h"

#include <cmath>

char dateStr[20];
char weekStr[20];
MyDialog::MyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MyDialog)
{
    ui->setupUi(this);

    dht11 = new device(&dht11_fd, "/dev/dht11", O_RDWR);

    rtc = new device(&rtc_fd, "/dev/rtc", O_RDWR);
    rtc->myioctl(rtc_fd, RDTIME, (unsigned long)&time);

    sprintf(dateStr, "%04x-%02x-%02x", time.year, time.mon, time.date);
    ui->pushButton->setText(tr(dateStr));

    sprintf(weekStr, "%x", time.day);
    ui->pushButton_2->setText(tr(weekStr));

    bmp180 = new device(&bmp180_fd, "/dev/bmp180", O_RDWR);

    windSpeed = new device(&windSpeed_fd, "/dev/wind_speed", O_RDWR);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerUpDate()));
    timer->start(2000);                                     //2s执行一次timerUpDate()

    model = new QSqlTableModel(this);
}

MyDialog::~MyDialog()
{
    delete ui;
}

//注意:这里要在设计模式中将lineedit提升为QLineEditIM
QLineEditIM::QLineEditIM(QWidget *parent) :
    QLineEdit(parent)

{
    QWSInputMethod *im = new SyszuxIM;
    QWSServer::setCurrentInputMethod(im);   //弹出输入法
    connect(this, SIGNAL(clicked()), this, SLOT(showInputMethod()));
}

//重写mousePressEvent()
void QLineEditIM::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked();                     //发送信号
    }

    QLineEdit::mousePressEvent(event);
}
//槽函数
void QLineEditIM::showInputMethod()
{
    clearFocus();                           //清除焦点
    setFocus();                             //获得焦点（为了点击lineedit时可以弹出输入法)
}

void MyDialog::timerUpDate()
{

    rtc->myioctl(rtc_fd, RDTIME, (unsigned long)&time);

    int data[5];
    dht11->myread(dht11_fd, data, sizeof(data));
    humidity = data[0];
    temperature = data[2];

    int data_bmp180[3];
    bmp180->myread(bmp180_fd, data_bmp180, sizeof(data_bmp180));
    pressure = data_bmp180[2];
    altitude = 44330 * (1 - pow(data_bmp180[2] / 101325.0, 1.0 / 5.255));

    int f = 0;
    windSpeed->myread(windSpeed_fd, &f, sizeof(f));
    wind_speed = (f * 7) / 80.0 + 0.1;
    if (wind_speed == 0.1)
        wind_speed = 0;

    qDebug("%02x:%02x:%02x", time.hour, time.min, time.sec);
    qDebug("pressure = %d  altitude = %.3f", pressure, altitude);
    qDebug("wind_speed = %.3f", wind_speed);

    connection(time, temperature, humidity, pressure, altitude, wind_speed);

    model->setTable("sensor");
    model->select();
    //设置编辑策略
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    ui->tableView->setModel(model);
}

//删除选中行
void MyDialog::on_pushButton_4_clicked()
{
    //获取选中的行
    int row = ui->tableView->currentIndex().row();
    //删除该行
    model->removeRow(row);
    int ok = QMessageBox::warning(this, tr("删除当前行!"),
                                  tr("确定删除当前行吗?"),QMessageBox::Yes, QMessageBox::No);
    if (ok == QMessageBox::No)          //不删除
        model->revertAll();             //撤销
    else                                //删除
        model->submitAll();             //提交,在数据库中删除该行
}

//按温度升序排列
void MyDialog::on_pushButton_5_clicked()
{
    // 温度字段, 即第1列,升序排列
    model->setSort(1, Qt::AscendingOrder);
    model->select();
}

//按温度降序排列
void MyDialog::on_pushButton_6_clicked()
{
    // 温度字段, 即第1列,降序排列
    model->setSort(1, Qt::DescendingOrder);
    model->select();
}

//按湿度升序排列
void MyDialog::on_pushButton_9_clicked()
{
    //湿度字段, 即第2列,升序排列
    model->setSort(2, Qt::AscendingOrder);
    model->select();
}

//按湿度降序排列
void MyDialog::on_pushButton_10_clicked()
{
    //湿度字段, 即第2列,降序排列
    model->setSort(2, Qt::DescendingOrder);
    model->select();
}

void mydelay_ms(unsigned int msec)
{
    QTime _Timer = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < _Timer )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//查询
void MyDialog::on_pushButton_7_clicked()
{
    QString time = ui->lineEdit->text();
    //根据时间进行筛选
    model->setFilter(QString("time = '%1'").arg(time));
    model->select();
    mydelay_ms(5000);
}

//显示全表
void MyDialog::on_pushButton_8_clicked()
{
    model->setTable("sensor");
    model->select();
}

