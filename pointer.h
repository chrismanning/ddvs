#ifndef POINTER_H
#define POINTER_H

#include <QGraphicsItem>
#include <QLineF>
#include <datatype.h>

class Pointer : public DataType
{
public:
    Pointer(const QString &type, const QString &name, DataType *pointsTo);
    ~Pointer();
    int pointTo(DataType *a);
    QGraphicsLineItem *link;
    DataType *pointsTo;

signals:

public slots:

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);

private:
};

#endif // POINTER_H
