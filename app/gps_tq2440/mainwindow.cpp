#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "myitem.h"
#include <QIcon>

#include <QScrollBar>
#include <QTime>
#include <QMatrix>
#include <QMessageBox>

#include "uart.h"
#include "gps.h"

#include "mydialog.h"
#include "connection.h"
#include <QTextCodec>

#include <QSqlQuery>
#include <QFile>

#define ORIGION_X   116.630758035               //原点横坐标
#define ORIGION_Y   23.41659924                 //原点纵坐标
#define JD_SPACE    (1.16909 * 1e-5)            //横坐标变化一个单位所对应的经度变化
#define WD_SPACE    (1.0459 * 1e-5)             //纵坐标变化一个单位所对应的纬度变化

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 新建场景
    QGraphicsScene *scene = new QGraphicsScene;

    QPixmap *pixmap = new QPixmap(QPixmap("/app/stu.jpg"));     //for tq2440

    MyItem *imageItem = new MyItem(pixmap);
    scene->addItem(imageItem);

/*
    rectItem = new QGraphicsRectItem(0, 0, 7, 7);
    rectItem->setRect(0, 0, 5, 5);
    scene->addItem(rectItem);
*/
    //或者
    rectItem = scene->addRect(QRect(0, 0, 7, 7), QPen(Qt::blue), QBrush(Qt::red));

    view = new QGraphicsView(this);
    view->setScene(scene);
    view->setDragMode(QGraphicsView::ScrollHandDrag);           //手抓模式
    view->resize(400, 300);

    ui->gridLayout->addWidget(view);

    QIcon icon;
    icon.addFile(tr("/app/gps/zoom_in.ico"));                   //for tq2440
    ui->pushButton_enlagre->setIcon(icon);
    icon.addFile(tr("/app/gps/zoom_out.ico"));

    uart1_init((char *)UART_NAME, 38400);

    ui->pushButton_reduce->setIcon(icon);

    timer = new QTimer(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//设置滚动条
void MainWindow::setScrollBar(int x, int y)
{
    //滚动条左移
    if (x <= -165 && x > -193)
        view->horizontalScrollBar()->setSliderPosition(-250);
    if (x <= -193 && x > -240)
        view->horizontalScrollBar()->setSliderPosition(-350);
    if (x <= -240 && x > -300)
        view->horizontalScrollBar()->setSliderPosition(-450);
    if (x <= -300 && x > -532)
        view->horizontalScrollBar()->setSliderPosition(-532);

    //滚动条上移
    if (y <= -82 && y > -239)
        view->verticalScrollBar()->setSliderPosition(-250);
    if (y <= -239 && y > -300)
        view->verticalScrollBar()->setSliderPosition(-350);
    if (y <= -300 && y > -350)
        view->verticalScrollBar()->setSliderPosition(-450);
    if (y <= -350 && y > -532)
        view->verticalScrollBar()->setSliderPosition(-550);

    //滚动条右移
    if (x >= 171 && x < 390)
        view->horizontalScrollBar()->setSliderPosition(48);
    if (x >= 390 && x < 420)
        view->horizontalScrollBar()->setSliderPosition(96);
    if (x >= 420 && x < 470)
        view->horizontalScrollBar()->setSliderPosition(144);
    if (x >= 470)
        view->horizontalScrollBar()->setSliderPosition(189);

    //滚动条下移
    if (y >= 87 && y < 264)
        view->verticalScrollBar()->setSliderPosition(88);
    if (y >= 264 && y < 350)
        view->verticalScrollBar()->setSliderPosition(176);
    if (y >= 350 && y < 440)
        view->verticalScrollBar()->setSliderPosition(274);
    if (y >= 440)
        view->verticalScrollBar()->setSliderPosition(352);
}

//放大
void MainWindow::on_pushButton_enlagre_clicked()
{
    timer->stop();
    view->scale(1.2, 1.2);
}

//缩小
void MainWindow::on_pushButton_reduce_clicked()
{
    timer->stop();
    view->scale(1 / 1.2, 1 / 1.2);
}

//恢复默认大小
void MainWindow::on_pushButton_resume_clicked()
{
    timer->stop();
    view->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    QMatrix q;
    q.setMatrix(1, view->matrix().m12(), view->matrix().m21(), 1,
                view->matrix().dx(), view->matrix().dy());
    view->setMatrix(q, false);
}

void MainWindow::locate()
{
    static double jd = ORIGION_X, wd = ORIGION_Y;

    //for tq2440
    nmea_msg gpsx; 											//GPS信息
    uart1_read(USART1_TX_BUF, USART1_MAX_RECV_LEN);
    qDebug("USART1_TX_BUF = %s", USART1_TX_BUF);
    NMEA_GPRMC_Analysis(&gpsx, (u8*)USART1_TX_BUF);         //解析GPRMC
    jd = (double)gpsx.longitude / 1000000;                  //经度
    wd = (double)gpsx.latitude / 1000000;                   //纬度

    qDebug("jd = %.8f  wd = %.8f", jd, wd);

    int x, y;
    if (jd > ORIGION_X)
        x = (int)((jd - ORIGION_X) / JD_SPACE);
    else
        x = -(int)((ORIGION_X - jd) / JD_SPACE);
    if (wd > ORIGION_Y)
        y = -(int)((wd - ORIGION_Y) / WD_SPACE);
    else
        y = (int)((ORIGION_Y - wd) / WD_SPACE);
    qDebug("x = %d  y = %d", x, y);
    if (x < -532 || x > 532 || y < -532 || y > 532)
    {
        QMessageBox::critical(0, "Not in STU!",
                              "Please navigate in STU!",
                              QMessageBox::Cancel);
        return;
    }
    setScrollBar(x, y);
    rectItem->setPos(0, 0);
    rectItem->moveBy(x, y);
}

//定位的槽函数
void MainWindow::on_pushButton_locate_clicked()
{
    timer->stop();                                      //停止定时器
    locate();
}

//导航
void MainWindow::on_pushButton_navigation_clicked()
{
    connect(timer, SIGNAL(timeout()), this, SLOT(timerUpDate()));
    timer->start(10000);                                //每隔10s执行一次timerUpDate()
}

void MainWindow::timerUpDate()
{
    locate();
}

int connectFlag;                                //创建数据库连接的标志
//显示气象信息
void MainWindow::on_pushButton_weather_clicked()
{
    //防止数据库中的汉字出现乱码
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

    if (!connectFlag)                           //没有创建数据库连接
    {
        if (!createConnection())
            return;
        connectFlag = 1;                        //标记创建数据库连接
    }
    MyDialog *dialog = new MyDialog(this);
    dialog->show();
}

extern char dateStr[20];
extern char weekStr[20];
//信息存储为txt
void MainWindow::on_pushButton_storage_clicked()
{
    if (!connectFlag)                           //没有创建数据库连接
    {
        if (!createConnection())
            return;
        connectFlag = 1;                        //标记创建数据库连接
    }

    QFile file("/sensor.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    file.write("日期:  ");
    file.write(dateStr);
    file.write("      星期:  ");
    file.write(weekStr);
    const char *tableHead = "\n时间        温度/℃    湿度/%    气压/Pa   "
                            "海拔/m   风速/m/s\n";
    file.write(tableHead);

    QSqlQuery query;
    query.exec("select * from sensor");
    qDebug() << "default connection:";
    char data[20], *str_p;
    QByteArray array, temp;
    while (query.next())                //遍历数据库
    {
        //将QString转化为char *
        array = query.value(0).toString().toUtf8();
        str_p = array.data();           //时间
        file.write(str_p);              //文件写入

        sprintf(data, "     %d     ", query.value(1).toInt());  //温度
        file.write(data);

        array = query.value(2).toString().toUtf8();
        for (int i = 0; i < 2; i++)     //截取前2个字符(最后一个字符是‘%’)
            temp[i] = array[i];
        str_p = temp.data();            //湿度
        file.write(str_p);              //文件写入

        sprintf(data, "        %d", query.value(3).toInt());    //气压
        file.write(data);

        sprintf(data, "    %.3f", query.value(4).toDouble());   //海拔
        file.write(data);

        sprintf(data, "    %.3f", query.value(5).toDouble());   //风速
        file.write(data);

        file.write("\n");
    }
    file.close();

    //提示写入完成
    QMessageBox::information(this, tr("AlertDialog"),
                            tr("Writing complete!"),
                            QMessageBox::Ok);
}
