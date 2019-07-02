#include "mywidget.h"
#include <QApplication>
#include <QTextCodec>

//注意：主函数中不能用while(1)，否则GUI会卡死
//使用定时器来刷新数据
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec *tc = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(tc);
    MyWidget w;
    w.show();
    return a.exec();
}
