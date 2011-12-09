#include "pointer.h"

Pointer::Pointer(const QString &type, const QString &name, DataType *pointsTo) : DataType(type,name)
{
    this->pointsTo = pointsTo;
    link = new QGraphicsLineItem;
    pointTo(pointsTo);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

Pointer::~Pointer()
{
    if(pointsTo != 0) {
        pointsTo->pointers.removeAll(this);
    }
    delete link;
}

int Pointer::pointTo(DataType *a)
{
    pointsTo = a;
    if(pointsTo != 0) {
        link->setLine(QLineF(this->pos(),pointsTo->pos()));
        link->show();
        pointsTo->pointers << this;
    }

    return 0;
}

QVariant Pointer::itemChange(GraphicsItemChange change, const QVariant &value)
{
    qDebug("Item changing...");
    if(change == QGraphicsItem::ItemPositionHasChanged) {
        qDebug("Item moving...");
        if(pointsTo != 0 && link != 0)
            link->setLine(QLineF(this->pos(),pointsTo->pos()));
    }
    return QGraphicsItem::itemChange(change, value);
}
