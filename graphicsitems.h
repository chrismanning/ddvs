#ifndef GRAPHICSITEMS_H
#define GRAPHICSITEMS_H

#include <QGraphicsItem>
#include <QGraphicsWidget>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

namespace Graphics
{

class Variable : public QGraphicsWidget
{
public:
    Variable(std::string const& name, int const& value, std::string const type_str);
    ~Variable() {}
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);
protected:
//    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint = QSizeF()) const
    {
        return boundingRect().size();
    }
    std::string const& name;
    int const& value;
    std::string const type_str;
    int textHeight;
    int textWidth;
};

class Pointer : public Variable
{
public:
    Pointer(std::string const& name,
            int const& value,
            std::string const type_str,
            std::map<std::string, int> const& vars,
            QHash<QString, QGraphicsWidget*> const& items,
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
    QHash<QString, QGraphicsWidget*> const& items;
    QGraphicsLineItem* link;
};

class Struct : public QGraphicsItem
{
public:
    Struct();
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);
protected:
//    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
};
}

#endif // GRAPHICSITEMS_H
