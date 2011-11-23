#include "datatype.h"

DataType::DataType(const QString &type, int x, int y)
{
    setFlags(ItemIsMovable | ItemIsSelectable);

    this->type = type;

    qDebug("Inside Datatype constructor");
}

void DataType::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen pen(QColor(0,0,0));
    //QPen pen(myOutlineColor);
    if (option->state & QStyle::State_Selected) {
        pen.setStyle(Qt::DotLine);
        pen.setWidth(2);
    }
    painter->setPen(pen);
    painter->setBrush(QColor(100,100,100));
    //QRectF rect = outlineRect();
    painter->drawRoundedRect(QRectF(0,0,100,200),20,20);
    //painter->setPen(myTextColor);
    //painter->drawText(rect, Qt::AlignCenter, myText);
}

QRectF DataType::boundingRect() const
{
    return QRectF(0,0,100,200);
}

//QPainterPath DataType::shape() const
//{
//    QPainterPath path;
//    path.addRoundedRect(x,y,100,200,20,20);
//    return path;
//}

//Types::dataTypes("int", "*int", "struct", "*struct");
//"int" << "*int" << "struct" << "*struct"
