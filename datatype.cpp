#include "datatype.h"

DataType::DataType(const QString &type, const QString &name, const QString &value)
{
    setFlags(ItemIsMovable | ItemIsSelectable);
    this->type = type;
    members << new Member(name,value);
//    this->name = name;
//    this->value = value;
}

void DataType::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen pen(QColor(0,0,0));
    if (option->state & QStyle::State_Selected) {
        pen.setStyle(Qt::DotLine);
        //pen.setWidth(2);
    }
    painter->setPen(pen);
    painter->setBrush(QColor(200,200,200));
    painter->drawRoundedRect(createRect(),5,5);
    //painter->setPen(textColor);
    foreach(Member *member, members) {
        painter->drawText(createRect(5), Qt::AlignHCenter, member->name + " : " + member->value);
    }
    //painter->drawText(createRect(5), Qt::AlignHCenter|Qt::AlignBottom, members[0]->value);
    painter->drawText(createRect(5), Qt::AlignHCenter|Qt::AlignBottom, type);
}

QRectF DataType::boundingRect() const
{
    return createRect();
}

QRectF DataType::createRect(qreal padding) const
{
    qreal textHeight = 24;
    qreal width = 100;
    qreal height = textHeight + textHeight * members.size();
    QRectF rect(0+padding,0+padding,width-(2*padding),height-(2*padding));
    return rect;
}

//QPainterPath DataType::shape() const
//{
//    QPainterPath path;
//    path.addRoundedRect(x,y,100,200,20,20);
//    return path;
//}

//Types::dataTypes("int", "*int", "struct", "*struct");
//"int" << "*int" << "struct" << "*struct"
