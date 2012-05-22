#include <graphicsitems.h>

namespace Graphics
{

Variable::Variable(const std::string name, const int &value, const std::string type_str)
    : name(name)
    , value(value)
    , textHeight(0)
    , textWidth(0)
{
    setFlags(ItemIsSelectable | ItemSendsGeometryChanges);
    this->type_str = type_str;
    //setPreferredSize(80,40);
    //setMinimumSize(80,40);
}

void Variable::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget*)
{
    QColor base_colour(0,0,0);
    if(option->state & QStyle::State_Selected) {
        base_colour.setRgb(150,150,150);
    }
    QPen pen(base_colour);
    QString str = QString::fromStdString(type_str) + " : " + QString::number(value);
    if(textHeight == 0 || textWidth == 0) {
        auto fm = painter->fontMetrics();
        textHeight = fm.height();
        textWidth = fm.width(str);
        prepareGeometryChange();
        if(QString::fromStdString(name).length() > str.length()) {
            textWidth = fm.width(QString::fromStdString(name));
        }
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
    if(items.contains(name)) {
        link.setPoints(pos(), items[name]->pos());
    }
    else {
        link.unSet();
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
    : name(name), textHeight(0), textWidth(0), fm(QFont())
{
    this->type_str = type_str;
    setFlags(ItemIsSelectable | ItemSendsGeometryChanges);
    for(member_container const& mem : members) {
        this->members.insert(QString::fromStdString(mem.name),
                             MemberContainer
                             (QString::fromStdString(mem.type_name),
                              mem.value));
    }
    //setPreferredSize(80,45);
    //setMinimumSize(80,45);
    updateString();
}

void Struct::updateString()
{
    str = QString::fromStdString(type_str)
            + "\n" + QString::fromStdString(name)
            + "\nmembers:";
    for(auto i = members.constBegin(); i != members.constEnd(); ++i) {
        str += "\n" + i.value().type_name + " " + i.key()
                + " : " + QString::number(i.value().value);
    }
    this->setToolTip(str);
}

QRectF Struct::boundingRect() const
{
    int padding = 5;
    int height = 45;
    auto r = textHeight * (3 + members.size());
    if(r > height) {
        height = r;
    }
    int width = 80;
    if(textWidth > width) {
        //width = textWidth;
    }
    QRectF rect(0, 0, width + (padding*2), height);
    return rect;
}

void Struct::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget*)
{
    QColor base_colour(0,0,0);
    if(option->state & QStyle::State_Selected) {
        base_colour.setRgb(150,150,150);
    }
    QPen pen(base_colour);
    if(textHeight == 0) {
        fm = painter->fontMetrics();
        textHeight = fm.height();
        textWidth = fm.width(QString::fromStdString(type_str)
                             + "\n" + QString::fromStdString(name)
                             + "\nmembers:\n");
        prepareGeometryChange();
    }

    painter->setPen(pen);
    painter->setBrush(QColor(200, 200, 200));
    painter->drawRoundedRect(boundingRect(), 2.5, 2.5);
    painter->drawText(boundingRect(), Qt::AlignHCenter, str);
    setMinimumSize(boundingRect().size());
}

}
