#ifndef MYITEM_H
#define MYITEM_H

#include <QGraphicsItem>
#include <QPixmap>

#include <QMouseEvent>

class MyItem : public QGraphicsItem
{
public:
    MyItem(QPixmap *pixmap);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

private:
    QPixmap pix;
};

#endif // MYITEM_H
