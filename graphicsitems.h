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
    int const& value;
protected:
//    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint = QSizeF()) const
    {
        return boundingRect().size();
    }
    std::string const name;
    int textHeight;
    int textWidth;
};

class Pointer : public Variable
{
public:
    Pointer(std::string const name,
            int const& value,
            std::string const type_str,
            std::map<std::string, int> const& vars,
            QHash<QString, GraphicsWidget*> const& items,
            QGraphicsLineItem* link)
        : Variable(name, value, type_str),
          vars(vars),
          items(items),
          link(link)
    {}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);
protected:
//    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
private:
    std::string const findByValue(const int value);
    std::map<std::string, int> const& vars;
    QHash<QString, GraphicsWidget*> const& items;
    QGraphicsLineItem* link;
};

struct member_container
{
    member_container(const std::string name,
                     const std::string type_name,
                     const int& value)
        : value(value), name(name), type_name(type_name)
    {}
    const std::string name;
    const std::string type_name;
    const int& value;
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
protected:
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF & constraint = QSizeF()) const
    {
        return boundingRect().size();
    }
//    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
private:
    QRectF* textBounds;
    QMap<QString, MemberContainer> members;
    QString buildString();
    std::string const name;
    int textHeight;
    int textWidth;
};
}

#endif // GRAPHICSITEMS_H
