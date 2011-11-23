#ifndef DATATYPE_H
#define DATATYPE_H

#include <QGraphicsItem>
#include <QStringList>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

struct Member {
    Member(const QString &name, const QString &value) {
        this->name = name;
        this->value = value;
    }

    QString name;
    QString value;
};

class DataType : public QGraphicsItem
{
public:
    DataType(const QString &type = "int", int x=0, int y=0);
    DataType(const QString &type, const QString &name, const QString &value);

    QRectF boundingRect() const;
//    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

protected:
    QRectF createRect(qreal padding = 0) const;
//    void mousePressEvent(QGraphicsSceneMouseEvent *event);
//    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
//    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    QString type;
    QList<Member*> members;
    int x, y;
};

#endif // DATATYPE_H
