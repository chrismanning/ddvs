#include "datatype.h"
#include "pointer.h"

DataType::DataType(const QString &type, const QString &name, const QString &value)
{
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
    this->type = type;
    this->name = name;
    //members << new Member(name,value);
//    this->name = name;
    this->value = value;
}

DataType::~DataType()
{
    foreach(DataType *p, pointers) {
        ((Pointer*)p)->link->hide();
        ((Pointer*)p)->pointsTo = 0;
    }
}

void DataType::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget*)
{
    QPen pen(QColor(0,0,0));
    if(option->state & QStyle::State_Selected) {
        pen.setStyle(Qt::DotLine);
        //pen.setWidth(2);
    }
    painter->setPen(pen);
    painter->setBrush(QColor(200,200,200));
    painter->drawRoundedRect(createRect(),5,5);
    //painter->setPen(textColor);
//    foreach(Member *member, members) {
//        painter->drawText(createRect(5), Qt::AlignHCenter, member->name + " : " + member->value);
//    }
    painter->drawText(createRect(5), Qt::AlignHCenter|Qt::AlignTop, name + " : " + value);
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
    int members = this->members.size() > 0 ? this->members.size() : 1;
    qreal height = textHeight + textHeight * members;
    QRectF rect(0+padding,0+padding,width-(2*padding),height-(2*padding));
    return rect;
}

QVariant DataType::itemChange(GraphicsItemChange change, const QVariant &value)
{
    //qDebug("Item changing...");
    if(change == QGraphicsItem::ItemPositionHasChanged) {
        //qDebug("Item moving...");
        foreach(DataType* p, pointers) {
            ((Pointer*)p)->link->setLine(QLineF(p->pos(),this->pos()));
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

int pointTo(DataType * /*a*/)
{
    return -1;
}

//QPainterPath DataType::shape() const
//{
//    QPainterPath path;
//    path.addRoundedRect(x,y,100,200,20,20);
//    return path;
//}

//Types::dataTypes("int", "*int", "struct", "*struct");
//"int" << "*int" << "struct" << "*struct"
