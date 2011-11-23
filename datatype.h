#ifndef DATATYPE_H
#define DATATYPE_H

#include <QGraphicsItem>
#include <QStringList>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

class DataType : public QGraphicsItem
{
public:
    DataType(const QString &type = "int", int x=0, int y=0);
    DataType(const QString &type, const QString &name, const QString &value);

    QRectF boundingRect() const;
//    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

protected:
//    void mousePressEvent(QGraphicsSceneMouseEvent *event);
//    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
//    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    QString name;
    QString value;
    QString type;
    int x, y;
};

#endif // DATATYPE_H
