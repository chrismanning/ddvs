#include <graphicsitems.h>

namespace Graphics
{

Variable::Variable(const std::string name, const int &value, const std::string type_str)
    : name(name), value(value), type_str(type_str), textHeight(0), textWidth(0)
{
    setFlags(ItemIsSelectable | ItemSendsGeometryChanges);
}

void Variable::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget*)
{
    QPen pen(QColor(0,0,0));
    if(option->state & QStyle::State_Selected) {
        pen.setStyle(Qt::DotLine);
        //pen.setWidth(2);
    }
    QString str = QString::fromStdString(type_str) + " : " + QString::number(value);
    if(textHeight == 0 || textWidth == 0) {
        auto fm = painter->fontMetrics();
        textHeight = fm.height();
        textWidth = fm.width(str);
        prepareGeometryChange();
        if(QString::fromStdString(name).length() > str.length()) {
            textWidth = fm.width(QString::fromStdString(name));
        }
        qDebug() << "height:" << textHeight;
        qDebug() << "width:" << textWidth;
    }
    str += "\n" + QString::fromStdString(name);
    this->setToolTip(str);
    painter->setPen(pen);
    painter->setBrush(QColor(200, 200, 200));
    painter->drawRoundedRect(boundingRect(), 2.5, 2.5);
    painter->drawText(boundingRect(), Qt::AlignCenter, str);
}

QRectF Variable::boundingRect() const
{
    int padding = 5;
    int width = 80;
    if(textWidth > width) {
        width = textWidth;
    }
    int height = 40;
    if(textHeight * 2 > height) {
        height = textHeight * 2;
    }
    QRectF rect(0, 0, width+(2*padding), height);
    return rect;
}

void Pointer::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
    Variable::paint(painter, item, widget);

    auto name = QString::fromStdString(findByValue(value));
    if(name.size() <= 0) {
        name = QString::fromStdString(type_str);
        name.chop(1);
        name += QString::number(value);
    }
    qDebug() << "pointer name:" << name;
    if(items.contains(name)) {
        qDebug() << "Setting pointer line";
        link->setLine(QLineF(this->pos(), items[name]->pos()));
        link->show();
    }
    else {
        link->hide();
    }
}

std::string const Pointer::findByValue(const int value)
{
    for(auto const& var : vars) {
        if(var.second == value) {
            return var.first;
        }
    }
    return "";
}

Struct::Struct(std::string const name,
       std::string const type_str,
       std::list<member_container> members)
    : name(name), type_str(type_str), textBounds(0)
{
    setFlags(ItemIsSelectable | ItemSendsGeometryChanges);
    for(member_container const& mem : members) {
        this->members.insert(QString::fromStdString(mem.name),
                             MemberContainer
                             (QString::fromStdString(mem.type_name),
                              mem.value));
    }
}

QRectF Struct::boundingRect() const
{
    int padding = 5;
    int height = 40;
    if(textHeight * (3 + members.size()) > height) {
        height = textHeight * (3 + members.size());
    }
    int width = 80;
    if(textWidth > width) {
        width = textWidth;
    }
    QRectF rect(0, 0, width + (padding*2), height);
    return rect;
}

void Struct::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget*)
{
    QPen pen(QColor(0,0,0));
    if(option->state & QStyle::State_Selected) {
        pen.setStyle(Qt::DotLine);
        //pen.setWidth(2);
    }
    if(textHeight == 0 || textWidth == 0) {
        auto fm = painter->fontMetrics();
        textHeight = fm.height();
        prepareGeometryChange();
    }
    QString str = QString::fromStdString(type_str);
    str += "\n" + QString::fromStdString(name) + "\nmembers:\n";
    auto fm = painter->fontMetrics();
    for(auto i = members.constBegin(); i != members.constEnd(); ++i) {
        str += i.value().type_name + " " + i.key()
                + " : " + QString::number(i.value().value) + "\n";
    }
    this->setToolTip(str);
    painter->setPen(pen);
    painter->setBrush(QColor(200, 200, 200));
    painter->drawRoundedRect(boundingRect(), 2.5, 2.5);
    painter->drawText(boundingRect(), Qt::AlignCenter, str);
}

}
