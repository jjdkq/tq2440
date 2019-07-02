#include "myitem.h"
#include <QPainter>

MyItem::MyItem(QPixmap *pixmap)
{
    pix = *pixmap;
    setAcceptDrops(true);   //设置可拖拽
}

//重写boundingRect(),用来限制绘制图形项的矩形区域
QRectF MyItem::boundingRect() const
{
    return QRectF(-pix.width() / 2, -pix.height() / 2,
                  pix.width(), pix.height());
}

//重写paint(),执行绘图操作
void MyItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->drawPixmap(-pix.width() / 2, -pix.height() / 2, pix);
}

