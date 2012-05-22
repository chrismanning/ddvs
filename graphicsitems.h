#ifndef GRAPHICSITEMS_H
#define GRAPHICSITEMS_H

#include <QGraphicsItem>
#include <QGraphicsWidget>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <ast.h>
#include <interpreter.h>
#include <tuple>

namespace Graphics
{

class GraphicsWidget : public QGraphicsWidget
{
public:
    std::string type_str;
};

class Variable : public GraphicsWidget
{
public:
    Variable(std::string const name, int const& value, std::string const type_str);
    ~Variable() {}
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);
    std::string const name;
protected:
//    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    QSizeF sizeHint(Qt::SizeHint /*which*/, const QSizeF& /*constraint*/) const
    {
        return boundingRect().size();
    }
    int const& value;
    int textHeight;
    int textWidth;
};

class PointerLine : public QGraphicsItem
{
public:
    PointerLine() : isSet(false)
    {}
    PointerLine(QPointF const& a, QPointF const& b) : a(a), b(b + QPointF(5,5)), isSet(true)
    {}
    void setPoints(QPointF const& a, QPointF const& b) {
        this->a = a;
        this->b = b + QPointF(5,5);
        isSet = true;
    }

    void unSet() {
        isSet = false;
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem */*item*/, QWidget */*widget*/)
    {
        if(isSet) {
            QPen pen(QColor(100,100,100,200));
            painter->setPen(pen);
            QPainterPath path;
            path.moveTo(a);
            QPointF middle;
            if(a.x() > b.x()) {
                middle.setX(a.x() - b.x()/2);
            }
            else {
                middle.setX(b.x() - a.x()/2);
            }
            if(a.y() > b.y()) {
                middle.setY(a.y() - b.y()/2 - 10);
            }
            else {
                middle.setY(b.y() - a.y()/2 - 10);
            }
            path.quadTo(middle, b);
            painter->drawPath(path);
            QPolygonF triangle;
            triangle << b;
            triangle << b-QPointF(0,3);
            triangle << b-QPointF(3,0);
            painter->drawPolygon(triangle);
        }
    }
    QRectF boundingRect() const
    {
        return QRectF(a,b);
    }

private:
    QPointF a,b;
    bool isSet;
};

class Pointer : public Variable
{
public:
    Pointer(std::string const name,
            int const& value,
            std::string const type_str,
            std::map<std::string, int> const& vars,
            QHash<QString, GraphicsWidget*> const& items)
        : Variable(name, value, type_str),
          vars(vars),
          items(items)
    {}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);
    PointerLine * getLink() {
        return &link;
    }

protected:
//    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
private:
    std::string const findByValue(const int value);
    std::map<std::string, int> const& vars;
    QHash<QString, GraphicsWidget*> const& items;
    PointerLine link;
};

struct member_container
{
    member_container(const std::string name,
                     const std::string type_name,
                     const int& value)
        : value(value), name(name), type_name(type_name)
    {}
    const int& value;
    const std::string name;
    const std::string type_name;
};

struct MemberContainer
{
    MemberContainer(QString type_name, const int & value)
        : type_name(type_name), value(value)
    {}
    MemberContainer(MemberContainer const& b)
        : value(b.value)
    {
        type_name = b.type_name;
    }
    MemberContainer& operator=(MemberContainer const& b)
    {
        type_name = b.type_name;
        return *this;
    }

    QString type_name;
    const int& value;
};

class Struct : public GraphicsWidget
{
public:
    Struct(std::string const name,
           std::string const type_str,
           std::list<member_container> members);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);
    void updateString();
protected:
    QSizeF sizeHint(Qt::SizeHint /*which*/, const QSizeF & /*constraint*/) const
    {
        return boundingRect().size();
    }
//    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
private:
    QMap<QString, MemberContainer> members;
    std::string const name;
    QString str;
    int textHeight;
    int textWidth;
    QFontMetrics fm;
};
}

#endif // GRAPHICSITEMS_H
